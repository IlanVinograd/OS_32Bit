#ifndef _RENDER_H_
#define _RENDER_H_

#include <vga.h>
#include <string.h>
#include <math.h>
#include <test.h>

#define WIDTH 84
#define HEIGHT 24
#define CUBE_SIZE 9 // Smaller size for better fit
#define VSET 43     // Adjust for better horizontal centering
#define HSET 13     // Adjust for better vertical centering

float roll(int i, int j, int k);
float pitch(int i, int j, int k);
float yaw(int i, int j, int k);
int cubePos(int x, int y, int z);
void insertSymbol(int index, char symbol, float depth);
void renderCube();

#endif /* _RENDER_H_ */
