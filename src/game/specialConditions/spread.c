#include "spread.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../../math/math.h"
#include "../../math/rand.h"

#include "log/log.h"

double getMaxSpreadByWeapon(int weapon) {
  if (weapon == 0) return 5;
  if (weapon == 1) return 10;
  if (weapon == 2) return 15;
  if (weapon == 3) return 25;
  return 10;
}

static int getRandomSpread() {
  return getRandomValue(0, 100);
}

double addSpreadToCurrAngle(int weapon, double currAngle) {
  double maxSpread = getMaxSpreadByWeapon(weapon);
  int offset = getRandomSpread();

  double spread = ((double)(offset % 100) / 100.0f) * 2 * maxSpread - maxSpread;

  return currAngle + spread;
}

static void drawSpreadLines(SDL_Renderer* renderer, int x0, int y0, int x1,
                            int y1, int thickness) {
  for (int i = -1; i < thickness; i++)
    SDL_RenderDrawLine(renderer, x0, y0 + i, x1, y1 + i);
}

void renderSpreadArea(App* app, RenderObject* spreadArea) {
  SDL_SetRenderTarget(app->renderer, spreadArea->data.texture.texture);
  SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 0);
  SDL_RenderClear(app->renderer);

  SDL_Point gunEdgeCoord;

  if (app->currPlayer->tankGunObj->data.texture.flipFlag ==
      SDL_FLIP_HORIZONTAL) {
    gunEdgeCoord = getPixelScreenPosition(
        (SDL_Point){
            app->currPlayer->tankObj->data.texture.constRect.x *
                app->scalingFactorX,
            app->currPlayer->tankObj->data.texture.constRect.y *
                app->scalingFactorY,
        },
        (SDL_Point){8 * app->scalingFactorX, 27 * app->scalingFactorY},
        app->currPlayer->tankObj->data.texture.angle,
        (SDL_Point){24 * app->scalingFactorX, 8 * app->scalingFactorY});

    spreadArea->data.texture.constRect.x =
        gunEdgeCoord.x / app->scalingFactorX -
        spreadArea->data.texture.constRect.w;
    spreadArea->data.texture.constRect.y =
        gunEdgeCoord.y / app->scalingFactorY -
        spreadArea->data.texture.constRect.h / 1.5f;
  } else {
    gunEdgeCoord = getPixelScreenPosition(
        (SDL_Point){
            app->currPlayer->tankObj->data.texture.constRect.x *
                app->scalingFactorX,
            app->currPlayer->tankObj->data.texture.constRect.y *
                app->scalingFactorY,
        },
        (SDL_Point){5 * app->scalingFactorX, 27 * app->scalingFactorY},
        app->currPlayer->tankObj->data.texture.angle,
        (SDL_Point){24 * app->scalingFactorX, 8 * app->scalingFactorY});

    spreadArea->data.texture.constRect.x = gunEdgeCoord.x / app->scalingFactorX;
    spreadArea->data.texture.constRect.y =
        gunEdgeCoord.y / app->scalingFactorY -
        spreadArea->data.texture.constRect.h / 1.5f;
  }

  double currGunAngle = app->currPlayer->tankGunObj->data.texture.angle;

  if (app->currPlayer->tankGunObj->data.texture.flipFlag ==
      SDL_FLIP_HORIZONTAL) {
    currGunAngle += 180 - app->currPlayer->tankGunObj->data.texture.angleAlt;
  } else {
    currGunAngle += app->currPlayer->tankGunObj->data.texture.angleAlt;
  }

  currGunAngle = 360 - normalizeAngle(currGunAngle);

  SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, 255);

  double offset = getMaxSpreadByWeapon(app->currWeapon);

  double currGunCos = cos(DEGTORAD(currGunAngle));
  double currGunSin = sin(DEGTORAD(currGunAngle));

  double upLineCos = cos(DEGTORAD((currGunAngle + offset)));
  double upLineSin = sin(DEGTORAD((currGunAngle + offset)));

  double downLineCos = cos(DEGTORAD((currGunAngle - offset)));
  double downLineSin = sin(DEGTORAD((currGunAngle - offset)));

  if (app->currPlayer->tankGunObj->data.texture.flipFlag ==
      SDL_FLIP_HORIZONTAL) {
    drawSpreadLines(
      app->renderer,
      15 * currGunCos + spreadArea->data.texture.constRect.w,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * currGunSin,
      15 * upLineCos * 5 + spreadArea->data.texture.constRect.w,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * upLineSin * 5,
      1
    );
    drawSpreadLines(
      app->renderer,
      15 * currGunCos + spreadArea->data.texture.constRect.w,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * currGunSin,
      15 * downLineCos * 5 + spreadArea->data.texture.constRect.w,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * downLineSin * 5,
      1
    );
  } else {
    drawSpreadLines(
      app->renderer, 
      20 * currGunCos,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * currGunSin,
      20 * upLineCos * 5,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * upLineSin * 5,
      1
    );
    drawSpreadLines(
      app->renderer, 
      20 * currGunCos,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * currGunSin,
      20 * downLineCos * 5,
      spreadArea->data.texture.constRect.h / 1.5 - 18 * downLineSin * 5,
      1
    );
  }

  SDL_SetRenderTarget(app->renderer, NULL);
}