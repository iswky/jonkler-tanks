#include "main_menu_render.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <log/log.h>

#include "../SDL/SDL_main.h"
#include "../SDL/SDL_render.h"
#include "../SDL/event_handlers.h"
#include "../SDL/ui_helpers.h"
#include "../game/animations.h"
#include "../math/physics.h"
#include "log/log.h"

inline static void mainMenuInit(App* app, mainMenuObjects* objs) {
  objs->logoObject = createRenderObject(app->renderer, TEXTURE, 2, b_NONE,
                                        "media/imgs/logo.png", NULL);
  objs->logoObject->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX -
       objs->logoObject->data.texture.constRect.w) /
      2;
}

void mainMenu(App* app) {
  mainMenuObjects objs = {0};

  mainMenuInit(app, &objs);

  // inversed height
  double height = app->screenHeight / 2.;
  double velocity = 1000;

  RenderObject* objectsArr[] = {objs.logoObject};

  // output 'dropping' logo
  while (simulateFall_1Step(&height, &velocity) != -1 &&
         app->currState != EXIT) {
    pollAllEvents(app);
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    objectsArr[0]->data.texture.constRect.y = 33 - (int32_t)height;

    renderTextures(app, objectsArr, sizeof(objectsArr) / sizeof(*objectsArr),
                   SDL_TRUE);

    SDL_RenderPresent(app->renderer);
    SDL_Delay(16);
  }

  if (app->currState == EXIT) {
    freeRenderObject(objs.logoObject);
    SDL_Cleanup(app);
    exit(0);
  }

  // setting up current state to MENU
  app->currState = MENU;

  log_info("success completed state: LOGO");

  log_info("current state: MENU");

  // func will wait untill user do some interactions
  menuRenderLoop(app, objs.logoObject);

  log_info("success completed state: MENU");

  // going back to mainLoop
}

inline static void menuRenderLoopInit(App* app, menuRenderLoopObjects* objs) {
  TTF_Font* menuButtonsFont = loadMainFont(app, 80);

  // create icon buttons
  int32_t iconY = 6 + ((!app->settings.isFullscreen) ? 24 : 0);
  objs->infoButtonObj = createRenderObject(
      app->renderer, TEXTURE | CAN_BE_TRIGGERED, 1, b_HELP,
      "media/imgs/helpIco.png",
      &(SDL_Point){app->screenWidth / app->scalingFactorX - 55, iconY});

  objs->leaderboardObj = createRenderObject(
      app->renderer, TEXTURE | CAN_BE_TRIGGERED, 1, b_LEADERBOARDS,
      "media/imgs/trophy.png",
      &(SDL_Point){app->screenWidth / app->scalingFactorX - 55, iconY + 60});

  // create menu buttons
  objs->playTextObj = createCenteredButton(app, "PLAY", menuButtonsFont, 404,
                                           COLOR_GRAY, COLOR_RED, b_PLAY);

  // calculate position for settings button
  int32_t settingsY = 404 + objs->playTextObj->data.texture.constRect.h;
  objs->settingsTextObj =
      createCenteredButton(app, "SETTINGS", menuButtonsFont, settingsY,
                           COLOR_GRAY, COLOR_RED, b_SETTINGS);

  // calculate position for quit button
  int32_t quitY = settingsY + objs->settingsTextObj->data.texture.constRect.h;
  objs->quitTextObj = createCenteredButton(app, "QUIT", menuButtonsFont, quitY,
                                           COLOR_GRAY, COLOR_RED, b_QUIT);

  TTF_CloseFont(menuButtonsFont);
}

inline static void menuRenderLoopClear(menuRenderLoopObjects* objs,
                                       RenderObject* logo) {
  freeRenderObject(objs->playTextObj);
  freeRenderObject(objs->settingsTextObj);
  freeRenderObject(objs->quitTextObj);
  freeRenderObject(objs->infoButtonObj);
  freeRenderObject(objs->leaderboardObj);
  freeRenderObject(logo);
}

void menuRenderLoop(App* app, RenderObject* logo) {
  menuRenderLoopObjects objs = {0};
  menuRenderLoopInit(app, &objs);

  // creting objects arr
  RenderObject* objectsArr[] = {objs.leaderboardObj, objs.infoButtonObj,
                                objs.playTextObj,    objs.settingsTextObj,
                                objs.quitTextObj,    logo};

  uint32_t rgbOffset = 0;
  while (app->currState == MENU) {
    pollAllEvents(app);
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

  menuRenderLoopClear(&objs, logo);
}