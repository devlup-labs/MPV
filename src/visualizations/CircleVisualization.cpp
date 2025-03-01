#define _USE_MATH_DEFINES
#include "CircleVisualization.h"
#include <cmath>
#include <iostream>

CircleVisualization::CircleVisualization() : window(nullptr), vbo(0), vao(0), smoothedFFT(128, 0.0f) {}

CircleVisualization::~CircleVisualization() {
    cleanup();
}

bool CircleVisualization::initialize(int windowWidth, int windowHeight) {
    std::cerr << "DEBUG: Initializing Circle Visualization..." << std::endl;
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

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // ✅ Define vertex attribute layout (Position: location 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ✅ Define vertex attribute layout (Color: location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::cerr << "DEBUG: VAO and VBO configured successfully!" << std::endl;

    return true;
}


void CircleVisualization::render(const std::vector<float>& fftMagnitudes) {
    std::cerr << "DEBUG: ENtered CircleVisualization::render()" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT);

    if (fftMagnitudes.empty()) {
        std::cerr << "ERROR: No FFT data received!\n";
        return;
    }

    size_t numPoints = fftMagnitudes.size() / 8;
    std::vector<float> vertices;

    if (smoothedFFT.size() != numPoints) {
        smoothedFFT.resize(numPoints, 0.0f);
    }

    std::cerr << "DEBUG: numPoints = " << numPoints << std::endl;

    if (numPoints == 0) {
        std::cerr << "ERROR: No FFT data received!" << std::endl;
        return;
    }

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

            float r = fabs(sin(angle));  // ✅ Color changes dynamically
            float g = fabs(cos(angle));
            float b = 1.0f - r;

            vertices.insert(vertices.end(), {x, y, r, g, b});
        }
    }

    std::cerr << "DEBUG: Vertex Count: " << vertices.size() / 5 << std::endl;

    if (vertices.empty()) {
        std::cerr << "ERROR: No vertices generated!" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 5);
    std::cerr << "Render Done" << std::endl;
    glfwSwapBuffers(window);
    glfwPollEvents();
}


// bool CircleVisualization::shouldClose() {
//     return glfwWindowShouldClose(window);
// }

bool CircleVisualization::shouldClose() {
    bool closed = glfwWindowShouldClose(window);
    std::cerr << "DEBUG: shouldClose() returned " << closed << std::endl;
    return closed;
}
    
void CircleVisualization::cleanup() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
