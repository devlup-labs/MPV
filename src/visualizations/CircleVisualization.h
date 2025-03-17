#ifndef CIRCLE_VISUALIZATION_H
#define CIRCLE_VISUALIZATION_H

#include "BaseVisualization.h"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class CircleVisualization : public BaseVisualization {
public:
    CircleVisualization();
    ~CircleVisualization();

    bool initialize(int windowWidth, int windowHeight) override;
    void render(const std::vector<float>& fftMagnitudes) override;
    bool shouldClose() override;
    void cleanup() override;

private:
    GLFWwindow* window;
    GLuint vbo, vao;
    std::vector<float> smoothedFFT;
};

#endif
