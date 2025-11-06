#ifndef LEADERBOARD_BOARD
#define LEADERBOARD_BOARD

#include "../App.h"

typedef struct PlayerScore {
  int32_t score;
  char name[16];
} PlayerScore;

typedef struct leaderboardMainObjects {
  RenderObject* leaderboardLabel;
  RenderObject* returnArrowObj;
  RenderObject* placeLabel;
  RenderObject* nameLabel;
  RenderObject* scoreLabel;
  SDL_Texture* leaderboard;
} leaderboardMainObjects;

typedef struct leaderboardAddMainObjects {
  RenderObject* wonLabel;
  RenderObject* scoreLabel;
  RenderObject* enterNameLabel;
  RenderObject* nameInput;
  RenderObject* placeLabel;
  int32_t placeInTable;
  RenderObject* continueButton;
} leaderboardAddMainObjects;

void leaderboardAddMain(App* app);
//void addToLeaderBoard(const char* name, int32_t score);
void leaderboardMain(App* app, const char* name);

#endif