CXX = g++
CXXFLAGS =
LDFLAGS += $(CXXFLAGS)
MAKEFLAGS = -j4

OBJS = src/main.o src/wav.o src/sounds.o src/voices.o src/notes.o src/effects.o src/logging.o src/math.o src/codepts.o
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

vim_install: util/musiscript.vim
	mkdir -p ~/.vim/syntax
	cp util/musiscript.vim ~/.vim/syntax/musiscript.vim

nvim_install: util/musiscript.vim
	mkdir -p ~/.config/nvim/syntax
	cp util/musiscript.vim ~/.config/nvim/syntax/musiscript.vim

install: $(TARGET)
	cp $(TARGET) /usr/bin/

uninstall:
	rm -f /usr/bin/$(TARGET)
