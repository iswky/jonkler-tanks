#include "leaderboard_render.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include "../SDL/SDL_render.h"
#include "../SDL/event_handlers.h"
#include "../SDL/ui_helpers.h"
#include "log/log.h"

static void renderPlayer(App* app, int32_t y, PlayerScore currPlayer,
                         const char* placeText, TTF_Font* font, uint8_t r,
                         uint8_t g, uint8_t b, uint8_t a) {
  char temp[10];
  int32_t w, h;
  int32_t x;

  SDL_Texture* playerPlace =
      createTextTexture(app->renderer, font, placeText, r, g, b, a);
  SDL_Texture* playerName =
      createTextTexture(app->renderer, font, currPlayer.name, r, g, b, a);
  snprintf(temp, sizeof(temp), "%d", currPlayer.score);
  SDL_Texture* playerScore =
      createTextTexture(app->renderer, font, temp, r, g, b, a);

  y = y * app->scalingFactorY;

  SDL_QueryTexture(playerPlace, NULL, NULL, &w, &h);
  x = (app->screenWidth / app->scalingFactorX / 2 - w) / 2 *
      app->scalingFactorX;
  w *= app->scalingFactorX;
  h *= app->scalingFactorY;
  SDL_RenderCopy(app->renderer, playerPlace, NULL, &(SDL_Rect){x, y, w, h});

  SDL_QueryTexture(playerName, NULL, NULL, &w, &h);
  x = (app->screenWidth / app->scalingFactorX - w) / 2 * app->scalingFactorX;
  w *= app->scalingFactorX;
  h *= app->scalingFactorY;
  SDL_RenderCopy(app->renderer, playerName, NULL, &(SDL_Rect){x, y, w, h});

  SDL_QueryTexture(playerScore, NULL, NULL, &w, &h);
  x = (app->screenWidth / app->scalingFactorX * 3 / 2 - w) / 2 *
      app->scalingFactorX;
  w *= app->scalingFactorX;
  h *= app->scalingFactorY;
  SDL_RenderCopy(app->renderer, playerScore, NULL, &(SDL_Rect){x, y, w, h});

  SDL_DestroyTexture(playerScore);
  SDL_DestroyTexture(playerPlace);
  SDL_DestroyTexture(playerName);
}

static void loadLeaderboardToArray(PlayerScore* leaderboard) {
  char temp[256];

  char* basePath = SDL_GetBasePath();

  snprintf(temp, sizeof(temp), "%sdata/leaderboard", basePath);
  free(basePath);

  FILE* file = fopen(temp, "r");
  if (file == NULL) {
    file = fopen(temp, "w");
    for (int32_t i = 0; i < 10; ++i) {
      fprintf(file, "0 EMPTY\n");
    }
    fclose(file);
    file = fopen(temp, "r");
  }

  for (int32_t i = 0; i < 10; ++i) {
    if (fscanf(file, "%d %15s", &leaderboard[i].score, leaderboard[i].name) <
        2) {
      break;
    }
  }

  fclose(file);
}

static int32_t findPlaceInLeaderboard(int32_t score) {
  char temp[256];

  char* basePath = SDL_GetBasePath();

  snprintf(temp, sizeof(temp), "%sdata/leaderboard", basePath);
  free(basePath);

  FILE* file = fopen(temp, "r");
  if (file == NULL) {
    file = fopen(temp, "w");
    for (int32_t i = 0; i < 10; ++i) {
      fprintf(file, "0 EMPTY\n");
    }
    fclose(file);
    file = fopen(temp, "r");
  }
  char name[16];
  int32_t currScore;

  for (int32_t i = 0; i < 10; ++i) {
    // broken file or smth like that
    if (fscanf(file, "%d %15s\n", &currScore, name) < 2) {
      return -1;
    }
    // if we ve found the place
    if (score > currScore) {
      fclose(file);
      return i + 1;
    }
  }
  fclose(file);
  return -1;
}

