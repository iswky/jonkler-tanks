#ifndef BASIC_MATH_H
#define BASIC_MATH_H

#include "../App.h"

#define DEGTORAD(x) (x * M_PI / 180.0)
#define RADTODEG(x) (x * 180.0 / M_PI)
#define MAX(x, y) (x > y ? x : y)

double getAngle(int x, int* heightMap, int dx);
SDL_Point getPixelScreenPosition(SDL_Point drawPos, SDL_Point center,
                                 double angleDeg, SDL_Point target);
double normalizeAngle(double angleDeg);

int getRandomValue(int min, int max);
int getAllowedNumber(App* app);

void getPositionAtSpecTime(SDL_FPoint* pos, double initVel, double angle,
                           double currTime);
int calcHitPosition(SDL_FPoint* initPos, double initVel, double angle,
                    int* heightMap, const App* app, const SDL_Point* collision1,
                    const SDL_Point* collison2, const SDL_Point* collision3,
                    const int collision1R, const int collision2R,
                    const int collision3R, RenderObject* projectile);
void smoothChangeAngle(Player* player, int endAngle, enum State* currState);
void smoothChangePower(Player* player, int endPower, enum State* currState);
int smoothMove(App* app, SDL_bool isFirstPlayer, SDL_bool isRight,
               int* heightMap);

SDL_bool isInTriangle(const int x, const int y, SDL_Point p1, SDL_Point p2,
                      SDL_Point p3);
SDL_bool isInCircle(const int x, const int y, const SDL_Point* center,
                    const int radius);

#endif