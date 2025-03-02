#ifndef BAR_VISUALIZATION_H
#define BAR_VISUALIZATION_H

#include "..\ShaderUtils.h"
#include "BaseVisualization.h"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class BarVisualization : public BaseVisualization {
public:
    BarVisualization();
    ~BarVisualization();

    bool initialize(int windowWidth, int windowHeight) override;
    void render(const std::vector<float>& fftMagnitudes) override;
    bool shouldClose() override;
    void cleanup() override;

private:
    GLFWwindow* window;
    GLuint vbo, vao;
    std::vector<float> smoothedFFT;
    GLuint shaderProgram;
};

#endif
