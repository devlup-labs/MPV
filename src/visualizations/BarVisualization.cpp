#define _USE_MATH_DEFINES  //  Ensures M_PI is defined
#include "BarVisualization.h"
#include "ColorUtils.h"
#include <cmath>
#include <iostream>

BarVisualization::BarVisualization() : window(nullptr), vbo(0), vao(0), smoothedFFT(128, 0.0f) {}

BarVisualization::~BarVisualization() {
    cleanup();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // ✅ Update OpenGL viewport when the window resizes
}

bool BarVisualization::initialize(int windowWidth, int windowHeight) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return false;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "Bar Visualization", nullptr, nullptr);
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

void BarVisualization::render(const std::vector<float>& fftMagnitudes) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);  // ✅ Ensure shaders are active before drawing

    if (fftMagnitudes.empty()) {
        std::cerr << "ERROR: No FFT data received!\n";
        return;
    }

    size_t numBars = fftMagnitudes.size() / 8; // Reduce number of bars for clarity
    std::vector<float> vertices;

    float barWidth = 2.0f / numBars; // Normalize to OpenGL's -1 to 1 range
    float maxHeight = 0.9f; // Ensure bars stay within screen height
    float minHeight = 0.02f; // Minimum bar height to keep them visible

    // Ensure smoothedFFT is the correct size
    if (smoothedFFT.size() != numBars) {
        smoothedFFT.resize(numBars, 0.0f);
    }

    // Find max magnitude for scaling
    float maxMagnitude = 0.0f;
    for (size_t i = 0; i < numBars; ++i) {
        if (fftMagnitudes[i] > maxMagnitude) {
            maxMagnitude = fftMagnitudes[i];
        }
    }
    if (maxMagnitude < 1e-6) {  // Prevent division by zero
        maxMagnitude = 1.0f;
    }

    float decayFactor = 0.9f;

    for (size_t i = 0; i < numBars; ++i) {
        float rawMag = fftMagnitudes[i];

        // Normalize magnitude relative to the maxMagnitude
        float normalizedMag = rawMag / maxMagnitude;

        // Apply log scaling (adding 1 to avoid log(0))
        float logMag = log10(1 + normalizedMag * 10) / log10(2); // Scales between 0 and 1

        // Apply exponential smoothing to stabilize fluctuations
        smoothedFFT[i] = (smoothedFFT[i] * decayFactor) + ((1.0f - decayFactor) * logMag);

        // Compute final height
        float height = smoothedFFT[i] * maxHeight;
        if (height < minHeight) height = minHeight;  // Prevents bars from disappearing

        float x = -1.0f + i * barWidth;
        Color color = getColorFromMagnitude(smoothedFFT[i], 0.0f, 1.0f); // ✅ Dynamic color

        vertices.insert(vertices.end(), {
            x, -1.0f,   color.r, color.g, color.b,
            x + barWidth * 0.8f, -1.0f,  color.r, color.g, color.b,
            x, height - 1.0f,  color.r, color.g, color.b,

            x + barWidth * 0.8f, -1.0f,  color.r, color.g, color.b,
            x + barWidth * 0.8f, height - 1.0f,  color.r, color.g, color.b,
            x, height - 1.0f,  color.r, color.g, color.b
        });
    }


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool BarVisualization::shouldClose() {
    return glfwWindowShouldClose(window);
}

void BarVisualization::cleanup() {
    if (vbo != 0) glDeleteBuffers(1, &vbo);
    if (vao != 0) glDeleteVertexArrays(1, &vao);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
