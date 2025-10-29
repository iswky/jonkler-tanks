#include "obstacle.h"
#include <SDL2/SDL_stdinc.h> 
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "../SDL/SDL_render.h"
#include "../math/math.h"
#include "log/log.h"

obstacleRock[] = {0};

//Fallout
void renderShelter76(App* app, RenderObject* objectsArr[],
                     SDL_bool* flag_regenShelter, int32_t* count_shelter,
                     int32_t* x, int32_t* heightmap) {
  if (*flag_regenShelter) {
    while (*count_shelter <= 0) *count_shelter = rand() % 5;
    log_debug("count shelter = %d \n", *count_shelter);
    for (int i = 0; i < *count_shelter; i++) {
      x[i] = 100 + rand() % (app->screenWidth - 2 * 100);
     /* while (x[i] < 150 ||
             x[i] > app->screenWidth - 150) {
        x[i] = rand() % app->screenWidth;
      }*/
    }
    *flag_regenShelter = false;
  }
  obstacleRock[0] = 10000;
  obstacleRock[1] = 10000;
  obstacleRock[2] = 10000;
  obstacleRock[3] = 10000;
  int i = 0;
  for (; i < *count_shelter; i++) {
    objectsArr[i] = fall(app, NULL, heightmap, x[i]);
    obstacleRock[i] = objectsArr[i]->data.texture.constRect.x;
  }
}

RenderObject* fall(App* app, RenderObject* object, int32_t* heightmap,
                   int32_t x) {
  //int32_t max = 0;
  //int32_t dx = 101;
  //for (int32_t i = x; i < (x + 101); i++) {
  //  if (max < heightmap[(int32_t)(i * app->scalingFactorX)]) {
  //    max = heightmap[(int32_t)(i * app->scalingFactorX)];
  //    dx = i;
  //  }
  //}
  SDL_Point* pos = malloc(sizeof(SDL_Point));
  if (pos != NULL)
    *pos = (SDL_Point){x, -69 + app->screenHeight / app->scalingFactorY -
                              heightmap[(int32_t)(x * app->scalingFactorX)] /
                                  app->scalingFactorY};
  //*pos = (SDL_Point){200 * app->scalingFactorX, 200};
  double angle =
      getAngle(x * app->scalingFactorX, heightmap, 80 * app->scalingFactorX);
  object = createRenderObject(app->renderer, TEXTURE, 0, b_NONE,
                              "media/imgs/rock.png", pos);
  object->data.texture.angle = 360 - angle;
  //object->data.texture.angle = 0.0f;
  object->data.texture.angleAlt = 0.0f;
  object->data.texture.flipFlag = SDL_FLIP_NONE;
  SDL_Point* center = malloc(sizeof(SDL_Point));
  if (center != NULL) *center = (SDL_Point){0, 0};
  object->data.texture.centerRot = center;
  object->data.texture.centerRot_Alt = NULL;
  return object;
}

