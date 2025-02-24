#include "Visuals.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>

using namespace std;

Visualization::Visualization(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight), window(nullptr), vbo(0), vao(0), shaderProgram(0), smoothedFFT(128, 0.0f) {}

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

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return true;
}
void Visualization::render(const vector<float>& fftMagnitudes) {
    glClear(GL_COLOR_BUFFER_BIT);

    size_t numPoints = fftMagnitudes.size();
    vector<float> vertices;

    float decayFactor = 0.95f; 
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

    
    static size_t lastVertexCount = 0;
    if (vertices.size() > lastVertexCount) {
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        lastVertexCount = vertices.size();
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 2);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Visualization::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Visualization::cleanup() {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
