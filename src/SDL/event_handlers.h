#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "../App.h"

int32_t pollAllEvents(void* arg);
int32_t threadCursorTrigger(void* arg);

void proceedSlider(App* app, int32_t sliderPosX);
void proceedShiftedButtons(App* app, enum Button buttonName);
void proceedTextInputLine(App* app, RenderObject* currObj);

#endif