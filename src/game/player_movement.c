#include "player_movement.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <log/log.h>
#include <math.h>

#include "../math/math.h"
#include "autosave.h"
#include "bot.h"

// function fires a shot from currPlayer and detects any hits to enemyPlayer
void shoot(App* app, Player* firstPlayer, Player* secondPlayer,
           RenderObject* projectile, RenderObject* explosion, int* heightMap,
           SDL_bool* regenMap) {
  if (app->currState != PLAY) return;

  Mix_PlayChannel(-1, app->sounds[0], 0);

  // determining which player is the current enemy
  Player* enemyPlayer;
  if (app->currPlayer == firstPlayer) {
    enemyPlayer = secondPlayer;
  } else {
    enemyPlayer = firstPlayer;
  }

  // printf("[\e[36mDEBUG\e[0m][\e[1;32mINFO\e[0m] currPlayer angle: %.0f\n", app->currPlayer->tankAngle);

  // printf("[\e[36mDEBUG\e[0m][\e[1;32mINFO\e[0m] currPlayer pos: [%.0f, %.0f]\n",
  //     app->currPlayer->tankObj->data.texture.constRect.x * app->scalingFactorX,
  //     app->currPlayer->tankObj->data.texture.constRect.x * app->scalingFactorX + app->currPlayer->tankObj->data.texture.constRect.w * app->scalingFactorX * cos(DEGTORAD(app->currPlayer->tankAngle))
  // );

  // printf("[\e[36mDEBUG\e[0m][\e[1;32mINFO\e[0m] enemyPlayer angle: %.0f\n", enemyPlayer->tankAngle);
  // printf("[\e[36mDEBUG\e[0m][\e[1;32mINFO\e[0m] enemyPlayer width: %d\n", enemyPlayer->tankObj->data.texture.constRect.w);

  // printf("[\e[36mDEBUG\e[0m][\e[1;32mINFO\e[0m] enemyPlayer pos: [%.0f, %.0f]\n",
  //     enemyPlayer->tankObj->data.texture.constRect.x * app->scalingFactorX,
  //     enemyPlayer->tankObj->data.texture.constRect.x * app->scalingFactorX + enemyPlayer->tankObj->data.texture.constRect.w * app->scalingFactorX * cos(DEGTORAD(enemyPlayer->tankAngle))
  // );
  //

  // that angle is clockwise
  double currGunAngle = app->currPlayer->tankGunObj->data.texture.angle;

  // calculating angle specifically for a current player
  if (app->currPlayer == secondPlayer) {
    currGunAngle += 180 - app->currPlayer->tankGunObj->data.texture.angleAlt;
  } else {
    currGunAngle += app->currPlayer->tankGunObj->data.texture.angleAlt;
  }

  // normalizing just to be sure its in [0;2pi) and now its counterclockwise
  currGunAngle = 360 - normalizeAngle(currGunAngle);

  // thats an init pos of a curr player tank
  SDL_Point tempPos = getPixelScreenPosition(
      (SDL_Point){app->currPlayer->tankObj->data.texture.scaleRect.x,
                  app->currPlayer->tankObj->data.texture.scaleRect.y},
      (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
      app->currPlayer->tankObj->data.texture.angle,
      (SDL_Point){24 * app->scalingFactorX, 7 * app->scalingFactorY});

  SDL_FPoint initPos = {
      .x = tempPos.x,
      .y = tempPos.y,
  };

  SDL_FPoint relativePos = {
      .x = 0.0f,
      .y = 0.0f,
  };

  SDL_Point savedPos = {
      .x = (int)initPos.x,
      .y = (int)initPos.y,
  };

  initPos.x /= app->scalingFactorX;
  initPos.y /= app->scalingFactorY;

  // initpos now points to the edge of the tank gun
  initPos.x += 25 * cos(DEGTORAD(currGunAngle));
  initPos.y -= 25 * sin(DEGTORAD(currGunAngle));

  // printf("initpos: x: %lf y: %lf, angle: %lf\n", initPos.x, initPos.y, currGunAngle);
  //

  // showing projectile
  projectile->disableRendering = SDL_FALSE;

  // select the time interval for recalculations
  const double dt = 1 / 10.0;
  double currTime = 0.0;

  double vel;
  int explosionRadius;
  SDL_bool isHittableNearby;
  double damageMultiplier;
  switch (app->currWeapon) {
    // small bullet
    case 0:
      vel = app->currPlayer->firingPower * 2;
      explosionRadius = 0;
      isHittableNearby = SDL_FALSE;
      damageMultiplier = 1.5;
      break;
    // BIG BULLET
    case 1:
      vel = app->currPlayer->firingPower * 1.75;
      explosionRadius = projectile->data.texture.constRect.w;
      isHittableNearby = SDL_FALSE;
      damageMultiplier = 2.0;
      break;
    // small boom
    case 2:
      vel = app->currPlayer->firingPower * 1.25;
      explosionRadius = projectile->data.texture.constRect.w * 2;
      isHittableNearby = SDL_TRUE;
      damageMultiplier = 1.25;
      break;
    // BIG BOOM
    case 3:
      vel = app->currPlayer->firingPower * 1;
      explosionRadius = projectile->data.texture.constRect.w * 4;
      isHittableNearby = SDL_TRUE;
      damageMultiplier = 1.75;
      break;
    default:
      vel = app->currPlayer->firingPower;
      explosionRadius = projectile->data.texture.constRect.w;
      isHittableNearby = SDL_FALSE;
      damageMultiplier = 1;
      break;
  }

  // printf("initVel:%lf\n", vel);
  //

  // player collision center and radiuses
  SDL_Point collisionP1, collisionP2, collisionP3;
  int collisionP1R, collisionP2R, collisionP3R;
  // getting player collisions centers
  if (app->currPlayer == secondPlayer) {
    collisionP1 = getPixelScreenPosition(
        (SDL_Point){enemyPlayer->tankObj->data.texture.scaleRect.x,
                    enemyPlayer->tankObj->data.texture.scaleRect.y},
        (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemyPlayer->tankObj->data.texture.angle,
        (SDL_Point){35 * app->scalingFactorX, 19 * app->scalingFactorY});
    collisionP2 = getPixelScreenPosition(
        (SDL_Point){enemyPlayer->tankObj->data.texture.scaleRect.x,
                    enemyPlayer->tankObj->data.texture.scaleRect.y},
        (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemyPlayer->tankObj->data.texture.angle,
        (SDL_Point){20 * app->scalingFactorX, 12 * app->scalingFactorY});
    collisionP3 = getPixelScreenPosition(
        (SDL_Point){enemyPlayer->tankObj->data.texture.scaleRect.x,
                    enemyPlayer->tankObj->data.texture.scaleRect.y},
        (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemyPlayer->tankObj->data.texture.angle,
        (SDL_Point){7 * app->scalingFactorX, 19 * app->scalingFactorY});
    // radiuses
    collisionP1R = 9 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP2R = 13 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP3R = 10 * MAX(app->scalingFactorX, app->scalingFactorY);
  }
  // right player collison
  else {
    collisionP1 = getPixelScreenPosition(
        (SDL_Point){enemyPlayer->tankObj->data.texture.scaleRect.x,
                    enemyPlayer->tankObj->data.texture.scaleRect.y},
        (SDL_Point){8 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemyPlayer->tankObj->data.texture.angle,
        (SDL_Point){11 * app->scalingFactorX, 19 * app->scalingFactorY});
    collisionP2 = getPixelScreenPosition(
        (SDL_Point){enemyPlayer->tankObj->data.texture.scaleRect.x,
                    enemyPlayer->tankObj->data.texture.scaleRect.y},
        (SDL_Point){8 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemyPlayer->tankObj->data.texture.angle,
        (SDL_Point){23 * app->scalingFactorX, 13 * app->scalingFactorY});
    collisionP3 = getPixelScreenPosition(
        (SDL_Point){enemyPlayer->tankObj->data.texture.scaleRect.x,
                    enemyPlayer->tankObj->data.texture.scaleRect.y},
        (SDL_Point){8 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemyPlayer->tankObj->data.texture.angle,
        (SDL_Point){36 * app->scalingFactorX, 18 * app->scalingFactorY});
    // radiuses
    collisionP1R = 11 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP2R = 11 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP3R = 9 * MAX(app->scalingFactorX, app->scalingFactorY);
  }

  // main shooting loop
  while (app->currState == PLAY) {
    currTime += dt;

    // getting coords with respect to the init pos
    getPositionAtSpecTime(&relativePos, vel, currGunAngle, currTime);

    int currX = initPos.x + relativePos.x;
    int currY = initPos.y - relativePos.y;

    // calculating angle between the old projectile pos and the new 1
    double currAngle =
        360 - atan2(savedPos.y - currY, currX - savedPos.x) * 180.0 / M_PI;

    // printf("angle: %lf\n", currAngle);
    //

    // now this is a current projectile position
    savedPos = (SDL_Point){
        .x = currX,
        .y = currY,
    };

    int currMapHeight =
        app->screenHeight - heightMap[(int)round(currX * app->scalingFactorX)];

    //printf("cmh:%d, (%d %d)\n", currMapHeight, currX, currY);

    projectile->data.texture.constRect.x = currX;
    projectile->data.texture.constRect.y = currY;
    projectile->data.texture.angle = currAngle;

    // out of bounds check
    if (currX < 0 || currX * app->scalingFactorX >= app->screenWidth ||
        currY * app->scalingFactorY > app->screenHeight) {
      //     printf("out of bounds!\n");
      //
      break;
    }

    int currXScaled = currX * app->scalingFactorX;
    int currYScaled = currY * app->scalingFactorY;
    if (isInCircle(currXScaled, currYScaled, &collisionP1, collisionP1R) ||
        isInCircle(currXScaled, currYScaled, &collisionP2, collisionP2R) ||
        isInCircle(currXScaled, currYScaled, &collisionP3, collisionP3R)) {
      //     printf("COLLISION HIT!\n");
      //

      // if we hit enemy straight
      app->currPlayer->score +=
          abs(currX - enemyPlayer->tankObj->data.texture.constRect.x -
              enemyPlayer->tankObj->data.texture.constRect.w / 2) *
          damageMultiplier;

      if (isHittableNearby == SDL_FALSE) {
        explosion->data.texture.constRect.h =
            projectile->data.texture.constRect.h;
        explosion->data.texture.constRect.w =
            projectile->data.texture.constRect.w;
      } else {
        explosion->data.texture.constRect.h = explosionRadius;
        explosion->data.texture.constRect.w = explosionRadius;
      }
      explosion->data.texture.constRect.x =
          currX - explosion->data.texture.constRect.w / 2;
      explosion->data.texture.constRect.y =
          currY - explosion->data.texture.constRect.h / 2;
      explosion->data.texture.currFrame = 0;
      explosion->disableRendering = SDL_FALSE;

      break;
    }

    // ground impact check
    if ((currY + projectile->data.texture.constRect.h) * app->scalingFactorY >=
        currMapHeight) {
      //     printf("GROUND HIT!\n");
      //

      if (isHittableNearby) {
        // hit near to enemy
        if (currX >= enemyPlayer->tankObj->data.texture.constRect.x -
                         explosionRadius &&
            currX <= enemyPlayer->tankObj->data.texture.constRect.x +
                         enemyPlayer->tankObj->data.texture.constRect.w *
                             cos(DEGTORAD(enemyPlayer->tankAngle)) +
                         explosionRadius) {
          int center = enemyPlayer->tankObj->data.texture.constRect.x +
                       enemyPlayer->tankObj->data.texture.constRect.w / 2;
          app->currPlayer->score += abs(center - currX) * damageMultiplier;
        }

        int explosionCenter = currX * app->scalingFactorX;

        for (int i = 1; i <= explosionRadius; ++i) {
          int delta = sqrt(explosionRadius * explosionRadius - i * i);
          if (explosionCenter - i >= 0) {
            heightMap[explosionCenter - i] -= delta;
          }
          if (explosionCenter + i < app->screenWidth) {
            heightMap[explosionCenter + i] -= delta;
          }
        }

        heightMap[explosionCenter] -= explosionRadius;

        *regenMap = SDL_TRUE;

        recalcPlayerPos(app, firstPlayer, heightMap, 0, 5);
        recalcPlayerPos(app, secondPlayer, heightMap, 0, 8);
      }
      explosion->data.texture.constRect.h = explosionRadius;
      explosion->data.texture.constRect.w = explosionRadius;
      explosion->data.texture.constRect.x =
          currX - explosion->data.texture.constRect.w / 2;
      explosion->data.texture.constRect.y =
          currY - explosion->data.texture.constRect.h / 2;
      explosion->data.texture.currFrame = 0;
      explosion->disableRendering = SDL_FALSE;
      break;
    }

    SDL_Delay(16);
  }

  // hiding it again
  projectile->disableRendering = SDL_TRUE;
}

void recalcPlayerPos(App* app, Player* player, int* heightMap, int dx,
                     int xOffset) {
  RenderObject* tankObj = player->tankObj;
  RenderObject* gunObj = player->tankGunObj;

  double newAngle =
      360 - getAngle((tankObj->data.texture.constRect.x + xOffset + dx) *
                         app->scalingFactorX,
                     heightMap, 20 * app->scalingFactorX);

  tankObj->data.texture.constRect.x += dx;
  tankObj->data.texture.constRect.y =
      -27 + app->screenHeight / app->scalingFactorY -
      heightMap[(int)((tankObj->data.texture.constRect.x + 5) *
                      app->scalingFactorX)] /
          app->scalingFactorY;
  tankObj->data.texture.angle = newAngle;

  gunObj->data.texture.constRect.x += dx;
  gunObj->data.texture.constRect.y =
      -27 + app->screenHeight / app->scalingFactorY -
      heightMap[(int)((gunObj->data.texture.constRect.x + 5) *
                      app->scalingFactorX)] /
          app->scalingFactorY;
  gunObj->data.texture.angle = newAngle;

  player->tankAngle = newAngle;
}

void recalcPlayerGunAngle(Player* player, int dy) {
  player->gunAngle += dy;
  player->tankGunObj->data.texture.angleAlt = -player->gunAngle;
}

int playerMove(void* data) {
  log_info("started player move thread! wazup o//");

  struct paramsStruct {
    App* app;
    Player* firstPlayer;
    Player* secondPlayer;
    int* heightMap;
    RenderObject* projectile;
    RenderObject* explosion;
    SDL_bool* regenMap;
    unsigned mapSeed;
  };
  struct paramsStruct* params = (struct paramsStruct*)data;

  App* app = params->app;
  Player* firstPlayer = params->firstPlayer;
  Player* secondPlayer = params->secondPlayer;
  int* heightMap = params->heightMap;
  RenderObject* projectile = params->projectile;
  RenderObject* explosion = params->explosion;
  SDL_bool* regenMap = params->regenMap;
  unsigned mapSeed = params->mapSeed;

  while (app->currState == PLAY) {
    SDL_Delay(16);
    // skipping if player is in animation
    if (app->currPlayer->inAnimation) continue;
    // each player shot 12 times => game is over rn
    if (app->timesPlayed >= 24) {
      SDL_Delay(1500);
      if (firstPlayer->score > secondPlayer->score) {
        if (firstPlayer->type != MONKE) {
          app->winner = 3;
        } else {
          app->winner = 1;
          app->winnerScore = firstPlayer->score;
        }
      } else if (firstPlayer->score < secondPlayer->score) {
        if (secondPlayer->type != MONKE) {
          app->winner = 3;
        } else {
          app->winner = 2;
          app->winnerScore = secondPlayer->score;
        }
      } else {
        app->winner = 0;
      }
      app->currState = LEADERBOARD_ADD;
      continue;
    }
    // if currPlayer played by a user
    if (app->currPlayer->type == MONKE) {
      // if weapon wasn't chosen
      while (app->currWeapon == -1) {
        SDL_Delay(16);
      }
      // moving right
      if (app->keyStateArr[SDL_SCANCODE_RIGHT] && app->currPlayer->movesLeft) {
        // now in animation
        app->currPlayer->inAnimation = SDL_TRUE;
        smoothMove(app, app->currPlayer == firstPlayer, SDL_TRUE, heightMap);
        saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                         app->currPlayer == firstPlayer, mapSeed);
        // leaving animation
        app->currPlayer->inAnimation = SDL_FALSE;
      }

      // moving left
      else if (app->keyStateArr[SDL_SCANCODE_LEFT] &&
               app->currPlayer->movesLeft) {
        // now in animation
        app->currPlayer->inAnimation = SDL_TRUE;
        smoothMove(app, app->currPlayer == firstPlayer, SDL_FALSE, heightMap);
        saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                         app->currPlayer == firstPlayer, mapSeed);
        // leaving animation
        app->currPlayer->inAnimation = SDL_FALSE;
      }

      // gun moving up
      else if (app->keyStateArr[SDL_SCANCODE_UP]) {
        while (app->keyStateArr[SDL_SCANCODE_UP]) {
          SDL_Delay(30);
          // if angle is already maxed
          if (fabs((app->currPlayer->gunAngle - 180.0)) < 1e-9) {
            break;
          }
          recalcPlayerGunAngle(app->currPlayer, 1);
        }
        saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                         app->currPlayer == firstPlayer, mapSeed);
      }

      // gun moving down
      else if (app->keyStateArr[SDL_SCANCODE_DOWN]) {
        while (app->keyStateArr[SDL_SCANCODE_DOWN]) {
          SDL_Delay(30);
          // if angle is already maxed
          if (fabs(app->currPlayer->gunAngle) < 1e-9) {
            app->currPlayer->gunAngle = 0.0;
            break;
          }
          recalcPlayerGunAngle(app->currPlayer, -1);
        }
        saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                         app->currPlayer == firstPlayer, mapSeed);
      }

      // decreasing power
      else if (app->keyStateArr[SDL_SCANCODE_S]) {
        while (app->keyStateArr[SDL_SCANCODE_S]) {
          SDL_Delay(30);
          // if angle is already maxed
          if (app->currPlayer->firingPower <= 1) {
            app->currPlayer->firingPower = 1;
            break;
          }
          app->currPlayer->firingPower--;
        }
        saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                         app->currPlayer == firstPlayer, mapSeed);
      }

      // increasing power
      else if (app->keyStateArr[SDL_SCANCODE_W]) {
        while (app->keyStateArr[SDL_SCANCODE_W]) {
          SDL_Delay(30);
          // if angle is already maxed
          if (app->currPlayer->firingPower >= 100) {
            app->currPlayer->firingPower = 100;
            break;
          }
          app->currPlayer->firingPower++;
        }
        saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                         app->currPlayer == firstPlayer, mapSeed);
      }

      // should be firing when pressing space
      else if (app->keyStateArr[SDL_SCANCODE_SPACE]) {
        // starting animation state
        app->currPlayer->inAnimation = SDL_TRUE;

        shoot(app, firstPlayer, secondPlayer, projectile, explosion, heightMap,
              regenMap);

        // stopping animation state
        app->currPlayer->inAnimation = SDL_FALSE;

        if (app->currPlayer == firstPlayer) {
          app->currPlayer = secondPlayer;
        } else {
          app->currPlayer = firstPlayer;
        }

        app->timesPlayed++;
        app->currWeapon = -1;
        saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                         app->currPlayer == firstPlayer, mapSeed);

        log_info("players swapped");
        SDL_Delay(200);
      }
    }
    // if played by bot
    else {
      app->currPlayer->inAnimation = SDL_TRUE;
      // if weapon wasn't chosen
      while (app->currWeapon == -1) {
        SDL_Delay(16);
      }
      log_info("currweapon - %d", app->currWeapon);

      botMain(app, firstPlayer, secondPlayer, heightMap, projectile, explosion,
              regenMap, app->currPlayer->type);

      app->currPlayer->inAnimation = SDL_FALSE;

      // switching players
      if (app->currPlayer == firstPlayer) {
        app->currPlayer = secondPlayer;
      } else {
        app->currPlayer = firstPlayer;
      }

      recalcPlayerPos(app, firstPlayer, heightMap, 0, 5);
      recalcPlayerPos(app, secondPlayer, heightMap, 0, 8);

      app->timesPlayed++;
      app->currWeapon = -1;
      saveCurrentState(app, firstPlayer, secondPlayer, heightMap,
                       app->currPlayer == firstPlayer, mapSeed);
    }
  }

  log_info("[bot] killed player move thread! byee o//");

  return 0;
}
