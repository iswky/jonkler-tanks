#define _USE_MATH_DEFINES

#include "math.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#include <log/log.h>

#include "../game/obstacle.h"
#include "../game/obstacle_struct.h"
#include "../game/player_movement.h"
#include "rand.h"

// check if (x, y) in a triangle with vertexes p1, p2, p3
SDL_bool isInTriangle(const int32_t x, const int32_t y, SDL_Point p1,
                      SDL_Point p2, SDL_Point p3) {
  /*
   *
   * calculating (x2 - x1) * (y - y1) - (y2 - y1) * (x0 - x1)
   * for each pairs of a points and (x, y)
   *
   *       p2        p2
   *      / |        | \
   *     /  |        |  \
   *    p1  |   or   |  p1
   *     \  |        |  /
   *      \ |        | /
   *       p3        p3
   */
  int32_t D1, D2, D3;

  D1 = (p2.x - p1.x) * (y - p1.y) - (p2.y - p1.y) * (x - p1.x);
  D2 = (p3.x - p2.x) * (y - p2.y) - (p3.y - p2.y) * (x - p2.x);
  D3 = (p1.x - p3.x) * (y - p3.y) - (p1.y - p3.y) * (x - p3.x);

  return (D1 >= 0 && D2 >= 0 && D3 >= 0) || (D1 <= 0 && D2 <= 0 && D3 <= 0);
}

SDL_bool isInCircle(const int32_t x, const int32_t y, const SDL_Point* center,
                    const int32_t radius) {
  return ((center->x - x) * (center->x - x) +
          (center->y - y) * (center->y - y)) <= radius * radius;
}

double getAngle(int32_t x, int32_t* heightMap, int32_t dx) {
  int32_t dy = heightMap[x + dx] - heightMap[x];
  return atan2(dy, dx) * 180.0 / M_PI;
}

double normalizeAngle(double angleDeg) {
  while (angleDeg >= 360.0) {
    angleDeg -= 360.0;
  }
  while (angleDeg < 0.0) {
    angleDeg += 360.0;
  }
  return angleDeg;
}

SDL_Point getPixelScreenPosition(SDL_Point drawPos, SDL_Point center,
                                 double angleDeg, SDL_Point target) {
  angleDeg = normalizeAngle(angleDeg);
  double angleRad = DEGTORAD(angleDeg);

  int32_t dx = target.x - center.x;
  int32_t dy = target.y - center.y;

  double rotatedX = dx * cos(angleRad) - dy * sin(angleRad);
  double rotatedY = dx * sin(angleRad) + dy * cos(angleRad);

  SDL_Point result;
  result.x = (int32_t)(drawPos.x + center.x + rotatedX + 0.5);
  result.y = (int32_t)(drawPos.y + center.y + rotatedY + 0.5);

  return result;
}

