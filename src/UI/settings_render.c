#include "settings_render.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "../SDL/SDL_render.h"
#include "../SDL/event_handlers.h"
#include "../SDL/ui_helpers.h"
#include "../game/settings.h"

static void settingsMainInit(App* app, settingsMainObjects* objs) {
  // load fonts
  objs->mainFont = loadMainFont(app, 40);
  objs->titleFont = loadMainFont(app, 50);
  // create title
  objs->settingsTextObj =
      createCenteredText(app, "SETTINGS", objs->titleFont, 20, COLOR_WHITE);
  // create section labels
  int32_t leftSectionX =
      getCenteredX(app, app->screenWidth / app->scalingFactorX / 2);
  int32_t rightSectionX =
      getCenteredX(app, app->screenWidth / app->scalingFactorX / 2) +
      app->screenWidth / app->scalingFactorX / 2;
  objs->videoTextObj = createLeftAlignedText(app, "VIDEO", objs->mainFont,
                                             leftSectionX, 100, COLOR_WHITE);
  objs->soundTextObj = createLeftAlignedText(
      app, "SOUND", objs->mainFont, leftSectionX,
      app->screenHeight / app->scalingFactorY / 3 + 10, COLOR_WHITE);
  objs->weaponsTextObj = createLeftAlignedText(app, "WEAPONS", objs->mainFont,
                                               rightSectionX, 100, COLOR_WHITE);
  // Center headers within their halves
  int32_t wTmp, hTmp;
  int32_t leftCenterX = app->screenWidth / app->scalingFactorX / 4;
  int32_t rightCenterX = app->screenWidth / app->scalingFactorX * 3 / 4;
  SDL_QueryTexture(objs->videoTextObj->data.texture.texture, NULL, NULL, &wTmp,
                   &hTmp);
  objs->videoTextObj->data.texture.constRect.x = leftCenterX - wTmp / 2;
  SDL_QueryTexture(objs->soundTextObj->data.texture.texture, NULL, NULL, &wTmp,
                   &hTmp);
  objs->soundTextObj->data.texture.constRect.x = leftCenterX - wTmp / 2;
  SDL_QueryTexture(objs->weaponsTextObj->data.texture.texture, NULL, NULL,
                   &wTmp, &hTmp);
  objs->weaponsTextObj->data.texture.constRect.x = rightCenterX - wTmp / 2;
  // volume label
  int32_t volumeY = objs->soundTextObj->data.texture.constRect.y +
                    objs->soundTextObj->data.texture.constRect.h + 10;
  objs->volumeTextObj = createLeftAlignedText(
      app, "Volume:", objs->mainFont, leftSectionX, volumeY, COLOR_GRAY);
  SDL_QueryTexture(objs->volumeTextObj->data.texture.texture, NULL, NULL, &wTmp,
                   &hTmp);
  objs->volumeTextObj->data.texture.constRect.x = leftCenterX - wTmp / 2;
  // back button
  objs->returnArrowObj = createBackButton(
      app, objs->titleFont, objs->settingsTextObj->data.texture.constRect.y);
  // volume slider
  objs->volumeSliderObj = createRenderObject(
      app->renderer, SLIDER, 1, s_VOLUME,
      &(SDL_Rect){.x = 30,
                  .y = objs->volumeTextObj->data.texture.constRect.y +
                       objs->volumeTextObj->data.texture.constRect.h + 20,
                  .w = 300,
                  .h = 50});
  // Center slider within left half
  objs->volumeSliderObj->data.texture.constRect.x =
      leftCenterX - objs->volumeSliderObj->data.texture.constRect.w / 2;
  // volume increment/decrement triangles
  objs->volumeDecrTriangleObj = createRenderObject(
      app->renderer, TRIANGLE | CAN_BE_TRIGGERED, 1, bT_DECREMENT_VOLUME,
      &(SDL_Point){objs->volumeSliderObj->data.texture.constRect.x - 30,
                   objs->volumeSliderObj->data.texture.constRect.y +
                       objs->volumeSliderObj->data.texture.constRect.h / 2},
      &(SDL_Point){objs->volumeSliderObj->data.texture.constRect.x - 5,
                   objs->volumeSliderObj->data.texture.constRect.y + 10},
      &(SDL_Point){objs->volumeSliderObj->data.texture.constRect.x - 5,
                   objs->volumeSliderObj->data.texture.constRect.y +
                       objs->volumeSliderObj->data.texture.constRect.h - 10},
      &(SDL_Color){128, 128, 128, 255}, &(SDL_Color){230, 25, 25, 255});
  objs->volumeIncrTriangleObj = createRenderObject(
      app->renderer, TRIANGLE | CAN_BE_TRIGGERED, 1, bT_INCREMENT_VOLUME,
      &(SDL_Point){objs->volumeSliderObj->data.texture.constRect.x +
                       objs->volumeSliderObj->data.texture.constRect.w + 30,
                   objs->volumeSliderObj->data.texture.constRect.y +
                       objs->volumeSliderObj->data.texture.constRect.h / 2},
      &(SDL_Point){objs->volumeSliderObj->data.texture.constRect.x +
                       objs->volumeSliderObj->data.texture.constRect.w + 5,
                   objs->volumeSliderObj->data.texture.constRect.y + 10},
      &(SDL_Point){objs->volumeSliderObj->data.texture.constRect.x +
                       objs->volumeSliderObj->data.texture.constRect.w + 5,
                   objs->volumeSliderObj->data.texture.constRect.y +
                       objs->volumeSliderObj->data.texture.constRect.h - 10},
      &(SDL_Color){128, 128, 128, 255}, &(SDL_Color){230, 25, 25, 255});
  //
  objs->smallBulletLabel = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 1, b_W1, "small bullet",
      objs->mainFont, &(SDL_Point){0, 0}, &(SDL_Color){128, 128, 128, 255},
      &(SDL_Color){0, 216, 255, 255});
  objs->smallBulletLabel->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX * 3 / 2 -
       objs->smallBulletLabel->data.texture.constRect.w) /
      2;
  objs->smallBulletLabel->data.texture.constRect.y = 175;
  objs->bigBulletLabel = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 1, b_W2, "BIG BULLET",
      objs->mainFont, &(SDL_Point){0, 0}, &(SDL_Color){128, 128, 128, 255},
      &(SDL_Color){0, 100, 255, 255});
  objs->bigBulletLabel->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX * 3 / 2 -
       objs->bigBulletLabel->data.texture.constRect.w) /
      2;
  objs->bigBulletLabel->data.texture.constRect.y = 250;
  objs->smallBoomLabel = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 1, b_W3, "small boom",
      objs->mainFont, &(SDL_Point){0, 0}, &(SDL_Color){128, 128, 128, 255},
      &(SDL_Color){230, 25, 25, 255});
  objs->smallBoomLabel->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX * 3 / 2 -
       objs->smallBoomLabel->data.texture.constRect.w) /
      2;
  objs->smallBoomLabel->data.texture.constRect.y = 325;
  objs->bigBoomLabel = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 1, b_W4, "BIG BOOM",
      objs->mainFont, &(SDL_Point){0, 0}, &(SDL_Color){128, 128, 128, 255},
      &(SDL_Color){248, 0, 0, 255});
  objs->bigBoomLabel->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX * 3 / 2 -
       objs->bigBoomLabel->data.texture.constRect.w) /
      2;
  objs->bigBoomLabel->data.texture.constRect.y = 400;
  // checkbox textures
  objs->checkboxText =
      createTextTexture(app->renderer, objs->mainFont, "[ ]", 128, 128, 128,
                        255);  // checkbox (unchecked)
  objs->checkboxCheckedText =
      createTextTexture(app->renderer, objs->mainFont, "[X]", 128, 128, 128,
                        255);  // checkbox (checked)
  // surfaces for bttns
  SDL_Rect checkboxRect;
  //
  SDL_QueryTexture(objs->checkboxText, NULL, NULL, &checkboxRect.w,
                   &checkboxRect.h);
  // Fullscreen label
  objs->fullscreenTextObj = createRenderObject(
      app->renderer, TEXT, 1, b_NONE, "Fullscreen", objs->mainFont,
      &(SDL_Point){30, 20}, &(SDL_Color){128, 128, 128, 255});
  SDL_QueryTexture(objs->fullscreenTextObj->data.texture.texture, NULL, NULL,
                   &wTmp, &hTmp);
  objs->fullscreenTextObj->data.texture.constRect.x =
      leftCenterX - wTmp / 2 - checkboxRect.w / 2;
  objs->fullscreenTextObj->data.texture.constRect.y =
      objs->videoTextObj->data.texture.constRect.y +
      objs->videoTextObj->data.texture.constRect.h + 10;
  //
  checkboxRect.x = objs->fullscreenTextObj->data.texture.constRect.x +
                   objs->fullscreenTextObj->data.texture.constRect.w + 10;
  checkboxRect.y = objs->fullscreenTextObj->data.texture.constRect.y;
  objs->controlsText = createRenderObject(
      app->renderer, TEXT, 1, b_NONE, "CONTROLS", objs->mainFont,
      &(SDL_Point){0, app->screenHeight / app->scalingFactorY / 3 * 2 + 5},
      &(SDL_Color){255, 255, 255, 255});
  objs->controlsText->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX -
       objs->controlsText->data.texture.constRect.w) /
      2;
  objs->controls1Text = createRenderObject(
      app->renderer, TEXT, 1, b_NONE, "W/S: change power ↑/↓: change angle",
      objs->mainFont,
      &(SDL_Point){10, objs->controlsText->data.texture.constRect.y +
                           objs->controlsText->data.texture.constRect.h + 5},
      &(SDL_Color){128, 128, 128, 255});
  objs->controls2Text = createRenderObject(
      app->renderer, TEXT, 1, b_NONE, "←/→: move tank space: shoot",
      objs->mainFont,
      &(SDL_Point){10, objs->controls1Text->data.texture.constRect.y +
                           objs->controls1Text->data.texture.constRect.h + 5},
      &(SDL_Color){128, 128, 128, 255});
  objs->checkboxFullscreen = (RenderObject){
      .data.texture.texture = app->settings.isFullscreen
                                  ? objs->checkboxCheckedText
                                  : objs->checkboxText,
      .data.texture.triggeredTexture = app->settings.isFullscreen
                                           ? objs->checkboxCheckedText
                                           : objs->checkboxText,
      .data.texture.constRect = checkboxRect,
      .data.texture.scaleRect = checkboxRect,
      .canBeTriggered = SDL_TRUE,
      .Zpos = 1,
      .buttonName = b_CHECKBOX_FULLSCREEN,
  };
  objs->prevVolume = app->settings.currentVolume;

  TTF_CloseFont(objs->mainFont);
  TTF_CloseFont(objs->titleFont);
}

