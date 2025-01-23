#include "AudioReader.h"
#include <mpg123.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <sndfile.h>

using namespace std;

AudioFileReader::AudioFileReader() : sampleRate(0) {
    if (mpg123_init() != MPG123_OK) {
        cerr << "Failed to initialize mpg123 library." << endl;
    }
}

AudioFileReader::~AudioFileReader() {
    mpg123_exit();
}

bool AudioFileReader::loadMP3(const string& filePath) {
    mpg123_handle* mh = mpg123_new(nullptr, nullptr);
    if (!mh) {
        cerr << "Failed to create mpg123 handle." << endl;
        return false;
    }

    if (mpg123_open(mh, filePath.c_str()) != MPG123_OK) {
        cerr << "Failed to open file: " << filePath << endl;
        mpg123_delete(mh);
        return false;
    }

    long rate;
    int channels, encoding;
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
        cerr << "Failed to get audio format." << endl;
        mpg123_close(mh);
        mpg123_delete(mh);
        return false;
    }

    sampleRate = static_cast<int>(rate);

    size_t bufferSize = mpg123_outblock(mh);
    unsigned char* buffer = new unsigned char[bufferSize];
    size_t done;
    vector<float> interleavedData;

    while (mpg123_read(mh, buffer, bufferSize, &done) == MPG123_OK) {
        for (size_t i = 0; i < done; i += sizeof(short)) {
            short sample = *reinterpret_cast<short*>(buffer + i);
            interleavedData.push_back(sample / 32768.0f);
        }
    }

    delete[] buffer;

    leftChannel.clear();
    rightChannel.clear();

    if (channels == 2) {
        for (size_t i = 0; i < interleavedData.size(); i += 2) {
            leftChannel.push_back(interleavedData[i]);
            rightChannel.push_back(interleavedData[i + 1]);
        }
    } else if (channels == 1) {
        leftChannel = interleavedData;
        rightChannel = interleavedData;
    }

    mpg123_close(mh);
    mpg123_delete(mh);

    return true;
}

bool AudioFileReader::loadWAV(const string& filePath) {
    SF_INFO sfInfo;
    SNDFILE* file = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!file) {
        cerr << "Failed to open WAV file: " << filePath << endl;
        return false;
    }

    sampleRate = sfInfo.samplerate;
    vector<float> interleavedData(sfInfo.frames * sfInfo.channels);
    sf_readf_float(file, interleavedData.data(), sfInfo.frames);
    sf_close(file);

    leftChannel.clear();
    rightChannel.clear();

    if (sfInfo.channels == 2) {
        for (size_t i = 0; i < interleavedData.size(); i += 2) {
            leftChannel.push_back(interleavedData[i]);
            rightChannel.push_back(interleavedData[i + 1]);
        }
    } else if (sfInfo.channels == 1) {
        leftChannel = interleavedData;
        rightChannel = interleavedData;
    }

    return true;
}

bool AudioFileReader::loadFile(const string& filePath) {
    string extension = filePath.substr(filePath.find_last_of('.') + 1);

    if (extension == "mp3") {
        return loadMP3(filePath);
    }
    else if (extension == "wav") {
        return loadWAV(filePath);
    }

    cerr << "Unsupported file format." << endl;
    return false;
}

const vector<float>& AudioFileReader::getLeftChannel() const {
    return leftChannel;
}

const vector<float>& AudioFileReader::getRightChannel() const {
    return rightChannel;
}

int AudioFileReader::getSampleRate() const {
    return sampleRate;
}
