#define _USE_MATH_DEFINES
#include "CircularBarVisualization.h"
#include "ColorUtils.h"
#include <cmath>
#include <iostream>

CircularBarVisualization::CircularBarVisualization() : window(nullptr), vbo(0), vao(0), smoothedFFT(128, 0.0f) {}

CircularBarVisualization::~CircularBarVisualization() {
    cleanup();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // ✅ Update OpenGL viewport when the window resizes
}

bool CircularBarVisualization::initialize(int windowWidth, int windowHeight) {
    std::cerr << "DEBUG: Initializing Circular Bar Visualization..." << std::endl;
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return false;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "Circular Bar Visualization", nullptr, nullptr);
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

    std::cerr << "DEBUG: VAO and VBO configured successfully!" << std::endl;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    shaderProgram = createShaderProgram("visualizations/vertexShader.glsl", "visualizations/fragmentShader.glsl");
    if (shaderProgram == 0) {
        std::cerr << "ERROR: Failed to create shader program!" << std::endl;
        exit(1);
    }
    

    return true;
}


void CircularBarVisualization::render(const std::vector<float>& fftMagnitudes) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);

    if (fftMagnitudes.empty()) {
        std::cerr << "ERROR: No FFT data received!\n";
        return;
    }

    size_t numBars = fftMagnitudes.size() / 8; // Reduce number of bars for clarity
    std::vector<float> vertices;
    
    float innerRadius = 0.3f;  // Minimum radius for bars
    float maxHeight = 0.6f;    // Maximum extension
    float angleStep = (2.0f * M_PI) / numBars;
    
    // Smooth FFT values
    float decayFactor = 0.9f;
    for (size_t i = 0; i < numBars; ++i) {
        smoothedFFT[i] = (smoothedFFT[i] * decayFactor) + (fftMagnitudes[i] * (1.0f - decayFactor));
    }

    for (size_t i = 0; i < numBars; ++i) {
        float angle = i * angleStep;
        float magnitude = smoothedFFT[i];
        float barHeight = innerRadius + magnitude * maxHeight;
        
        // Compute bar endpoints
        float x1 = innerRadius * cos(angle);
        float y1 = innerRadius * sin(angle);
        float x2 = barHeight * cos(angle);
        float y2 = barHeight * sin(angle);

        Color color = getColorFromMagnitude(magnitude, 0.0f, 1.0f); // Map to color

        // Store triangle vertices (for bar width)
        float barWidth = angleStep * 0.4f;  // Controls thickness
        float x3 = innerRadius * cos(angle + barWidth);
        float y3 = innerRadius * sin(angle + barWidth);
        float x4 = barHeight * cos(angle + barWidth);
        float y4 = barHeight * sin(angle + barWidth);

        // Push triangle vertices to buffer
        vertices.insert(vertices.end(), {
            x1, y1, color.r, color.g, color.b,
            x2, y2, color.r, color.g, color.b,
            x3, y3, color.r, color.g, color.b,

            x2, y2, color.r, color.g, color.b,
            x3, y3, color.r, color.g, color.b,
            x4, y4, color.r, color.g, color.b
        });
    }

    // Send data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);

    glfwSwapBuffers(window);
    glfwPollEvents();
}



// bool CircleVisualization::shouldClose() {
//     return glfwWindowShouldClose(window);
// }

bool CircularBarVisualization::shouldClose() {
    bool closed = glfwWindowShouldClose(window);
    if (closed) {
        std::cerr << "DEBUG: Window close detected!" << std::endl;
    }
    return closed;
}
    
void CircularBarVisualization::cleanup() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
