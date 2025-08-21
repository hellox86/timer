current_path = $(shell pwd)

all: 
	gcc -Wall -O0 -I/usr/local/include -L/usr/local/lib -lraylib -lpthread main.c 

