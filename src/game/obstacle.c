#include "obstacle.h"

#include "log/log.h"
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

//generate count trees, coordination_x and create render object for trees
void createTrees(App* app, RenderObject* objectsArr[], int32_t* count_tree,
                 int32_t* x, int32_t* heightmap) {
  *count_tree = getRandomValue(0, 6);
  log_debug("count trees = %d", *count_tree);
  for (int i = 0; i < *count_tree; i++) {
    x[i] = getRandomValue(100, app->screenWidth - 100);
  }

  if (*count_tree == 0) return;
  for (int i = 0; i < *count_tree; i++) {
    objectsArr[i] = createRenderObject(
        app->renderer, TEXTURE, 1, b_NONE, "media/imgs/tree1.png",
        &(SDL_Point){
            x[i], -120 + app->screenHeight / app->scalingFactorY -
                      heightmap[(int32_t)((x[i] + 54) * app->scalingFactorX)] /
                          app->scalingFactorY});
  }
}

//generate count clouds, coordination_x and create render object for clouds
void createClouds(App* app, RenderObject* objectsArr[],
                  SDL_bool* flag_regencloud, int32_t* count_cloud, int32_t* x,
                  int32_t* heightmap) {
  // *count_cloud = getRandomValue(0, 6);
  // log_debug("count cloud = %d", *count_cloud);
  // for (int i = 0; i < *count_cloud; i++) {
  //   x[i] = getRandomValue(100, app->screenWidth - 100);
  // }

  // if (*count_cloud == 0) {
  //   *flag_regencloud = true;
  //   return;
  // }
  // for (int i = 0; i < *count_cloud; i++) {
  //   objectsArr[i] = createRenderObject(
  //       app->renderer, TEXTURE, 1, b_NONE, "media/imgs/cloud.png",
  //       &(SDL_Point){x[i],
  //                    -250 + app->screenHeight / app->scalingFactorY -
  //                        heightmap[(int32_t)(x[i] * app->scalingFactorX)] /
  //                            app->scalingFactorY});
  //   x[i] = x[i] + 1;
  //   if (x[i] > app->screenWidth) {
  //     *flag_regencloud = true;
  //   }
  // }
}

RenderObject* createStone(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos) {
  static uint32_t currStonesCnt = 0;
  int x = getRandomValue(startPos, endPos);
  obstacleRock[currStonesCnt++] = x;
  return fall(app, heightmap, x);
}
