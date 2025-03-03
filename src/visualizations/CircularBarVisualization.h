#ifndef CIRCULARBAR_H
#define CIRCULARBAR_H

#include "BaseVisualization.h"
#include "..\ShaderUtils.h"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class CircularBarVisualization : public BaseVisualization {
public:
    CircularBarVisualization();
    ~CircularBarVisualization();

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