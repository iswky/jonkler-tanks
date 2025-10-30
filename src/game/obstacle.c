#include "obstacle.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <stdbool.h>

#include "../SDL/SDL_render.h"
#include "../math/math.h"
#include "../math/rand.h"

uint32_t obstacleRock[2];

// the freak is fall func name??
static RenderObject* fall(App* app, int32_t* heightmap, int32_t x) {
  SDL_Point pos = {x, -66 + app->screenHeight / app->scalingFactorY -
                          heightmap[(int32_t)(x * app->scalingFactorX)] /
                              app->scalingFactorY};
  double angle =
      getAngle(x * app->scalingFactorX, heightmap, 95 * app->scalingFactorX);

  RenderObject* object = createRenderObject(
      app->renderer, TEXTURE | EXTENDED, 0, b_NONE, "media/imgs/rock.png", &pos,
      360 - angle, SDL_FLIP_NONE, &(SDL_Point){0, 0});
  return object;
}

RenderObject* createCloud(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos, int32_t probability) {
  int imMrKrabsAndILikeMoney = getRandomValue(0, 100);
  RenderObject* res = NULL;

  if (imMrKrabsAndILikeMoney > probability) {
    int x = getRandomValue(startPos, endPos);

    res = createRenderObject(
        app->renderer, TEXTURE, 1, b_NONE, "media/imgs/cloud.png",
        &(SDL_Point){x,
                     -300 + app->screenHeight / app->scalingFactorY -
                         heightmap[(int32_t)((x + 54) * app->scalingFactorX)] /
                             app->scalingFactorY});
  }

  return res;
}

RenderObject* createStone(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos) {
  static uint32_t currStonesCnt = 0;
  int x = getRandomValue(startPos, endPos);
  obstacleRock[currStonesCnt++] = x;
  return fall(app, heightmap, x);
}

RenderObject* createTree(App* app, int32_t* heightmap, int32_t startPos,
                         int32_t endPos, int32_t probability) {
  int imMrKrabsAndILikeMoney = getRandomValue(0, 100);
  RenderObject* res = NULL;

  if (imMrKrabsAndILikeMoney > probability) {
    int x = getRandomValue(startPos, endPos);

    res = createRenderObject(
        app->renderer, TEXTURE, 1, b_NONE, "media/imgs/tree1.png",
        &(SDL_Point){x,
                     -120 + app->screenHeight / app->scalingFactorY -
                         heightmap[(int32_t)((x + 54) * app->scalingFactorX)] /
                             app->scalingFactorY});
  }

  return res;
}
