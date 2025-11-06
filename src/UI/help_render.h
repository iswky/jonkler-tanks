#ifndef ABOUT_RENDER
#define ABOUT_RENDER

#include "../App.h"

typedef struct helpMainObjects {
  SDL_Texture* textTexture;
  RenderObject* returnArrowObj;
} helpMainObjects;

void helpMain(App* app);

#endif