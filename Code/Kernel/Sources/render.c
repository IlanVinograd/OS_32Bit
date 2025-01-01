#include "../Includes/render.h"

extern TextStyle backGroundColor;
char background[WIDTH * HEIGHT];
char prevBackground[WIDTH * HEIGHT];
float zBuffer[WIDTH * HEIGHT];
float A = 0, B = 0, C = 0;

float roll(int i, int j, int k) {
    return j * sinf(A) * sinf(B) * cosf(C) 
         - k * cosf(A) * sinf(B) * cosf(C) 
         + j * cosf(A) * sinf(C) 
         + k * sinf(A) * sinf(C) 
         + i * cosf(B) * cosf(C);
}

float pitch(int i, int j, int k) {
    return j * cosf(A) * cosf(C) 
         + k * sinf(A) * cosf(C) 
         - j * sinf(A) * sinf(B) * sinf(C) 
         + k * cosf(A) * sinf(B) * sinf(C) 
         - i * cosf(B) * sinf(C);
}

float yaw(int i, int j, int k) {
    return k * cosf(A) * cosf(B) 
         - j * sinf(A) * cosf(B) 
         + i * sinf(B);
}

int cubePos(int x, int y, int z) {
    float X, Y, Z;
    X = roll(x, y, z);
    Y = pitch(x, y, z);
    Z = yaw(x, y, z) + 1000;

    // Screen mapping
    X = (int)(X + VSET);
    Y = (int)(Y / 2 + HSET);

    if (X < 0 || X >= WIDTH || Y < 0 || Y >= HEIGHT) {
        return -1;
    }

    return (int)(Y * WIDTH + X);
}

void insertSymbol(int index, char symbol, float depth) {
    if (index >= 0 && index < WIDTH * HEIGHT) {
        if (depth > zBuffer[index]) {
            zBuffer[index] = depth;
            background[index] = symbol;
        }
    }
}

void renderCube() {
    float velocity = 0.7;

    // Clear buffers
    memset(background, ' ', sizeof(background));
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        zBuffer[i] = -3.402823466e+38F;
    }

    // Render the cube
    for (float iX = -CUBE_SIZE; iX <= CUBE_SIZE; iX += velocity) {
        for (float iY = -CUBE_SIZE; iY <= CUBE_SIZE; iY += velocity) {
            float depth;

            // Front face
            depth = yaw(iX, iY, -CUBE_SIZE);
            if (depth > 1e-5) insertSymbol(cubePos(iX, iY, -CUBE_SIZE), '#', 1 / depth);

            // Back face
            depth = yaw(-iX, iY, CUBE_SIZE);
            if (depth > 1e-5) insertSymbol(cubePos(-iX, iY, CUBE_SIZE), '*', 1 / depth);

            // Left face
            depth = yaw(-CUBE_SIZE, iY, iX);
            if (depth > 1e-5) insertSymbol(cubePos(-CUBE_SIZE, iY, iX), '@', 1 / depth);

            // Right face
            depth = yaw(CUBE_SIZE, iY, iX);
            if (depth > 1e-5) insertSymbol(cubePos(CUBE_SIZE, iY, iX), '%', 1 / depth);

            // Top face
            depth = yaw(iX, CUBE_SIZE, iY);
            if (depth > 1e-5) insertSymbol(cubePos(iX, CUBE_SIZE, iY), '-', 1 / depth);

            // Bottom face
            depth = yaw(iX, -CUBE_SIZE, iY);
            if (depth > 1e-5) insertSymbol(cubePos(iX, -CUBE_SIZE, iY), '+', 1 / depth);
        }
    }

    // Print the cube
    __asm__ ("cli");
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (background[i] != prevBackground[i]) {
            int row = i / WIDTH;
            int col = i % WIDTH;
            setCursorPosition(row, col);
            if (background[i] != ' ') {
                putc(background[i], backGroundColor);
            } else {
                putc(' ', backGroundColor);
            }
        }
    }
    __asm__ ("sti");

    memcpy(prevBackground, background, sizeof(background));
}