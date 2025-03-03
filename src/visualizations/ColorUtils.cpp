#include "ColorUtils.h"
#include <cmath>

// HSV to RGB conversion
Color hsvToRgb(float h, float s, float v) {
    float r, g, b;
    int i = static_cast<int>(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return {r, g, b};
}

// Generate color based on magnitude
Color getColorFromMagnitude(float magnitude, float minMagnitude, float maxMagnitude) {
    float normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude); // Normalize to 0-1 range

    // Cycle hue from 0° (red) to 360° (purple)
    float hue = fmod(normalized * 360.0f, 360.0f) / 360.0f; // Normalize hue to [0, 1]
    float saturation = 1.0f;  // Full saturation for vibrancy
    float value = 0.7f + 0.3f * normalized; // Make higher values brighter

    return hsvToRgb(hue, saturation, value);
}

