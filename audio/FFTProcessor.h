#ifndef FFTPROCESSOR_H
#define FFTPROCESSOR_H

#include <vector>

using namespace std;

class FFTProcessor {
public:
    FFTProcessor(size_t bufferSize);
    ~FFTProcessor();

    void computeFFT(const vector<float>& audioData);
    const vector<float>& getMagnitudes() const;

private:
    size_t bufferSize;
    vector<float> magnitudes;
    float* fftInput;
    float* fftOutput;
    void* fftPlan;  // Plan type depends on FFTW version
};

#endif // FFTPROCESSOR_H
