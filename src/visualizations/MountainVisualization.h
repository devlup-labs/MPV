#ifndef MOUNTAIN_VISUALIZATION_H
#define MOUNTAIN_VISUALIZATION_H

#include "..\ShaderUtils.h"
#include "BaseVisualization.h"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class MountainVisualization : public BaseVisualization {
public:
    MountainVisualization();
    ~MountainVisualization();

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
