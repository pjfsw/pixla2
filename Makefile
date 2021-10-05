CC	:= gcc
CCFLAGS:=$(shell sdl2-config --cflags) -Wall -O2 -Werror -pedantic
LDFLAGS := $(shell sdl2-config --libs) -lSDL2_ttf -lm
#STATIC := $(shell sdl2-config --static-libs)
TARGET := Debug

all:
	mkdir -p $(TARGET)
	$(CC) $(CCFLAGS) -o $(TARGET)/pixla2 src/*.c $(LDFLAGS)

clean:
	rm $(TARGET)/*

