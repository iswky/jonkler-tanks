#ifndef WIND_H
#define WIND_H

#include "../../App.h"

void updateWind(App* app);

enum WindDirection getWindDir(App* app);
void getWindRange(App* app, int32_t* p_min, int32_t* p_max);
double getWindAngle(App* app);

#endif