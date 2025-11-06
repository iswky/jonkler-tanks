#ifndef LOGO_RENDER_H
#define LOGO_RENDER_H

#include "../App.h"

typedef struct mainMenuObjects {
  RenderObject* logoObject;
} mainMenuObjects;

typedef struct menuRenderLoopObjects {
  RenderObject* infoButtonObj;
  RenderObject* leaderboardObj;
  RenderObject* playTextObj;
  RenderObject* settingsTextObj;
  RenderObject* quitTextObj;
} menuRenderLoopObjects;

void mainMenu(App* app);
void menuRenderLoop(App* app, RenderObject* logo);

#endif