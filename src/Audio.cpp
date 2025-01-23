#include "Audio.h"
#include "../audio/AudioReader.h"
#include "../audio/FFTProcessor.h"
#include <portaudio.h>
#include <iostream>
#include <algorithm>

using namespace std;

AudioProcessor::AudioProcessor(size_t bufferSize) 
    : bufferSize(bufferSize), audioReader(nullptr), fftProcessor(nullptr), stream(nullptr), isBufferReady(false) {
    sharedBuffer.resize(bufferSize);
}

AudioProcessor::~AudioProcessor() {
    cleanup();
}

bool AudioProcessor::loadAudioFile(const string& fileName) {
    audioReader = new AudioFileReader();
    if (!audioReader->loadFile(fileName)) {
        cerr << "Failed to load audio file: " << fileName << endl;
        return false;
    }
    fftProcessor = new FFTProcessor(bufferSize);
    return true;
}

bool AudioProcessor::startProcessing() {
    if (!audioReader || !fftProcessor) {
        cerr << "AudioProcessor not initialized properly." << endl;
        return false;
    }

    Pa_Initialize();
    Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, audioReader->getSampleRate(), bufferSize, audioCallback, this);
    Pa_StartStream(static_cast<PaStream*>(stream));
    return true;
}

void AudioProcessor::cleanup() {
    if (stream) {
        Pa_StopStream(static_cast<PaStream*>(stream));
        Pa_CloseStream(static_cast<PaStream*>(stream));
        stream = nullptr;
    }
    Pa_Terminate();

    delete fftProcessor;
    fftProcessor = nullptr;

    delete audioReader;
    audioReader = nullptr;
}

vector<float> AudioProcessor::getFFTData() {
    unique_lock<mutex> lock(audioMutex);
    bufferReady.wait(lock, [this] { return isBufferReady; });

    fftProcessor->computeFFT(sharedBuffer);
    isBufferReady = false;
    return fftProcessor->getMagnitudes();
}

int AudioProcessor::audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
                                   const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    AudioProcessor* processor = static_cast<AudioProcessor*>(userData);
    float* out = static_cast<float*>(outputBuffer);
    auto& leftChannel = processor->audioReader->getLeftChannel();

    static size_t currentOffset = 0;
    if (currentOffset + framesPerBuffer <= leftChannel.size()) {
        copy(leftChannel.begin() + currentOffset, leftChannel.begin() + currentOffset + framesPerBuffer, out);

        {
            lock_guard<mutex> lock(processor->audioMutex);
            copy(leftChannel.begin() + currentOffset, leftChannel.begin() + currentOffset + framesPerBuffer, processor->sharedBuffer.begin());
            processor->isBufferReady = true;
        }
        processor->bufferReady.notify_one();
        currentOffset += framesPerBuffer;
    } else {
        return paComplete;
    }

    return paContinue;
}
