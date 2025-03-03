#define _USE_MATH_DEFINES  //  Ensures M_PI is defined
#include "MountainVisualization.h"
#include "ColorUtils.h"
#include <cmath>
#include <iostream>

MountainVisualization::MountainVisualization() : window(nullptr), vbo(0), vao(0), smoothedFFT(128, 0.0f) {}
MountainVisualization::~MountainVisualization() {
    cleanup();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // ✅ Update OpenGL viewport when the window resizes
}

bool MountainVisualization::initialize(int windowWidth, int windowHeight) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return false;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "Mountain Visualization", nullptr, nullptr);
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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    shaderProgram = createShaderProgram("visualizations/vertexShader.glsl", "visualizations/fragmentShader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "ERROR: Failed to create shader program!" << std::endl;
        exit(1);
    }

    glUseProgram(shaderProgram);  // ✅ Ensure OpenGL uses the shaders

    return true;
}

void MountainVisualization::render(const std::vector<float>& fftMagnitudes) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);

    if (fftMagnitudes.empty()) {
        std::cerr << "ERROR: No FFT data received!\n";
        return;
    }

    float maxMagnitude = 0.0f;
    for (float mag : fftMagnitudes) {
        if (mag > maxMagnitude) {
            maxMagnitude = mag;
        }
    }

    size_t numPoints = fftMagnitudes.size() / 8; // Reduce number of points for clarity
    std::vector<float> vertices;

    float maxHeight = 1.0f; // Peak height
    float minHeight = -1.0f; // Base height
    float spacing = 2.0f / (numPoints - 1); // Space out points evenly along x-axis

    // Ensure smoothedFFT has the right size
    if (smoothedFFT.size() != numPoints) {
        smoothedFFT.resize(numPoints, 0.0f);
    }

    // Apply smoothing
    float smoothingFactor = 0.9f;
    for (size_t i = 0; i < numPoints; ++i) {
        float normalizedMag = fftMagnitudes[i] / maxMagnitude; 
        smoothedFFT[i] = (smoothingFactor * smoothedFFT[i]) + ((1.0f - smoothingFactor) * normalizedMag);

        float x = -1.0f + i * spacing;
        float y = smoothedFFT[i] * maxHeight + minHeight; // Scale y values

        Color color = getColorFromMagnitude(smoothedFFT[i], 0.0f, 1.0f);

        // Store vertex (x, y) and color
        vertices.insert(vertices.end(), { x, y, color.r, color.g, color.b });
    }

    // Send data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, numPoints); // ✅ Connect points into a line

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool MountainVisualization::shouldClose() {
    return glfwWindowShouldClose(window);
}

void MountainVisualization::cleanup() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
