#ifndef BUFFS_H
#define BUFFS_H

#include <SDL2/SDL.h>

typedef struct {
  SDL_bool weaponIsBroken;
  SDL_bool isDoubleDamage;
  SDL_bool isShielded;
} Buffs;

#endif