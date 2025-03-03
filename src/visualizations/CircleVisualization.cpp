#define _USE_MATH_DEFINES
#include "CircleVisualization.h"
#include "ColorUtils.h"
#include <cmath>
#include <iostream>

CircleVisualization::CircleVisualization() : window(nullptr), vbo(0), vao(0), smoothedFFT(128, 0.0f) {}

CircleVisualization::~CircleVisualization() {
    cleanup();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // ✅ Update OpenGL viewport when the window resizes
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

    shaderProgram = createShaderProgram("visualizations/vertexShader.glsl", "visualizations/fragmentShader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "ERROR: Failed to create shader program!" << std::endl;
        exit(1);
    }

    glUseProgram(shaderProgram);

    return true;
}


void CircleVisualization::render(const std::vector<float>& fftMagnitudes) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (fftMagnitudes.empty()) {
        std::cerr << "ERROR: No FFT data received!\n";
        return;
    }

    size_t numPoints = fftMagnitudes.size() ;
    std::vector<float> vertices;

    if (smoothedFFT.size() != numPoints) {
        smoothedFFT.resize(numPoints, 0.0f);
    }

    if (numPoints == 0) {
        std::cerr << "ERROR: No FFT data received!" << std::endl;
        return;
    }

    float decayFactor = 0.9f;
    for (size_t i = 0; i < numPoints; ++i) {
        smoothedFFT[i] = (smoothedFFT[i] * decayFactor) + (fftMagnitudes[i] * (1.0f - decayFactor));
    }

    float maxRadius = 0.9f;
    float minRadius = 0.1f;
    float centerX = 0.0f, centerY = 0.0f;

    for (size_t i = 0; i < numPoints; i += 8) {
        float angleOffset = 2.0f * M_PI * i / numPoints;
        float magnitude = smoothedFFT[i];
        float radius = minRadius + (maxRadius - minRadius) * magnitude * 0.5f;
    
        std::vector<float> ringVertices;  // ✅ Store vertices for this ring only

        Color color = getColorFromMagnitude(smoothedFFT[i], 0.0f, 1.0f);
    
        for (int j = 0; j < 360; j += 1) {
            float theta = j * M_PI / 180.0f;
            float x = centerX + radius * cos(theta + angleOffset);
            float y = centerY + radius * sin(theta + angleOffset);
    
            float r = color.r;
            float g = color.g;
            float b = color.b;

            // float r = 0.5f;
            // float g = 0.5f;
            // float b = 0.5f;
            
            ringVertices.insert(ringVertices.end(), {x, y, r, g, b});
        }
    
        // ✅ Send this ring separately to OpenGL
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, ringVertices.size() * sizeof(float), ringVertices.data(), GL_DYNAMIC_DRAW);
    
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_LOOP, 0, ringVertices.size() / 5);  // ✅ Draw only this ring
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}


// bool CircleVisualization::shouldClose() {
//     return glfwWindowShouldClose(window);
// }

bool CircleVisualization::shouldClose() {
    bool closed = glfwWindowShouldClose(window);
    return closed;
}
    
void CircleVisualization::cleanup() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