static void addToLeaderBoard(const char* name, int32_t score) {
  char temp[256];

  char* basePath = SDL_GetBasePath();

  snprintf(temp, sizeof(temp), "%sdata/leaderboard", basePath);
  free(basePath);

  FILE* file = fopen(temp, "r");

  if (file == NULL) {
    file = fopen(temp, "w");
    if (file == NULL) {
      log_error("error while reading leaderboard file!\n");
      return;
    }
    for (int32_t i = 0; i < 10; ++i) {
      fprintf(file, "0 EMPTY\n");
    }
    fclose(file);
    file = fopen(temp, "r");
  }

  PlayerScore leaderboard[10];
  memset(leaderboard, 0x00, sizeof(PlayerScore) * 10);

  for (int32_t i = 0; i < 10; ++i) {
    // reading an old leaderboard table
    if (fscanf(file, "%d %15s\n", &leaderboard[i].score, leaderboard[i].name) !=
        2) {
      log_error("error reading leaderboard table");

      return;
    }
  }

  fclose(file);

  int32_t newScorePos = findPlaceInLeaderboard(score) - 1;

  if (newScorePos != -1) {
    for (int32_t i = 9; i > newScorePos; i--) {
      leaderboard[i] = leaderboard[i - 1];
    }
  }

  leaderboard[newScorePos].score = score;
  strncpy(leaderboard[newScorePos].name, name, 16);

  file = fopen(temp, "w");
  if (file == NULL) {
    log_error("error while reading leaderboard file!");
    return;
  }

  for (int32_t i = 0; i < 10; ++i) {
    fprintf(file, "%d %s\n", leaderboard[i].score, leaderboard[i].name);
  }

  fclose(file);
}

///////////////////////////////////////////////////////////////////////////
static void leaderboardMainInit(App* app, const char* name,
                                leaderboardMainObjects* objs) {
  TTF_Font* smallFont = loadSmallFont(app, 30);
  TTF_Font* mainFont = loadMainFont(app, 60);

  if (*name != '\0') {
    addToLeaderBoard(name, app->winnerScore);
  }

  // creating labels
  objs->leaderboardLabel =
      createCenteredText(app, "LEADERBOARD", mainFont, 20, COLOR_WHITE);

  // creating column headers
  int32_t leftColX =
      getCenteredX(app, app->screenWidth / app->scalingFactorX / 2);
  int32_t centerColX =
      app->screenWidth / app->scalingFactorX / 2 - 20 * app->scalingFactorX;
  int32_t rightColX =
      getCenteredX(app, app->screenWidth / app->scalingFactorX / 2) +
      app->screenWidth / app->scalingFactorX / 2;

  objs->placeLabel = createLeftAlignedText(app, "PLACE", smallFont, leftColX,
                                           150, COLOR_WHITE);
  // Recalculate X to match row positioning using logical (unscaled) coords
  int32_t tmpW, tmpH;
  SDL_QueryTexture(objs->placeLabel->data.texture.texture, NULL, NULL, &tmpW,
                   &tmpH);
  objs->placeLabel->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX / 2 - tmpW) / 2;
  objs->placeLabel->data.texture.scaleRect.x =
      objs->placeLabel->data.texture.constRect.x;
  objs->nameLabel = createLeftAlignedText(app, "NAME", smallFont, centerColX,
                                          150, COLOR_WHITE);
  SDL_QueryTexture(objs->nameLabel->data.texture.texture, NULL, NULL, &tmpW,
                   &tmpH);
  objs->nameLabel->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX - tmpW) / 2;
  objs->nameLabel->data.texture.scaleRect.x =
      objs->nameLabel->data.texture.constRect.x;
  objs->scoreLabel = createLeftAlignedText(app, "SCORE", smallFont, rightColX,
                                           150, COLOR_WHITE);
  SDL_QueryTexture(objs->scoreLabel->data.texture.texture, NULL, NULL, &tmpW,
                   &tmpH);
  objs->scoreLabel->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX * 3 / 2 - tmpW) / 2;
  objs->scoreLabel->data.texture.scaleRect.x =
      objs->scoreLabel->data.texture.constRect.x;

  PlayerScore leaderboardArray[10] = {0};
  loadLeaderboardToArray(leaderboardArray);

  objs->leaderboard = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_TARGET,
                                        app->screenWidth, app->screenHeight);
  SDL_SetRenderTarget(app->renderer, objs->leaderboard);
  SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 0);
  SDL_RenderClear(app->renderer);

  renderPlayer(app, 200, leaderboardArray[0], "1st", smallFont, 179, 149, 0,
               255);
  renderPlayer(app, 250, leaderboardArray[1], "2nd", smallFont, 216, 216, 216,
               255);
  renderPlayer(app, 300, leaderboardArray[2], "3rd", smallFont, 130, 87, 44,
               255);
  renderPlayer(app, 350, leaderboardArray[3], "4th", smallFont, 128, 128, 128,
               255);
  renderPlayer(app, 400, leaderboardArray[4], "5th", smallFont, 128, 128, 128,
               255);
  renderPlayer(app, 450, leaderboardArray[5], "6th", smallFont, 128, 128, 128,
               255);
  renderPlayer(app, 500, leaderboardArray[6], "7th", smallFont, 128, 128, 128,
               255);
  renderPlayer(app, 550, leaderboardArray[7], "8th", smallFont, 128, 128, 128,
               255);
  renderPlayer(app, 600, leaderboardArray[8], "9th", smallFont, 128, 128, 128,
               255);
  renderPlayer(app, 650, leaderboardArray[9], "10th", smallFont, 128, 128, 128,
               255);

  SDL_SetRenderTarget(app->renderer, NULL);

  objs->returnArrowObj = createBackButton(
      app, mainFont, objs->leaderboardLabel->data.texture.constRect.y);

  TTF_CloseFont(smallFont);
  TTF_CloseFont(mainFont);
}

