#ifndef SRC_BUFFERED_FILE_H_
#define SRC_BUFFERED_FILE_H_

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define BUFFERED_FILE_BUFFER 1024

typedef struct {
    bool write;
    FILE *f;
    Uint32 buffer[BUFFERED_FILE_BUFFER];
    int pos;
    int size;
} BufferedFile;

BufferedFile *buffered_file_open(char *name, bool write);

bool buffered_file_write(BufferedFile *file, Uint32 value);

bool buffered_file_read(BufferedFile *file, Uint32 *value);

bool buffered_file_close(BufferedFile *file);


#endif /* SRC_BUFFERED_FILE_H_ */
