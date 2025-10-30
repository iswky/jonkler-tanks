#ifndef SPREAD_H
#define SPREAD_H

#include "../../App.h"

double getMaxSpreadByWeapon(int weapon);
double addSpreadToCurrAngle(int weapon, double currAngle);

void renderSpreadArea(App* app, RenderObject* spreadArea);

#endif