#ifndef PERLIN_H
#define PERLIN_H

float perlin1d(float x);
float perlin1d_octaves(float x, int octaves, float persistence);
void generatePermutation(unsigned seed);
void generateHeightmap(int* heightmap, unsigned seed, int WIDTH, int HEIGHT);
void showHeightMapUTF8(int* heightmap, int WIDTH, int HEIGHT);

#endif