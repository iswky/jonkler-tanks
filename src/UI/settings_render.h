#ifndef SETTINGS_RENDER_H
#define SETTINGS_RENDER_H

#include "../App.h"

typedef struct settingsMainObjects {
  TTF_Font* mainFont;
  TTF_Font* titleFont;
  RenderObject* settingsTextObj;
  RenderObject* videoTextObj;
  RenderObject* soundTextObj;
  RenderObject* weaponsTextObj;
  RenderObject* volumeTextObj;
  RenderObject* returnArrowObj;
  RenderObject* volumeSliderObj;
  RenderObject* volumeDecrTriangleObj;
  RenderObject* volumeIncrTriangleObj;
  RenderObject* smallBulletLabel;
  RenderObject* bigBulletLabel;
  RenderObject* smallBoomLabel;
  RenderObject* bigBoomLabel;
  RenderObject* fullscreenTextObj;
  RenderObject* controlsText;
  RenderObject* controls1Text;
  RenderObject* controls2Text;
  SDL_Texture* checkboxText;
  SDL_Texture* checkboxCheckedText;
  RenderObject checkboxFullscreen;
  int32_t prevVolume;
} settingsMainObjects;

void settingsMain(App* app);

#endif