// function return x coord of a possible hit
// if (x < 0 && x != -1) than hit was in enemy collison
int32_t calcHitPosition(SDL_FPoint* initPos, double initVel, double angle,
                        int32_t* heightMap, const App* app,
                        const SDL_Point* collision1,
                        const SDL_Point* collision2,
                        const SDL_Point* collision3, const int32_t collision1R,
                        const int32_t collision2R, const int32_t collision3R,
                        RenderObject* projectile, double windStrength) {
  double windAngleRad = DEGTORAD(normalizeAngle(
      360 - app->globalConditions.wind.directionIcon->data.texture.angle));

  double windStrengthX = windStrength * cos(windAngleRad);
  double windStrengthY = windStrength * sin(windAngleRad);

  double angleRad = DEGTORAD(angle);
  double vx = initVel * cos(angleRad);
  double vy = initVel * sin(angleRad);

  const double G = 9.81;

  double dt = 1. / 60;
  double currTime = 0.0;

  int32_t currX = (int32_t)initPos->x;
  int32_t currY = (int32_t)initPos->y;

  // while we dont hit the ground
  while (app->currState == PLAY) {
    currTime += dt;
    double dy =
        currY - initPos->y - (vy * currTime - 0.5 * G * currTime * currTime);
    double dx = currX - initPos->x + vx * currTime;

    projectile->data.texture.angle = 360 - atan2(dy, dx) * 180.0 / M_PI;

    currX = initPos->x + vx * currTime + windStrengthX * currTime;
    currY = initPos->y - (vy * currTime - 0.5 * G * currTime * currTime +
                          windStrengthY * currTime);

    int32_t currXScaled = currX * app->scalingFactorX;
    int32_t currYScaled = currY * app->scalingFactorY;

    // if out of bounds
    if (currX < 0 || currXScaled > app->screenWidth) {
      return -1;
    }

    // hit in enemy collision
    if (isInCircle(currXScaled, currYScaled, collision1, collision1R) ||
        isInCircle(currXScaled, currYScaled, collision2, collision2R) ||
        isInCircle(currXScaled, currYScaled, collision3, collision3R)) {
      return -currXScaled;
    }

    // hit at obstacles
    // res will be currXscaled_currYScaled
    if (checkObstacleCollisions(currX, currY)) {
      return currXScaled * 10000 + currYScaled;
    }

    // successful hit
    if ((currY + projectile->data.texture.constRect.h) * app->scalingFactorY >=
        app->screenHeight - heightMap[currXScaled]) {
      return currXScaled;
    }
  }

  return -1;
}

// func that generates number for a weapon
int32_t getAllowedNumber(App* app) {
  int32_t value = 0;
  do {
    value = getRandomValue(0, 3);
  } while (app->settings.weaponsAllowed[value] == SDL_FALSE);
  return value;
}

SDL_bool PointInRotatedRect(const SDL_Rect* rect, const SDL_Point* point,
                            float degrees) {
  if (!rect || !point) return SDL_FALSE;
  float rad = degrees * M_PI / 180.0f;
  float cos_a = cosf(rad);
  float sin_a = sinf(rad);
  float cx = rect->x + rect->w * 0.5f;
  float cy = rect->y + rect->h * 0.5f;
  float dx = point->x - cx;
  float dy = point->y - cy;
  float rot_x = dx * cos_a - dy * sin_a;
  float rot_y = dx * sin_a + dy * cos_a;
  float hw = rect->w * 0.5f;
  float hh = rect->h * 0.5f;
  return (rot_x >= -hw && rot_x <= hw && rot_y >= -hh && rot_y <= hh);
}

void smoothChangeAngle(Player* player, int32_t endAngle, enum State* currState,
                       SDL_bool* recalcBulletPath) {
  if ((int32_t)player->gunAngle > endAngle) {
    for (int32_t i = player->gunAngle; i > endAngle && *currState == PLAY;
         --i) {
      recalcPlayerGunAngle(player, -1);
      *recalcBulletPath = SDL_TRUE;
      SDL_Delay(16);
    }
  } else {
    for (int32_t i = player->gunAngle; i < endAngle && *currState == PLAY;
         ++i) {
      recalcPlayerGunAngle(player, 1);
      *recalcBulletPath = SDL_TRUE;
      SDL_Delay(16);
    }
  }
}

void smoothChangePower(Player* player, int32_t endPower, enum State* currState,
                       SDL_bool* recalcBulletPath) {
  if ((int32_t)player->firingPower > endPower) {
    for (int32_t i = player->firingPower; i > endPower && *currState == PLAY;
         --i) {
      player->firingPower--;
      SDL_Delay(16);
      *recalcBulletPath = SDL_TRUE;
    }
  } else {
    for (int32_t i = player->firingPower; i < endPower && *currState == PLAY;
         ++i) {
      player->firingPower++;
      SDL_Delay(16);
      *recalcBulletPath = SDL_TRUE;
    }
  }
}

