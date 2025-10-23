#include "wind.h"

//#include <>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdint.h>

#include "../../SDL/SDL_render.h"
#include "../../SDL/ui_helpers.h"
#include "../../math/math.h"
#include "../../math/rand.h"
#include "windStruct.h"

void updateWind(App* app) {
  Wind* wind = &app->globalConditions.wind;

  wind->windStrength = getRandomValue(0, 50) / 10.;
  wind->windDirection = getRandomValue(0, 6);

  updateWindDirectionIcon(wind->directionIcon, wind->windDirection);
  updateWindSpeedLabel(app, wind->speedLabel, wind->windStrength);
}

void updateWindDirectionIcon(RenderObject* directionIcon,
                             enum WindDirection direction) {
  directionIcon->data.texture.angle = direction * 45;
}

void updateWindSpeedLabel(App* app, RenderObject* speedLabel,
                          double windStrength) {
  int32_t minWindSpeedLabel, maxWindSpeedLabel;
  minWindSpeedLabel = floor(windStrength) - getRandomValue(0, 1);
  minWindSpeedLabel = MAX(minWindSpeedLabel, 0);

  maxWindSpeedLabel = ceil(windStrength) + getRandomValue(0, 1);

  char temp[16];
  snprintf(temp, 16, "%d-%d m/s", minWindSpeedLabel % 50,
           maxWindSpeedLabel % 50);

  TTF_Font* speedLabelFont = loadSmallFont(app, 30);

  // detroying old texture
  SDL_DestroyTexture(speedLabel->data.texture.texture);

  // creating new texture
  speedLabel->data.texture.texture =
      createTextTexture(app->renderer, speedLabelFont, temp, 0, 0, 200, 255);

  TTF_CloseFont(speedLabelFont);
}