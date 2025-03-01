#ifndef BASE_VISUALIZATION_H
#define BASE_VISUALIZATION_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class BaseVisualization {
public:
    virtual ~BaseVisualization() {}
    virtual bool initialize(int windowWidth, int windowHeight) = 0;
    virtual void render(const std::vector<float>& fftMagnitudes) = 0;
    virtual bool shouldClose() = 0;
    virtual void cleanup() = 0;
};

#endif
