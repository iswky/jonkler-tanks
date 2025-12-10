#include "bot1.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#include <log/log.h>
#include <time.h>

#include "../../math/math.h"
#include "../../math/rand.h"
#include "../obstacle.h"
#include "../player_movement.h"
#include "../specialConditions/wind.h"

// returning X coordinate of the nearest stone
// or -1 if stone wasnt found
static SDL_Point findNearestStone(SDL_bool commingFromLeft) {
  SDL_Point res = {-1, -1};
  if (!commingFromLeft) {
    for (int32_t i = MAXSTONES - 1; i >= 0; --i) {
      // skipping non existing objects or alredy destroyed objects
      if (obstacles[i].obstacleObject == NULL || obstacles[i].health == 0) {
        continue;
      }

      int obstacleX = obstacles[i].obstacleObject->data.texture.constRect.x;
      int obstacleW = obstacles[i].obstacleObject->data.texture.constRect.w;

      res.x = obstacleX;
      res.x += obstacleW;

      int obstacleY = obstacles[i].obstacleObject->data.texture.constRect.y;

      res.y = obstacleY;

      break;
    }
  } else {
    for (int32_t i = 0; i < MAXSTONES; ++i) {
      // skipping non existing objects or alredy destroyed objects
      if (obstacles[i].obstacleObject == NULL || obstacles[i].health == 0) {
        continue;
      }

      int obstacleX = obstacles[i].obstacleObject->data.texture.constRect.x;
      res.x = obstacleX;

      int obstacleY = obstacles[i].obstacleObject->data.texture.constRect.y;

      res.y = obstacleY;
      break;
    }
  }
  return res;
}

// returning X coordinate of the nearest cloud
// or -1 if no cloud was found
static SDL_Point findNearestCloud(SDL_bool isFirstPlayer) {
  SDL_Point res = {-1, -1};
  if (!isFirstPlayer) {
    for (int32_t i = MAXCLOUDS + MAXSTONES - 1; i >= MAXSTONES; --i) {
      // skipping non existing objects or alredy destroyed objects
      if (obstacles[i].obstacleObject == NULL || obstacles[i].health == 0) {
        continue;
      }

      int obstacleX = obstacles[i].obstacleObject->data.texture.constRect.x;
      int obstacleW = obstacles[i].obstacleObject->data.texture.constRect.w;

      res.x = obstacleX;
      res.x += obstacleW;

      int obstacleY = obstacles[i].obstacleObject->data.texture.constRect.y;

      res.y = obstacleY;
      break;
    }
  } else {
    for (int32_t i = MAXSTONES; i < MAXSTONES + MAXCLOUDS; ++i) {
      // skipping non existing objects or alredy destroyed objects
      if (obstacles[i].obstacleObject == NULL || obstacles[i].health == 0) {
        continue;
      }

      int obstacleX = obstacles[i].obstacleObject->data.texture.constRect.x;
      res.x = obstacleX;

      int obstacleY = obstacles[i].obstacleObject->data.texture.constRect.y;

      res.y = obstacleY;
      break;
    }
  }
  return res;
}

// func will find shelter (either under a cloud or behind a rock)
static void findShelter(App* app, int32_t* heightMap, Player* currPlayer,
                        SDL_bool isFirstPlayer) {
  enum shelterType shelter;

  SDL_Point shelterPos = findNearestStone(isFirstPlayer);
  log_info("stone pos: %d (isFirst: %d)", shelterPos, isFirstPlayer);

  // if stone was not found
  if (shelterPos.x == -1) {
    shelterPos = findNearestCloud(isFirstPlayer);
    log_info("cloud pos: %d (isFirst: %d)", shelterPos, isFirstPlayer);

    if (shelterPos.x == -1) {
      return;
    } else {
      shelter = CLOUD;
    }
  } else {
    shelter = STONE;
  }

  if (shelter == STONE) {
    //  if curr shelter is STONE we re just simply moving towards it
    while (
        smoothMove(app, isFirstPlayer, !isFirstPlayer, heightMap, obstacles)) {
      ;
    }
  } else if (shelter == CLOUD) {
    // If its a cloud, we try to hide beneath it a lil lefter(left player)
    // or righter(right player)
    // according to my super math calculations it will be about 83 px
    if (isFirstPlayer) {
    }
  }
}

