#include "wind.h"

//#include <>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <stdint.h>

#include "../../SDL/SDL_render.h"
#include "../../SDL/ui_helpers.h"
#include "../../math/math.h"
#include "../../math/rand.h"
#include "log/log.h"
#include "windStruct.h"

inline static void updateWindDirectionIcon(RenderObject* directionIcon,
                                           enum WindDirection direction) {
  switch (direction) {
    case E:
      directionIcon->data.texture.angle = 0;
      break;
    case SE:
      directionIcon->data.texture.angle = 45;
      break;
    case SW:
      directionIcon->data.texture.angle = 135;
      break;
    case W:
      directionIcon->data.texture.angle = 180;
      break;
    case NW:
      directionIcon->data.texture.angle = 225;
      break;
    case NE:
      directionIcon->data.texture.angle = 315;
      break;
  }
}

static void updateWindSpeedLabel(App* app, RenderObject* speedLabel,
                                 double windStrength) {
  int32_t minWindSpeedLabel, maxWindSpeedLabel;
  minWindSpeedLabel = floor(windStrength) - getRandomValue(0, 1);
  minWindSpeedLabel = MAX(minWindSpeedLabel, 0);

  maxWindSpeedLabel = MAX(ceil(windStrength) + getRandomValue(0, 1), 1);

  if (minWindSpeedLabel == maxWindSpeedLabel) {
    maxWindSpeedLabel++;
  }

  app->globalConditions.wind.windStrengthRange.min = minWindSpeedLabel;
  app->globalConditions.wind.windStrengthRange.max = maxWindSpeedLabel;

  char temp[16];
  snprintf(temp, 16, "%d - %d m/s", minWindSpeedLabel % 100,
           maxWindSpeedLabel % 100);
  log_debug("%s, REAL=%.1lf", temp, windStrength);
  TTF_Font* speedLabelFont = loadSmallFont(app, 30);

  // detroying old texture
  SDL_DestroyTexture(speedLabel->data.texture.texture);

  // creating new texture
  speedLabel->data.texture.texture =
      createTextTexture(app->renderer, speedLabelFont, temp, 0, 0, 200, 255);

  TTF_CloseFont(speedLabelFont);
}

void updateWind(App* app) {
  Wind* wind = &app->globalConditions.wind;

  const int windStrengthMult = 2;

  wind->windStrength = (getRandomValue(10, 100) / 10.) * windStrengthMult;
  wind->windDirection = getRandomValue(0, 6);

  updateWindDirectionIcon(wind->directionIcon, wind->windDirection);
  updateWindSpeedLabel(app, wind->speedLabel, wind->windStrength);
}

double getWindAngle(App* app) {
  double res;
  switch (app->globalConditions.wind.windDirection) {
    case E:
      res = 0;
      break;
    case SE:
      res = 45;
      break;
    case SW:
      res = 135;
      break;
    case W:
      res = 180;
      break;
    case NW:
      res = 225;
      break;
    case NE:
      res = 315;
      break;
  }
  return res;
}

// function returns wind strange in range [*p_min, *p_max]
void getWindRange(App* app, int32_t* p_min, int32_t* p_max) {
  if (p_min == NULL || p_max == NULL) {
    return;
  }

  *p_min = app->globalConditions.wind.windStrengthRange.min;
  *p_max = app->globalConditions.wind.windStrengthRange.max;
}
