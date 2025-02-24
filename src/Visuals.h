#ifndef VISUALS_H
#define VISUALS_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Visualization {
public:
    Visualization(int windowWidth, int windowHeight);
    ~Visualization();

    bool initialize();
    void render(const std::vector<float>& fftMagnitudes);
    bool shouldClose();
    void cleanup();

private:
    int windowWidth, windowHeight;
    GLFWwindow* window;
    GLuint vbo, vao, shaderProgram;

    std::vector<float> smoothedFFT;  // ✅ Added this to store smoothed FFT data
};

#endif
