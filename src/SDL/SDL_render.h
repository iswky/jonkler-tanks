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

  CAN_BE_TRIGGERED = 1 << 6,
  EXTENDED = 1 << 7,
  DOUBLE_EXTENDED = 1 << 8,
};

/*  This function creates renderObject with different internals and accepts different amount of args
 *  1) if flags & TEXT it will accept args in this order:
 *      a. const char* textToInsert, TTF_Font* font, SDL_Point* position, SDL_Color* normalColor
 *      b. if flags & CAN_BE_TRIGGERED it will accept one more argument SDL_Color* triggeredColor
 *  2) if flags & TRIANGLE it will accept args in this order:
 *      a. SDL_Point* v1, SDL_Point* v2, SDL_Point* v3, SDL_Color* normalColor --> v(?) stands for vertex coords
 *      b. if flags & CAN_BE_TRIGGERED it will accept one more argument SDL_Color* triggeredColor
 *  3) if flags & TEXTURE it will accept args in this order:
 *      a. const char* path, SDL_Point* position
 *      b. if flags & EXTENDED_RENDER it will accept 5 more argument double angle, double angleAlt, SDL_RendererFlip flip, 
 SDL_Point* centerRot, SDL_Point* centerRot_Alt
 *  4) if flags & SLIDER it will accept args in this order:
 *      a. SDL_Rect* rect
 *  5) if flags & TEXT_INPUT it will accept args in this order:
 *      a. SDL_Rect* rect, int maxInputChars, int charTypes, TTF_Font* font
 */
RenderObject* createRenderObject(SDL_Renderer* render,
                                 enum RenderObjectMode flags, int zPos,
                                 enum Button buttonName, ...);

TTF_Font* loadFont(const char* str, int ptSize);
SDL_Texture* createImgTexture(SDL_Renderer* renderer, const char* str);
SDL_Texture* createTextTexture(SDL_Renderer* renderer, TTF_Font* font,
                               const char* text, unsigned char r,
                               unsigned char g, unsigned char b,
                               unsigned char a);

void scaleObjects(App* app, RenderObject* objectsArr[], int objectArrSize);
void scaleRectRatio(SDL_Rect* rect, float ratio);

SDL_bool isInTriangle(const int x, const int y, SDL_Point p1, SDL_Point p2,
                      SDL_Point p3);

void renderTextures(App* app, RenderObject* objectsArr[], int objectsArrSize,
                    int isScaling);
void drawFilledTriangle(SDL_Renderer* renderer, const SDL_Point* p1,
                        const SDL_Point* p2, const SDL_Point* p3,
                        SDL_Color color);

void freeRenderObject(RenderObject* object);
void freeTexturesArr(SDL_Texture** arr, int size);

#endif