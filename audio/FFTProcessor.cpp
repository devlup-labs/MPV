#include "FFTProcessor.h"
#include <fftw3.h>
#include <cmath>
#include <iostream>

using namespace std;

FFTProcessor::FFTProcessor(size_t bufferSize)
    : bufferSize(bufferSize), magnitudes(bufferSize / 2, 0.0f) {
    // Allocate FFT input/output arrays
    fftInput = new float[bufferSize];
    fftOutput = new float[bufferSize];

    // Create FFTW plan
    fftPlan = fftwf_plan_r2r_1d(bufferSize, fftInput, fftOutput, FFTW_R2HC, FFTW_MEASURE);
    if (!fftPlan) {
        cerr << "Failed to create FFTW plan." << endl;
    }
}

FFTProcessor::~FFTProcessor() {
    // Destroy FFTW plan and free memory
    fftwf_destroy_plan(static_cast<fftwf_plan>(fftPlan));
    delete[] fftInput;
    delete[] fftOutput;
}

void FFTProcessor::computeFFT(const vector<float>& audioData) {
    if (audioData.size() < bufferSize) {
        cerr << "Audio data size is smaller than the buffer size." << endl;
        return;
    }

    // Copy audio data to fftInput
    for (size_t i = 0; i < bufferSize; ++i) {
        fftInput[i] = audioData[i];
    }

    // Execute FFT
    fftwf_execute(static_cast<fftwf_plan>(fftPlan));

    // Compute magnitudes from FFT output
    for (size_t i = 0; i < bufferSize / 2; ++i) {
        float real = fftOutput[i];
        float imag = (i == 0 || i == bufferSize / 2) ? 0 : fftOutput[bufferSize - i];
        magnitudes[i] = sqrt(real * real + imag * imag);
    }
}

const vector<float>& FFTProcessor::getMagnitudes() const {
    return magnitudes;
}