inline static void leaderboardMainLoop(App* app, leaderboardMainObjects* objs) {
  SDL_RenderCopy(app->renderer, objs->leaderboard, NULL, NULL);

  SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);
  // underscoring place label
  SDL_RenderDrawLine(
      app->renderer,
      objs->placeLabel->data.texture.scaleRect.x - 3 * app->scalingFactorX,
      objs->placeLabel->data.texture.scaleRect.y +
          objs->placeLabel->data.texture.scaleRect.h + 3 * app->scalingFactorY,
      objs->placeLabel->data.texture.scaleRect.x +
          objs->placeLabel->data.texture.scaleRect.w,
      objs->placeLabel->data.texture.scaleRect.y +
          objs->placeLabel->data.texture.scaleRect.h + 3 * app->scalingFactorY);
  // underscoring score label
  SDL_RenderDrawLine(
      app->renderer,
      objs->scoreLabel->data.texture.scaleRect.x - 3 * app->scalingFactorX,
      objs->scoreLabel->data.texture.scaleRect.y +
          objs->scoreLabel->data.texture.scaleRect.h + 3 * app->scalingFactorY,
      objs->scoreLabel->data.texture.scaleRect.x +
          objs->scoreLabel->data.texture.scaleRect.w,
      objs->scoreLabel->data.texture.scaleRect.y +
          objs->scoreLabel->data.texture.scaleRect.h + 3 * app->scalingFactorY);
  // underscoring name label
  SDL_RenderDrawLine(
      app->renderer,
      objs->nameLabel->data.texture.scaleRect.x - 3 * app->scalingFactorX,
      objs->nameLabel->data.texture.scaleRect.y +
          objs->nameLabel->data.texture.scaleRect.h + 3 * app->scalingFactorY,
      objs->nameLabel->data.texture.scaleRect.x +
          objs->nameLabel->data.texture.scaleRect.w,
      objs->nameLabel->data.texture.scaleRect.y +
          objs->nameLabel->data.texture.scaleRect.h + 3 * app->scalingFactorY);
}

inline static void leaderboardMainClear(leaderboardMainObjects* objs) {
  freeRenderObject(objs->leaderboardLabel);
  freeRenderObject(objs->placeLabel);
  freeRenderObject(objs->nameLabel);
  freeRenderObject(objs->scoreLabel);
  freeRenderObject(objs->returnArrowObj);
  SDL_DestroyTexture(objs->leaderboard);

  free(objs);
}

void leaderboardMain(App* app, const char* name) {
  leaderboardMainObjects objs = {0};
  leaderboardMainInit(app, name, &objs);

  RenderObject* objectsArr[] = {objs.returnArrowObj, objs.leaderboardLabel,
                                objs.placeLabel, objs.nameLabel,
                                objs.scoreLabel};

  while (app->currState == LEADERBOARD) {
    pollAllEvents(app);
    // filling up the background with black color and clearing render
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    renderTextures(app, objectsArr, sizeof(objectsArr) / sizeof(*objectsArr),
                   SDL_TRUE);

    leaderboardMainLoop(app, &objs);

    // rendering
    SDL_RenderPresent(app->renderer);

    SDL_Delay(16);
  }

  leaderboardMainClear(&objs);
}

