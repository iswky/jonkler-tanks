#ifndef PLAY_RENDER_H
#define PLAY_RENDER_H

#include "../App.h"

void playMain(App* app, unsigned SEED);
void preGameMain(App* app);
void renderMap(SDL_Renderer* renderer, int* heightmap, int* basedMap, int width,
               int height);
SDL_Texture* saveRenderMapToTexture(SDL_Renderer* renderer, int width,
                                    int height, int* heightMap, int* basedMap);

#endif