inline static void settingsMainClear(settingsMainObjects* objs) {
  freeRenderObject(objs->soundTextObj);
  freeRenderObject(objs->settingsTextObj);
  freeRenderObject(objs->returnArrowObj);
  freeRenderObject(objs->weaponsTextObj);
  freeRenderObject(objs->fullscreenTextObj);
  freeRenderObject(objs->videoTextObj);
  freeRenderObject(objs->volumeSliderObj);
  freeRenderObject(objs->volumeTextObj);
  freeRenderObject(objs->smallBulletLabel);
  freeRenderObject(objs->bigBulletLabel);
  freeRenderObject(objs->smallBoomLabel);
  freeRenderObject(objs->bigBoomLabel);
  freeRenderObject(objs->controls1Text);
  freeRenderObject(objs->controls2Text);

  SDL_DestroyTexture(objs->checkboxCheckedText);
  SDL_DestroyTexture(objs->checkboxText);
}

void settingsMain(App* app) {
  Mix_VolumeMusic(app->settings.currentVolume);  // set volume

  settingsMainObjects objs = {0};

  settingsMainInit(app, &objs);
  // render arr
  RenderObject* objectsArr[] = {
      &objs.checkboxFullscreen,   objs.soundTextObj,
      objs.settingsTextObj,       objs.returnArrowObj,
      objs.smallBulletLabel,      objs.bigBulletLabel,
      objs.smallBoomLabel,        objs.bigBoomLabel,
      objs.weaponsTextObj,        objs.fullscreenTextObj,
      objs.videoTextObj,          objs.volumeSliderObj,
      objs.volumeIncrTriangleObj, objs.volumeDecrTriangleObj,
      objs.volumeTextObj,         objs.controlsText,
      objs.controls1Text,         objs.controls2Text};

  int32_t prevVolume = app->settings.currentVolume;

  while (app->currState == SETTINGS) {
    pollAllEvents(app);
    // new texture if volume changed
    if (app->settings.currentVolume != prevVolume) {
      Mix_Volume(-1, app->settings.currentVolume);
      Mix_VolumeMusic(app->settings.currentVolume);
      prevVolume = app->settings.currentVolume;
    }

    // clear screen
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    // draw dividing lines
    SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
    // vertical line in the middle
    SDL_RenderDrawLine(app->renderer, app->screenWidth / 2,
                       (objs.settingsTextObj->data.texture.constRect.y +
                        objs.settingsTextObj->data.texture.constRect.h + 10) *
                           app->scalingFactorY,
                       app->screenWidth / 2, app->screenHeight * 2. / 3);
    // horizontal line under the title
    SDL_RenderDrawLine(app->renderer, 0,
                       (objs.settingsTextObj->data.texture.constRect.y +
                        objs.settingsTextObj->data.texture.constRect.h + 10) *
                           app->scalingFactorY,
                       app->screenWidth,
                       (objs.settingsTextObj->data.texture.constRect.y +
                        objs.settingsTextObj->data.texture.constRect.h + 10) *
                           app->scalingFactorY);
    // horizontal line in the left part n1
    SDL_RenderDrawLine(app->renderer, 0, app->screenHeight / 3,
                       app->screenWidth / 2, app->screenHeight / 3);
    // horizontal line in the left part n2
    SDL_RenderDrawLine(app->renderer, 0, app->screenHeight / 3 * 2,
                       app->screenWidth, app->screenHeight / 3 * 2);

    if (!app->settings.isFullscreen) {
      objectsArr[0]->data.texture.texture =
          objectsArr[0]->data.texture.triggeredTexture = objs.checkboxText;
    } else {
      objectsArr[0]->data.texture.texture =
          objectsArr[0]->data.texture.triggeredTexture =
              objs.checkboxCheckedText;
    }

    renderTextures(app, objectsArr, sizeof(objectsArr) / sizeof(*objectsArr),
                   SDL_TRUE);

    SDL_RenderPresent(app->renderer);
    SDL_Delay(16);
  }

  settingsMainClear(&objs);

  // saving settings
  saveSettings(app);
}