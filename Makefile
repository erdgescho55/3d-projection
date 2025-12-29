TARGET=demo

build: main.c
	cc main.c -o bin/$(TARGET) -I/usr/local/include/SDL3 -L/usr/local/lib -lSDL3 -Wl,-rpath,/usr/local/lib

clear:
	rm -r bin/*