int32_t smoothMove(App* app, SDL_bool isFirstPlayer, SDL_bool isRight,
                   int32_t* heightMap, obstacleStruct* obstacle) {
  if (isRight) {
    int32_t i = 0;
    for (; i != 45; ++i) {
      // trying to move forward
      for (int j = 0; j < MAXSTONES; j++) {
        if (obstacle[j].obstacleObject == NULL || obstacle[j].health == 0) {
          continue;
        }
        if ((app->currPlayer->tankObj->data.texture.constRect.x +
             app->currPlayer->tankObj->data.texture.constRect.w - 5) >=
                obstacle[j].obstacleObject->data.texture.constRect.x &&
            isFirstPlayer) {
          if (i) {
            app->currPlayer->movesLeft--;
          }
          return 2;
        }
      }

      if ((app->currPlayer->tankObj->data.texture.constRect.x +
           app->currPlayer->tankObj->data.texture.constRect.w - 2) *
              app->scalingFactorX >=
          app->screenWidth) {
        if (i) {
          app->currPlayer->movesLeft--;
        }
        return 2;
      }

      recalcPlayerPos(app, app->currPlayer, heightMap, 1,
                      (isFirstPlayer == SDL_TRUE) ? 5 : 8);

      SDL_Delay(16);
    }

    // if we moved at least a 1 px
    if (i) {
      app->currPlayer->movesLeft--;
    }
  } else {
    int32_t i = 0;
    for (; i != 45; ++i) {
      // trying to move forward
      for (int j = 0; j < MAXSTONES; j++) {
        if (obstacle[j].obstacleObject == NULL || obstacle[j].health == 0) {
          continue;
        }
        if (app->currPlayer->tankObj->data.texture.constRect.x <=
                obstacle[j].obstacleObject->data.texture.constRect.x + 120 &&
            !isFirstPlayer) {
          if (i) {
            app->currPlayer->movesLeft--;
          }
          return 2;
        }
      }
      if (app->currPlayer->tankObj->data.texture.constRect.x <= 2) {
        if (i) {
          app->currPlayer->movesLeft--;
        }
        return 2;
      }

      recalcPlayerPos(app, app->currPlayer, heightMap, -1,
                      (isFirstPlayer == SDL_TRUE) ? 5 : 8);
      SDL_Delay(16);
    }
    // if we moved at least a 1 px
    if (i) {
      app->currPlayer->movesLeft--;
    }
  }
  return 0;
}

void getPositionAtSpecTime(SDL_FPoint* pos, double vx, double vy, double windVx,
                           double windVy, double currTime) {
  const double G = 9.81;

  pos->x = vx * currTime + windVx * currTime;
  pos->y = vy * currTime - 0.5 * G * currTime * currTime + windVy * currTime;

  // printf("currTime: %lf, vy:%lf\n", currTime, vy - G * currTime);
}

int32_t findLineHeightIntersections(SDL_Point p1, SDL_Point p2,
                                    int32_t* heightMap, int32_t width) {
  int32_t x1 = p1.x, y1 = p1.y;
  int32_t x2 = p2.x, y2 = p2.y;

  // make sure x1 < x2
  if (x1 > x2) {
    int temp;
    temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
  }

  int32_t dx = x2 - x1;
  int32_t dy = y2 - y1;

  int32_t prevDiff = 0;
  int32_t prevSet = 0;

  for (int32_t x = x1; x <= x2 && x < width; x++) {
    int32_t yLine = y1 + (int64_t)(dy) * (x - x1) / dx;
    int32_t diff = yLine - heightMap[x];

    // exact intersection
    if (diff == 0) {
      log_warn("EXACT: %d", x);
      return x;
    }

    // sign change => crossing between x-1 and x
    if (prevSet && ((diff > 0 && prevDiff < 0) || (diff < 0 && prevDiff > 0))) {
      int32_t x0 = x - 1;
      int32_t yLine0 = y1 + (int64_t)dy * (x0 - x1) / dx;

      int32_t h0 = heightMap[x0];

      int32_t approxX;
      if (yLine == yLine0) {
        approxX = x;
      } else {
        approxX = x0 + (h0 - yLine0) * (int64_t)(x - x0) / (yLine - yLine0);
      }

      log_warn("approx: %d", approxX);
      return approxX;
    }

    prevDiff = diff;
    prevSet = 1;
  }
  return -1;
}