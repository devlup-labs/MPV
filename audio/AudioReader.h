#ifndef AUDIO_READER_H
#define AUDIO_READER_H

#include <string>
#include <vector>

using namespace std;

class AudioFileReader {
public:
    AudioFileReader(); 
    ~AudioFileReader();

    bool loadFile(const string& filePath); 
    const vector<float>& getLeftChannel() const; 
    const vector<float>& getRightChannel() const;
    int getSampleRate() const;

private:
    bool loadMP3(const string& filePath);  
    bool loadWAV(const string& filePath);  

    vector<float> leftChannel;  
    vector<float> rightChannel; 
    int sampleRate;             
};

#endif 
