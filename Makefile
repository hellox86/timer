current_path = $(shell pwd)

all: 
	gcc -Wall -O0 main.c -o timer `pkg-config --cflags --libs sdl2` -pthread -lSDL2_ttf


