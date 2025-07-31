PREFIX = i686-w64-mingw32
FLAGS = -Wall -std=c++11 -m32 -static -lws2_32 -lpsapi -lwinpthread -lwinmm -lole32 -ldinput -lwininet -ldwmapi -lgdi32 -lshlwapi
CC = $(PREFIX)-g++

all: takeover.exe

takeover.exe: main.cpp
	$(CC) -o $@ $(FLAGS) -Iinclude/ $<

clean:
	rm -f takeover.exe
