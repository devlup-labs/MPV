#define _USE_MATH_DEFINES
#include "CircleVisualization.h"
#include <cmath>
#include <iostream>

CircleVisualization::CircleVisualization() : window(nullptr), vbo(0), vao(0), smoothedFFT(128, 0.0f) {}

CircleVisualization::~CircleVisualization() {
    cleanup();
}

bool CircleVisualization::initialize(int windowWidth, int windowHeight) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return false;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "Circle Visualization", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        return false;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    return true;
}

void CircleVisualization::render(const std::vector<float>& fftMagnitudes) {
    glClear(GL_COLOR_BUFFER_BIT);

    size_t numPoints = fftMagnitudes.size();
    std::vector<float> vertices;

    float decayFactor = 0.9f;
    for (size_t i = 0; i < numPoints; ++i) {
        smoothedFFT[i] = (smoothedFFT[i] * decayFactor) + (fftMagnitudes[i] * (1.0f - decayFactor));
    }

    float maxRadius = 0.9f;
    float minRadius = 0.3f;
    float centerX = 0.0f, centerY = 0.0f;

    for (size_t i = 0; i < numPoints; i += 8) {
        float angle = 2.0f * M_PI * i / numPoints;
        float magnitude = smoothedFFT[i];
        float radius = minRadius + (maxRadius - minRadius) * magnitude * 0.5f;

        for (int j = 0; j <= 360; j += 10) {
            float theta = j * M_PI / 180.0f;
            float x = centerX + radius * cos(theta + angle);
            float y = centerY + radius * sin(theta + angle);

            vertices.push_back(x);
            vertices.push_back(y);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 2);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool CircleVisualization::shouldClose() {
    return glfwWindowShouldClose(window);
}

void CircleVisualization::cleanup() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
