#include "ColorUtils.h"
#include <algorithm>

// ✅ Function to map magnitude to RGB color
Color getColorFromMagnitude(float magnitude, float minMagnitude, float maxMagnitude) {
    float normalized = (magnitude - minMagnitude) / (maxMagnitude - minMagnitude); // Normalize to 0-1 range
    
    float r = std::clamp(normalized, 0.0f, 1.0f);   // ✅ Ensure valid range
    float g = std::clamp(1.0f - normalized, 0.0f, 1.0f);
    float b = std::clamp(normalized, 0.0f, 1.0f);      // ✅ Blue increases

    return { r , g , b  };  // Normalize to OpenGL's 0-1 range
}
