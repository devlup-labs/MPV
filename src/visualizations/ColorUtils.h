#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <array>  // ✅ Standard C++ header for fixed-size arrays

struct Color {
    float r, g, b;
};

Color getColorFromMagnitude(float magnitude, float minMagnitude, float maxMagnitude);

#endif  // COLOR_UTILS_H
