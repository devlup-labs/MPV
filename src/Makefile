# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I"C:/msys64/mingw64/include" -L"C:/msys64/mingw64/lib" -lglew32 -lglfw3 -lopengl32 -lportaudio -lmpg123 -lfftw3f -lsndfile


# Source files
SRC = main.cpp Audio.cpp ShaderUtils.cpp ../audio/AudioReader.cpp ../audio/FFTProcessor.cpp visualizations/CircleVisualization.cpp visualizations/CircularBarVisualization.cpp visualizations/BarVisualization.cpp visualizations/ColorUtils.cpp visualizations/MountainVisualization.cpp

# Output binary
OUT = audio_visualizer

# Target to build the program
$(OUT): $(SRC)
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS)

# Clean target to remove the binary
clean:
	rm -f $(OUT)
