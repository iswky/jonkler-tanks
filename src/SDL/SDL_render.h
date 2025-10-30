#ifndef SDL_RENDER_H
#define SDL_RENDER_H

#include "../App.h"

enum RenderObjectMode {
  TEXT = 1 << 0,
  TRIANGLE = 1 << 1,
  TEXTURE = 1 << 2,
  SLIDER = 1 << 3,
  TEXT_INPUT = 1 << 4,
  GIF = 1 << 5,
  EMPTY = 1 << 6,

  CAN_BE_TRIGGERED = 1 << 10,
  EXTENDED = 1 << 11,
  DOUBLE_EXTENDED = 1 << 12,
};

RenderObject* createRenderObject(SDL_Renderer* render,
                                 enum RenderObjectMode flags, int32_t zPos,
                                 enum Button buttonName, ...);

TTF_Font* loadFont(const char* str, int32_t ptSize);
SDL_Texture* createImgTexture(SDL_Renderer* renderer, const char* str);
SDL_Texture* createTextTexture(SDL_Renderer* renderer, TTF_Font* font,
                               const char* text, uint8_t r, uint8_t g,
                               uint8_t b, uint8_t a);

void renderTextures(App* app, RenderObject* objectsArr[],
                    int32_t objectsArrSize, int32_t isScaling);

void renderBulletPath(App* app, RenderObject* bulletPath);

void freeRenderObject(RenderObject* object);
void freeTexturesArr(SDL_Texture** arr, int32_t size);

#endif