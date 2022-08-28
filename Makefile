CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -std=c++17
LDFLAGS += $(CXXFLAGS)
MAKEFLAGS = -j4

OBJS = src/main.o src/util.o src/wav.o src/sounds.o src/voices.o src/notes.o src/effects.o src/logging.o src/math.o src/codepts.o
TARGET = musiscript

all: $(TARGET)
.PHONY: all run clean play

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
vim_uninstall: util/musiscript.vim
	rm -f ~/.vim/syntax/musiscript.vim

nvim_install: util/musiscript.vim
	mkdir -p ~/.config/nvim/syntax
	cp util/musiscript.vim ~/.config/nvim/syntax/musiscript.vim
nvim_uninstall: util/musiscript.vim
	rm -f ~/.config/nvim/syntax/musiscript.vim

install: $(TARGET)
	cp $(TARGET) /usr/bin/
uninstall:
	rm -f /usr/bin/$(TARGET)