void bot1Main(App* app, Player* firstPlayer, Player* secondPlayer,
              int32_t* heightMap, RenderObject* projectile,
              RenderObject* explosion, SDL_bool* regenMap,
              SDL_bool* recalcBulletPath, double initGunAngle) {
  Player* enemy;
  if (app->currPlayer == firstPlayer) {
    enemy = secondPlayer;
  } else {
    enemy = firstPlayer;
  }

  // getting collison for an enemy player
  SDL_Point collisionP1, collisionP2, collisionP3;
  int32_t collisionP1R, collisionP2R, collisionP3R;
  // getting player collisions centers and radiuses
  // player1 collision
  if (enemy == firstPlayer) {
    collisionP1 = getPixelScreenPosition(
        (SDL_Point){enemy->tankObj->data.texture.scaleRect.x,
                    enemy->tankObj->data.texture.scaleRect.y},
        (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemy->tankObj->data.texture.angle,
        (SDL_Point){35 * app->scalingFactorX, 19 * app->scalingFactorY});
    collisionP2 = getPixelScreenPosition(
        (SDL_Point){enemy->tankObj->data.texture.scaleRect.x,
                    enemy->tankObj->data.texture.scaleRect.y},
        (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemy->tankObj->data.texture.angle,
        (SDL_Point){20 * app->scalingFactorX, 12 * app->scalingFactorY});
    collisionP3 = getPixelScreenPosition(
        (SDL_Point){enemy->tankObj->data.texture.scaleRect.x,
                    enemy->tankObj->data.texture.scaleRect.y},
        (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemy->tankObj->data.texture.angle,
        (SDL_Point){7 * app->scalingFactorX, 19 * app->scalingFactorY});
    // radiuses
    collisionP1R = 9 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP2R = 13 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP3R = 10 * MAX(app->scalingFactorX, app->scalingFactorY);
  }
  // player2 collison
  else {
    collisionP1 = getPixelScreenPosition(
        (SDL_Point){enemy->tankObj->data.texture.scaleRect.x,
                    enemy->tankObj->data.texture.scaleRect.y},
        (SDL_Point){8 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemy->tankObj->data.texture.angle,
        (SDL_Point){11 * app->scalingFactorX, 19 * app->scalingFactorY});
    collisionP2 = getPixelScreenPosition(
        (SDL_Point){enemy->tankObj->data.texture.scaleRect.x,
                    enemy->tankObj->data.texture.scaleRect.y},
        (SDL_Point){8 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemy->tankObj->data.texture.angle,
        (SDL_Point){23 * app->scalingFactorX, 13 * app->scalingFactorY});
    collisionP3 = getPixelScreenPosition(
        (SDL_Point){enemy->tankObj->data.texture.scaleRect.x,
                    enemy->tankObj->data.texture.scaleRect.y},
        (SDL_Point){8 * app->scalingFactorX, 27 * app->scalingFactorY},
        enemy->tankObj->data.texture.angle,
        (SDL_Point){36 * app->scalingFactorX, 18 * app->scalingFactorY});
    // radiuses
    collisionP1R = 11 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP2R = 11 * MAX(app->scalingFactorX, app->scalingFactorY);
    collisionP3R = 9 * MAX(app->scalingFactorX, app->scalingFactorY);
  }

  // bullet initial position
  SDL_Point initPos = getPixelScreenPosition(
      (SDL_Point){app->currPlayer->tankObj->data.texture.scaleRect.x,
                  app->currPlayer->tankObj->data.texture.scaleRect.y},
      (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
      app->currPlayer->tankObj->data.texture.angle,
      (SDL_Point){24 * app->scalingFactorX, 7 * app->scalingFactorY});

  initPos.x /= app->scalingFactorX;
  initPos.y /= app->scalingFactorY;

  initPos.x += 25 * cos(DEGTORAD(initGunAngle));
  initPos.y -= 25 * sin(DEGTORAD(initGunAngle));

  SDL_FPoint currPos = {
      .x = (float)initPos.x,
      .y = (float)initPos.y,
  };

  double velMultiplicator;
  int32_t explosionRadius;
  SDL_bool isHittableNearby;
  switch (app->currWeapon) {
    // small bullet
    case 0:
      velMultiplicator = 2;
      explosionRadius = projectile->data.texture.constRect.w;
      isHittableNearby = SDL_FALSE;
      break;
    // BIG BULLET
    case 1:
      velMultiplicator = 1.75;
      explosionRadius = projectile->data.texture.constRect.w;
      isHittableNearby = SDL_FALSE;
      break;
    // small boom
    case 2:
      velMultiplicator = 1.25;
      explosionRadius = projectile->data.texture.constRect.w * 2;
      isHittableNearby = SDL_TRUE;
      break;
    // BIG BOOM
    case 3:
      velMultiplicator = 1.0;
      explosionRadius = projectile->data.texture.constRect.w * 4;
      isHittableNearby = SDL_TRUE;
      break;
    default:
      velMultiplicator = 1.0;
      explosionRadius = projectile->data.texture.constRect.w;
      isHittableNearby = SDL_FALSE;
      break;
  }

  int32_t initVel = app->currPlayer->firingPower * velMultiplicator;

  // getting current weapon state
  SDL_bool isWeaponBroken = app->currPlayer->buffs.weaponIsBroken;

  // getting current wind speed
  int32_t windStrengthMin, windStrengthMax;
  getWindRange(app, &windStrengthMin, &windStrengthMax);
  double windStrength = AVG(windStrengthMin, windStrengthMax);

  // 1. (!) firstly we should find shelter
  findShelter(app, heightMap, app->currPlayer, app->currPlayer == firstPlayer);

  // calculating initHitPosition
  int32_t hitPos =
      calcHitPosition(&currPos, initVel, initGunAngle, heightMap, app,
                      &collisionP1, &collisionP2, &collisionP3, collisionP1R,
                      collisionP2R, collisionP3R, projectile, windStrength);

  log_info("[bot1] curr hit pos is at %d", hitPos);
}