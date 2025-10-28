#include "spread.h"

#include "../../math/rand.h"
#include "log/log.h"


double getMaxSpreadByWeapon(int weapon) {
    if (weapon == 0) return 1.0f;
    if (weapon == 1) return 2.0f;
    if (weapon == 2) return 3.0f;
    if (weapon == 3) return 5.0f;
    return 2.0f;
}

static int getRandomSpread() {
    return getRandomValue(0, 100);
}

double addSpreadToCurrAngle(int weapon, double currAngle) {
    double maxSpread = getMaxSpreadByWeapon(weapon);
    int offset = getRandomSpread();

    double spread = ((double)(offset % 100) / 100.0f) * 2 * maxSpread - maxSpread;

    return currAngle + spread;
}

void renderSpreadLines(App* app) {
    
}