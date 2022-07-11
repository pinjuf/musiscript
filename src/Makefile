CXX = g++
CXXFLAGS +=
LDFLAGS +=
MAKEFLAGS =

OBJS = main.o sounds.o wav.o notes.o voices.o
TARGET = ../musiscript

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS) $(CXXFLAGS)

%.o: %.cpp %.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)
