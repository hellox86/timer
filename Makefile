current_path = $(shell pwd)

all: 
	gcc -O2 -lncurses $(current_path)/*.c -o timer
