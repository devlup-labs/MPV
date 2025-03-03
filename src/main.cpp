#include "Audio.h"
#include "visualizations/BaseVisualization.h"
#include "visualizations/CircleVisualization.h"
#include "visualizations/BarVisualization.h"
#include "visualizations/CircularBarVisualization.h"
#include "visualizations/MountainVisualization.h"

#include <iostream>
#include <memory>

using namespace std;

int main() {
    const size_t BUFFER_SIZE = 1024;
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    string fileName;
    cout << "Enter audio file path: ";
    cin >> fileName;

    int choice;
    cout << "\nSelect visualization:\n";
    cout << "1. Circle Visualization\n";
    cout << "2. Bar Visualization\n";
    cout << "3. Circular Bar Visualization\n";
    cout << "4. Mountain Visualization\n";
    cout << "Enter choice: ";
    cin >> choice;

    unique_ptr<BaseVisualization> visualization;

    switch (choice) {
        case 1:
            visualization = make_unique<CircleVisualization>();
            break;
        case 2:
            visualization = make_unique<BarVisualization>();
            break;
        case 3:
            visualization = make_unique<CircularBarVisualization>();
            break;
        case 4:
            visualization = make_unique<MountainVisualization>();
            break;
        default:
            cout << "Invalid choice. Exiting.\n";
            return -1;
    }

    if (!visualization->initialize(WINDOW_WIDTH, WINDOW_HEIGHT)) {
        cerr << "Failed to initialize visualization." << endl;
        return -1;
    }

    AudioProcessor audioProcessor(BUFFER_SIZE);
    if (!audioProcessor.loadAudioFile(fileName)) {
        cerr << "Failed to load audio file." << endl;
        return -1;
    }

    if (!audioProcessor.startProcessing()) {
        cerr << "Failed to start audio processing." << endl;
        return -1;
    }

    while (!visualization->shouldClose()) {
        auto fftData = audioProcessor.getFFTData();
        visualization->render(fftData);
    }

    audioProcessor.cleanup();
    visualization->cleanup();

    return 0;
}
