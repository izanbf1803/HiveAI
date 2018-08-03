all:
	g++ main.cc -std=gnu++11 -O0 -IC:\SDL2_32\include -LC:\SDL2_32\lib  -w -Wl,-subsystem,console -lmingw32 -lSDL2main -lSDL2 -o main