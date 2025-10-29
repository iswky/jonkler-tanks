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
  if (weapon == 2) return 20;
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
                            int y1, int offset) {
  for (int i = 0; i <= 1; i++)
    SDL_RenderDrawLine(renderer, x0, y0 + i, x1 + offset, y1 + offset + i);
  for (int i = 0; i <= 1; i++)
    SDL_RenderDrawLine(renderer, x0, y0 + i, x1 - offset, y1 - offset + i);
}

void renderSpreadArea(App* app, RenderObject* spreadArea) {
  SDL_SetRenderTarget(app->renderer, spreadArea->data.texture.texture);
  SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 0);
  SDL_RenderClear(app->renderer);

  // calculating starting point for the path (aka gun basis)
  SDL_Point gunEdgeCoord;
  SDL_Color currColor = {0, 0, 0, 255};

  // for 2nd player
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
    // for 1st player
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

  // that angle is clockwise
  double currGunAngle = app->currPlayer->tankGunObj->data.texture.angle;

  // calculating angle specifically for a current player
  // if currPlayer is a second player
  if (app->currPlayer->tankGunObj->data.texture.flipFlag ==
      SDL_FLIP_HORIZONTAL) {
    currGunAngle += 180 - app->currPlayer->tankGunObj->data.texture.angleAlt;
  } else {
    //  if currPlayer is a first player
    currGunAngle += app->currPlayer->tankGunObj->data.texture.angleAlt;
  }

  // normalizing just to be sure its in [0; 2pi) and now its counterclockwise
  currGunAngle = 360 - normalizeAngle(currGunAngle);

  double currGunCos = cos(DEGTORAD(currGunAngle));
  double currGunSin = sin(DEGTORAD(currGunAngle));

  SDL_FPoint relativePos = {0.f, 0.f};

  SDL_SetRenderDrawColor(app->renderer, 255, 0, 0, 255);

  double vx = cos(DEGTORAD(currGunAngle));
  double vy = sin(DEGTORAD(currGunAngle));

  if (app->currPlayer->tankGunObj->data.texture.flipFlag ==
      SDL_FLIP_HORIZONTAL) {
    drawSpreadLines(
        app->renderer,
        relativePos.x + relativePos.x + 15 * currGunCos +
            spreadArea->data.texture.constRect.w,
        -relativePos.y + spreadArea->data.texture.constRect.h / 1.5 -
            18 * currGunSin,
        relativePos.x + relativePos.x + 15 * currGunCos * 5 +
            spreadArea->data.texture.constRect.w,
        -relativePos.y + spreadArea->data.texture.constRect.h / 1.5 -
            18 * currGunSin * 5,
        getMaxSpreadByWeapon(app->currWeapon));
  } else {
    drawSpreadLines(
        app->renderer, relativePos.x + 20 * currGunCos,
        -relativePos.y + spreadArea->data.texture.constRect.h / 1.5 -
            18 * currGunSin,
        relativePos.x + 20 * currGunCos * 5,
        -relativePos.y + spreadArea->data.texture.constRect.h / 1.5 -
            18 * currGunSin * 5,
        getMaxSpreadByWeapon(app->currWeapon));
  }
  // enable rendering back
  SDL_SetRenderTarget(app->renderer, NULL);
}