CXX = g++
CXXFLAGS +=
LDFLAGS +=
MAKEFLAGS = -j4

OBJS = main.o sounds.o wav.o notes.o voices.o
TARGET = musiscript

all: $(TARGET)

$(TARGET):
	$(MAKE) -C src all

run: $(TARGET)
	./$(TARGET)

play: run
	ffplay data/out.wav 2> /dev/null

clean:
	rm src/*.o
	rm $(TARGET)
