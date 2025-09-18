#include "animations.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void drawRGBLine(App* app, int y, unsigned offset, int isBackward) {
  if (!isBackward) {
    for (int x = 0; x < app->screenWidth; x += app->scalingFactorX) {
      // ??
      int r = (sin((x + offset) * 0.02) + 1) * 127;
      int g = (sin((x + offset) * 0.02 + 2) + 1) * 127;
      int b = (sin((x + offset) * 0.02 + 4) + 1) * 127;

      SDL_SetRenderDrawColor(app->renderer, r, g, b, 255);
      SDL_RenderDrawLine(app->renderer, x, y, x, y + 10);
    }
  } else {
    for (int x = 0; x < app->screenWidth; x += app->scalingFactorX) {
      // ??
      int r = (sin((app->screenWidth - x + offset) * 0.02) + 1) * 127;
      int g = (sin((app->screenWidth - x + offset) * 0.02 + 2) + 1) * 127;
      int b = (sin((app->screenWidth - x + offset) * 0.02 + 4) + 1) * 127;

      SDL_SetRenderDrawColor(app->renderer, r, g, b, 255);
      SDL_RenderDrawLine(app->renderer, x, y, x, y + 10);
    }
  }
}