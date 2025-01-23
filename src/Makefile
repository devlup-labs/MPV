# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -lGLEW -lglfw -lGL -lportaudio -lmpg123 -lfftw3f -lm -lsndfile

# Source files
SRC = main.cpp Audio.cpp Visuals.cpp ../audio/AudioReader.cpp ../audio/FFTProcessor.cpp

# Output binary
OUT = audio_visualizer

# Target to build the program
$(OUT): $(SRC)
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS)

# Clean target to remove the binary
clean:
	rm -f $(OUT)
