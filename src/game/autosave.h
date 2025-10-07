#ifndef AUTOSAVE_H
#define AUTOSAVE_H

#include "../App.h"

void clearSave(App* app);

void saveCurrentState(App* app, Player* firstPlayer, Player* secondPlayer,
                      int* heightMap, int isFirstTarget, unsigned seed);

int loadSavedState(App* app, Player* firstPlayer, Player* secondPlayer,
                   int* heightMap, unsigned* seed);

#endif