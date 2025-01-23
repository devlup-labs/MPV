#include "Audio.h"
#include "Visuals.h"
#include <iostream>

using namespace std;

int main() {
    const size_t BUFFER_SIZE = 1024;
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    string fileName;
    cout << "Enter audio file path: ";
    cin >> fileName;

    AudioProcessor audioProcessor(BUFFER_SIZE);
    if (!audioProcessor.loadAudioFile(fileName)) {
        cerr << "Failed to load audio file." << endl;
        return -1;
    }

    if (!audioProcessor.startProcessing()) {
        cerr << "Failed to start audio processing." << endl;
        return -1;
    }

    Visualization visualization(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!visualization.initialize()) {
        cerr << "Failed to initialize visualization." << endl;
        return -1;
    }

    while (!visualization.shouldClose()) {
        auto fftData = audioProcessor.getFFTData();
        visualization.render(fftData);
    }

    audioProcessor.cleanup();
    visualization.cleanup();

    return 0;
}
