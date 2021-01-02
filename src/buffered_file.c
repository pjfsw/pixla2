#include "buffered_file.h"

BufferedFile *buffered_file_open(char *name, bool write) {
    FILE *f = fopen(name, write ? "wb" : "rb");
    if (f  == NULL) {
        fprintf(stderr, "Open %s failure: %s\n", write ? "write" : "read", name);
        return NULL;
    }

    BufferedFile *bf = calloc(1, sizeof(BufferedFile));
    bf->f = f;
    bf->write = write;
    return bf;

}

bool _buffered_file_write_to_disk(BufferedFile *file) {
    size_t elements_written = fwrite(file->buffer, sizeof(Uint32), file->pos, file->f);
    bool success = elements_written == file->pos;
    file->pos -= elements_written;
    return success;
}

bool buffered_file_write(BufferedFile *file, Uint32 value) {
    if (!file->write) {
        fprintf(stderr, "Attempting to write to file opened as read\n");
        return false;
    }
    file->buffer[file->pos] = value;
    file->pos++;
    if (file->pos >= BUFFERED_FILE_BUFFER) {
        return _buffered_file_write_to_disk(file);
    }
    return true;
}

bool buffered_file_read(BufferedFile *file, Uint32 *value) {
    if (file->write) {
        fprintf(stderr, "Attempting to read from file opened as write\n");
        return false;
    }
    if (file->pos == file->size) {
        size_t elements_read = fread(file->buffer, sizeof(Uint32), BUFFERED_FILE_BUFFER, file->f);
        if (elements_read == 0) {
            return false;
        }
        file->size = elements_read;
        file->pos = 0;
    }
    *value = file->buffer[file->pos];
    file->pos++;
    return true;
}

bool buffered_file_close(BufferedFile *file) {
    if (file == NULL) {
        return false;
    }
    bool status = true;
    if (file->pos > 0 && file->write) {
        status = _buffered_file_write_to_disk(file);
    }
    fclose(file->f);
    free(file);
    return status;
}

