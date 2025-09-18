#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "../App.h"

int threadEventPoll(void* arg);
int threadCursorTrigger(void* arg);

char filterPressedKey(SDL_Keycode key, int flags);

void LMBReleaseHandle(App* app);
void keyPressHandler(App* app, SDL_Event currEvent);
void proceedSlider(App* app, int sliderPosX);
void proceedShiftedButtons(App* app, enum Button buttonName);
void proceedTextInputLine(App* app, RenderObject* currObj);

#endif