CC	:= gcc
CCFLAGS:=$(shell sdl2-config --cflags) -Wall -O2 -Werror -pedantic
LDFLAGS := $(shell sdl2-config --libs) -lSDL_ttf
#STATIC := $(shell sdl2-config --static-libs)
TARGET := Debug

all:
	mkdir -p $(TARGET)
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $(TARGET)/pixla2 src/*.c 

clean:
	rm $(TARGET)/*

