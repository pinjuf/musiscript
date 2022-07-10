CXX = g++
CXXFLAGS +=
LDFLAGS +=
MAKEFLAGS = -j4

OBJS = main.o sounds.o wav.o notes.o voices.o
TARGET = musiscript

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS) $(CXXFLAGS)

%.o: %.cpp %.h
	$(CXX) -c $< -o $@ $(CXXFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm $(OBJS)
	rm $(TARGET)
