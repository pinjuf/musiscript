CXX = g++
CXXFLAGS =
LDFLAGS =
MAKEFLAGS = -j4

OBJS = src/main.o src/wav.o src/sounds.o src/voices.o src/notes.o src/effects.o src/logging.o
TARGET = musiscript

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp %.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

play: run
	ffplay data/out.wav -showmode 1 2> /dev/null
