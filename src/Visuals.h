#ifndef VISUALS_H
#define VISUALS_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

class Visualization {
public:
    Visualization(int windowWidth, int windowHeight);
    ~Visualization();

    // Initialize the GLFW window
    bool initialize();

    // Render the FFT visualization
    void render(const vector<float>& fftMagnitudes);

    // Check if the window should close
    bool shouldClose();

    // Cleanup resources
    void cleanup();

private:
    int windowWidth;
    int windowHeight;
    GLFWwindow* window;
};

#endif // VISUALIZATION_H
