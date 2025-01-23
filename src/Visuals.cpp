//Just a dummy visuals.cpp made for checking, (Just have a bar graph visualization)

#include "Visuals.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

Visualization::Visualization(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight), window(nullptr) {}

Visualization::~Visualization() {
    cleanup();
}

bool Visualization::initialize() {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW." << endl;
        return false;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "Audio Visualization", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window." << endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW." << endl;
        return false;
    }
    return true;
}

void Visualization::render(const vector<float>& fftMagnitudes) {
    glClear(GL_COLOR_BUFFER_BIT);

    size_t numBars = fftMagnitudes.size();
    float barWidth = 2.0f / numBars;

    for (size_t i = 0; i < numBars; ++i) {
        float magnitude = fftMagnitudes[i];
        float barHeight = magnitude * 0.8f;

        float x = -1.0f + i * barWidth;
        float y = -1.0f;

        glBegin(GL_QUADS);
        glColor3f(magnitude, 0.5f, 1.0f - magnitude);
        glVertex2f(x, y);
        glVertex2f(x + barWidth, y);
        glVertex2f(x + barWidth, y + barHeight);
        glVertex2f(x, y + barHeight);
        glEnd();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Visualization::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Visualization::cleanup() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
