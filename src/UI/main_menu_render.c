#include "main_menu_render.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <log/log.h>
#include <stdio.h>

#include "../SDL/SDL_render.h"
#include "../game/animations.h"
#include "../math/physics.h"

void mainMenu(App* app) {
  RenderObject* logoObject = createRenderObject(
      app->renderer, TEXTURE, 2, b_NONE, "media/imgs/logo.png", NULL);
  logoObject->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX -
       logoObject->data.texture.constRect.w) /
      2;

  // inversed height
  double height = app->screenHeight / 2.;
  double velocity = 1000;

  RenderObject* objectsArr[] = {logoObject};

  // output 'dropping' logo
  while (simulateFall_1Step(&height, &velocity) != -1 &&
         app->currState != EXIT) {
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    objectsArr[0]->data.texture.constRect.y = 33 - (int)height;

    renderTextures(app, objectsArr, sizeof(objectsArr) / sizeof(*objectsArr),
                   SDL_TRUE);

    SDL_RenderPresent(app->renderer);
    SDL_Delay(16);
  }

  if (app->currState == EXIT) {
    freeRenderObject(logoObject);
    SDL_Cleanup(app);
    exit(0);
  }

  // setting up current state to MENU
  app->currState = MENU;

  log_info("success completed state: LOGO");

  log_info("current state: MENU");

  // func will wait untill user do some interactions
  menuRenderLoop(app, logoObject);

  log_info("success completed state: MENU");

  // going back to mainLoop
}

void menuRenderLoop(App* app, RenderObject* logo) {
  // loading main font
  char temp[256];
  sprintf(temp, "%smedia/fonts/PixeloidSans-Bold.ttf", app->basePath);
  TTF_Font* menuButtonsFont = loadFont(temp, 80);

  RenderObject* infoButtonObj = createRenderObject(
      app->renderer, TEXTURE | CAN_BE_TRIGGERED, 1, b_HELP,
      "media/imgs/helpIco.png",
      &(SDL_Point){app->screenWidth / app->scalingFactorX - 55,
                   6 + ((!app->settings.isFullscreen) ? 24 : 0)});

  RenderObject* leaderboardObj = createRenderObject(
      app->renderer, TEXTURE | CAN_BE_TRIGGERED, 1, b_LEADERBOARDS,
      "media/imgs/trophy.png",
      &(SDL_Point){app->screenWidth / app->scalingFactorX - 55,
                   6 + ((!app->settings.isFullscreen) ? 24 : 0) + 60});
  // play button
  RenderObject* playTextObj = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 1, b_PLAY, "PLAY",
      menuButtonsFont, &(SDL_Point){500, 404}, &(SDL_Color){0, 255, 159, 200},
      &(SDL_Color){230, 25, 25, 255});
  playTextObj->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX -
       playTextObj->data.texture.constRect.w) /
      2;

  // settings button
  RenderObject* settingsTextObj = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 1, b_SETTINGS, "SETTINGS",
      menuButtonsFont,
      &(SDL_Point){500, 404 + playTextObj->data.texture.constRect.h},
      &(SDL_Color){128, 128, 128, 255}, &(SDL_Color){230, 25, 25, 255});
  settingsTextObj->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX -
       settingsTextObj->data.texture.constRect.w) /
      2;

  // quit button
  RenderObject* quitTextObj = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 1, b_QUIT, "QUIT",
      menuButtonsFont,
      &(SDL_Point){500, 404 + playTextObj->data.texture.constRect.h +
                            settingsTextObj->data.texture.constRect.h},
      &(SDL_Color){128, 128, 128, 255}, &(SDL_Color){230, 25, 25, 255});
  quitTextObj->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX -
       quitTextObj->data.texture.constRect.w) /
      2;
  // creting objects arr
  RenderObject* objectsArr[] = {leaderboardObj,  infoButtonObj, playTextObj,
                                settingsTextObj, quitTextObj,   logo};

  unsigned rgbOffset = 0;
  while (app->currState == MENU) {
    // filling up the background with black color and clearing render
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    renderTextures(app, objectsArr, sizeof(objectsArr) / sizeof(*objectsArr),
                   SDL_TRUE);

    // output rgb lines (1 reversed)
    drawRGBLine(app, 390 * app->scalingFactorY, rgbOffset, SDL_FALSE);
    drawRGBLine(app, 700 * app->scalingFactorY, rgbOffset, SDL_TRUE);

    // rendering
    SDL_RenderPresent(app->renderer);
    rgbOffset += 4;

    SDL_Delay(16);
  }

  // closing font
  TTF_CloseFont(menuButtonsFont);
  //

  // destroying textures
  freeRenderObject(playTextObj);
  freeRenderObject(settingsTextObj);
  freeRenderObject(quitTextObj);
  freeRenderObject(logo);
  freeRenderObject(infoButtonObj);
  freeRenderObject(leaderboardObj);
  //
}