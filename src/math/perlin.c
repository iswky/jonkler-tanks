#include <math.h>
#include <stdlib.h>

#define PERLIN_SIZE 256

int perm[PERLIN_SIZE * 2];

float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
float lerp(float a, float b, float t) { return a + t * (b - a); }
int grad(int hash) { return (hash & 1) ? 1 : -1; }

float perlin1d(float x) {
  int X = (int)floor(x) & 255;
  float xf = x - floor(x);
  float u = fade(xf);
  int g1 = grad(perm[X]);
  int g2 = grad(perm[X + 1]);
  float dot1 = g1 * xf;
  float dot2 = g2 * (xf - 1);
  return lerp(dot1, dot2, u);
}

float perlin1d_octaves(float x, int octaves, float persistence) {
  float total = 0, amplitude = 1, maxValue = 0;
  for (int i = 0; i < octaves; i++) {
    total += perlin1d(x * pow(2, i)) * amplitude;
    maxValue += amplitude;
    amplitude *= persistence;
  }
  return total / maxValue;
}

void generatePermutation(unsigned seed) {
  int basePerm[PERLIN_SIZE];
  for (int i = 0; i < PERLIN_SIZE; i++) basePerm[i] = i;
  srand(seed);
  for (int i = 0; i < PERLIN_SIZE; i++) {
    int j = rand() % PERLIN_SIZE;
    int temp = basePerm[i];
    basePerm[i] = basePerm[j];
    basePerm[j] = temp;
  }
  for (int i = 0; i < PERLIN_SIZE; i++) {
    perm[i] = basePerm[i];
    perm[i + PERLIN_SIZE] = basePerm[i];
  }
}

void generateHeightmap(int* heightmap, unsigned seed, int WIDTH, int HEIGHT) {
  generatePermutation(seed);

  float baseScale = 0.005;
  float rareScale = 0.0005;
  int baseOctaves = 4;
  int rareOctaves = 2;
  float persistence1 = 0.5;
  float persistence2 = 0.8;

  float scaleFactor = 512.0f / WIDTH;
  float scale1 = baseScale * scaleFactor;
  float scale2 = rareScale * scaleFactor;

  int octaves1 = baseOctaves + (WIDTH > 512 ? 2 : 0);
  int octaves2 = rareOctaves + (WIDTH > 512 ? 1 : 0);

  for (int x = 0; x < WIDTH; x++) {
    float baseNoise = perlin1d_octaves(x * scale1, octaves1, persistence1);
    float rareNoise = perlin1d_octaves(x * scale2, octaves2, persistence2);
    rareNoise = pow(fabs(rareNoise), 4) * (rareNoise > 0 ? 1 : -1);

    float heightValue = baseNoise + rareNoise;
    heightmap[x] = HEIGHT - (int)((heightValue + 1) * 0.7 * HEIGHT * 0.7);
  }
}