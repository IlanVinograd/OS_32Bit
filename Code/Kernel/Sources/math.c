#include "../Includes/math.h"

#define ITERATIONS 10 // Number of iterations for Taylor series approximations

float sinf(float x) {
    x = fmodf(x, 2 * PI); // Reduce x to the range [-2*PI, 2*PI]
    float term = x, sum = x;
    for (int n = 1; n <= ITERATIONS; ++n) {
        term *= -x * x / ((2 * n) * (2 * n + 1));
        sum += term;
    }
    return sum;
}

float cosf(float x) {
    x = fmodf(x, 2 * PI); // Reduce x to the range [-2*PI, 2*PI]
    float term = 1, sum = 1;
    for (int n = 1; n <= ITERATIONS; ++n) {
        term *= -x * x / ((2 * n - 1) * (2 * n));
        sum += term;
    }
    return sum;
}

float tanf(float x) {
    float cos_val = cosf(x);
    if (cos_val == 0) {
        return 0; // Undefined behavior; avoid division by zero
    }
    return sinf(x) / cos_val;
}

float sqrtf(float x) {
    if (x < 0) {
        return 0; // Undefined for negative numbers
    }
    float guess = x / 2.0f;
    for (int i = 0; i < ITERATIONS; ++i) {
        guess = (guess + x / guess) / 2.0f;
    }
    return guess;
}

float fabsf(float x) {
    return (x < 0) ? -x : x;
}

float powf(float base, float exponent) {
    if (exponent == 0) return 1;
    if (exponent < 0) return 1 / powf(base, -exponent);
    float result = 1;
    while (exponent >= 1) {
        if ((int)exponent % 2 == 1) {
            result *= base;
        }
        base *= base;
        exponent = (int)(exponent / 2);
    }
    return result;
}

float fmodf(float x, float y) {
    if (y == 0) {
        return 0; // Avoid division by zero
    }
    return x - (int)(x / y) * y;
}