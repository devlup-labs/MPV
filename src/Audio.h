#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <portaudio.h>

using namespace std;

class AudioProcessor {
public:
    AudioProcessor(size_t bufferSize);
    ~AudioProcessor();

    bool loadAudioFile(const string& fileName);

    vector<float> getFFTData();

    bool startProcessing();

    void cleanup();

private:
    size_t bufferSize;
    class AudioFileReader* audioReader;
    class FFTProcessor* fftProcessor;

    vector<float> sharedBuffer;
    mutex audioMutex;
    condition_variable bufferReady;
    bool isBufferReady;

    void* stream;

    static int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
};


#endif 