//////////////////////////////////////////////////////////////////////////
static void leaderboardAddMainInit(App* app, leaderboardAddMainObjects* objs) {
  TTF_Font* smallFont = loadSmallFont(app, 30);
  TTF_Font* mainFont = loadMainFont(app, 60);

  char temp[256];
  if (app->winner == 1) {
    strcpy(temp, "BETMENT WON!!!");
    Mix_PlayChannel(-1, app->sounds[1], 0);
  } else if (app->winner == 2) {
    strcpy(temp, "JONKLER WON!!!");
    Mix_PlayChannel(-1, app->sounds[1], 0);
  } else if (app->winner == 3) {
    strcpy(temp, "ROBOCOP WON!!!");
  } else {
    strcpy(temp, "NO ONE WON!!!");
  }

  objs->wonLabel = createCenteredText(app, temp, mainFont, 60, COLOR_GRAY);

  objs->scoreLabel = NULL;
  if (app->winner != 3) {
    snprintf(temp, sizeof(temp), "SCORE: %d",
             app->winnerScore == -1337 ? 0 : app->winnerScore);
    objs->scoreLabel = createCenteredText(app, temp, mainFont, 150, COLOR_GRAY);
  }

  objs->enterNameLabel = NULL;

  objs->nameInput = NULL;

  objs->placeLabel = NULL;

  objs->placeInTable = findPlaceInLeaderboard(app->winnerScore);

  // if player in the top 10 in the leaderboard
  if (objs->placeInTable != -1) {
    objs->enterNameLabel = createRenderObject(
        app->renderer, TEXT, 0, b_NONE, "Enter ur name:", smallFont,
        &(SDL_Point){0, 0}, &(SDL_Color){128, 128, 128, 255});
    objs->enterNameLabel->data.texture.constRect.x =
        (app->screenWidth / app->scalingFactorX -
         objs->enterNameLabel->data.texture.constRect.w) /
        2;
    objs->enterNameLabel->data.texture.constRect.y = 300;

    objs->nameInput = createRenderObject(
        app->renderer, TEXT_INPUT, 1, bTI_NAME,
        &(SDL_Rect){objs->enterNameLabel->data.texture.constRect.x - 80,
                    objs->enterNameLabel->data.texture.constRect.y +
                        objs->enterNameLabel->data.texture.constRect.h,
                    400, 50},
        15, 9, smallFont);

    snprintf(temp, sizeof(temp), "YOU'RE RANKED #%d on the leaderboard",
             objs->placeInTable);

    objs->placeLabel = createRenderObject(app->renderer, TEXT, 0, b_NONE, temp,
                                          smallFont, &(SDL_Point){0, 0},
                                          &(SDL_Color){128, 128, 128, 255});
    objs->placeLabel->data.texture.constRect.x =
        (app->screenWidth / app->scalingFactorX -
         objs->placeLabel->data.texture.constRect.w) /
        2;
    objs->placeLabel->data.texture.constRect.y = 250;
  }

  RenderObject* continueButton = createRenderObject(
      app->renderer, TEXT | CAN_BE_TRIGGERED, 0, b_LEADERBOARD_CONTINUE,
      "SAVE & CONTINUE", mainFont, &(SDL_Point){0, 0},
      &(SDL_Color){255, 255, 255, 255}, &(SDL_Color){230, 25, 25, 255});
  continueButton->data.texture.constRect.x =
      (app->screenWidth / app->scalingFactorX -
       continueButton->data.texture.constRect.w) /
      2;
  continueButton->data.texture.constRect.y = 670;

  TTF_CloseFont(smallFont);
  TTF_CloseFont(mainFont);
}

inline static void leaderboardAddMainClear(App* app,
                                           leaderboardAddMainObjects* objs,
                                           char* temp) {
  if (objs->placeInTable != -1) {
    strcpy(temp, objs->nameInput->data.textInputLine.savedText);
    freeRenderObject(objs->enterNameLabel);
    SDL_DestroyTexture(objs->nameInput->data.textInputLine.textTexture);
    free(objs->nameInput);
    freeRenderObject(objs->placeLabel);
  } else {
    memset(temp, 0x00, 256);
  }

  freeRenderObject(objs->wonLabel);
  freeRenderObject(objs->scoreLabel);
  freeRenderObject(objs->continueButton);

  free(objs);
}

void leaderboardAddMain(App* app) {
  leaderboardAddMainObjects objs = {0};
  leaderboardAddMainInit(app, &objs);

  RenderObject* objectsArr[] = {objs.wonLabel,       objs.scoreLabel,
                                objs.enterNameLabel, objs.nameInput,
                                objs.placeLabel,     objs.continueButton};

  while (app->currState == LEADERBOARD_ADD) {
    pollAllEvents(app);
    // filling up the background with black color and clearing render
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    renderTextures(app, objectsArr, sizeof(objectsArr) / sizeof(*objectsArr),
                   SDL_TRUE);

    // rendering
    SDL_RenderPresent(app->renderer);

    SDL_Delay(16);
  }

  char temp[256];

  leaderboardAddMainClear(app, &objs, temp);

  leaderboardMain(app, temp);
}

/////////////////////////////////////////////////////////////////////////////