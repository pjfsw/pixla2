#include <stdlib.h>
#include <SDL2/SDL.h>

#include "wav_saver.h"

#define WAV_BYTES_PER_SAMPLE 2

typedef struct _WavSaver {
    char *rawName;
    char *wavName;
    FILE *rawFile;
    Uint32 dataLength;
    Uint32 sampleRate;
    Uint16 number_of_channels;
} WavSaver;

typedef struct {
    char id[4];
    Uint32 size;
} WavChunkHeader;

typedef struct {
    WavChunkHeader header;
    Uint16 audioFormat;
    Uint16 numChannels;
    Uint32 sampleRate;
    Uint32 byteRate;
    Uint16 blockAlign;
    Uint16 bitsPerSample;
} FmtChunk;

typedef struct {
    WavChunkHeader header;
    char format[4];
    FmtChunk fmt;
    WavChunkHeader data;
} WavHeader;


void _wav_saver_write_header(WavSaver *wavSaver, FILE *file) {
    WavHeader wavHeader;

    memset(&wavHeader, 0, sizeof(WavHeader));

    memcpy(wavHeader.header.id, "RIFF", 4);
    wavHeader.header.size = wavSaver->dataLength + sizeof(WavHeader) - 8;
    memcpy(wavHeader.format, "WAVE", 4);

    /* fmt subchunk */
    memcpy(wavHeader.fmt.header.id, "fmt ", 4);
    wavHeader.fmt.header.size = 16; /* 16 for PCM */
    wavHeader.fmt.audioFormat = 1; /* Uncompressed PCM */

    wavHeader.fmt.numChannels = wavSaver->number_of_channels;
    wavHeader.fmt.sampleRate = wavSaver->sampleRate;
    wavHeader.fmt.byteRate = wavSaver->sampleRate * wavSaver->number_of_channels * WAV_BYTES_PER_SAMPLE;
    wavHeader.fmt.blockAlign = wavSaver->number_of_channels * WAV_BYTES_PER_SAMPLE;
    wavHeader.fmt.bitsPerSample = WAV_BYTES_PER_SAMPLE * 8;
    memcpy(wavHeader.data.id, "data", 4);
    wavHeader.data.size = wavSaver->dataLength;

    fwrite(&wavHeader, 1, sizeof(WavHeader), file);
}

WavSaver *wav_saver_init(char *fileName, Uint32 sampleRate, Uint16 number_of_channels) {
    WavSaver *wavSaver = calloc(1, sizeof(WavSaver));
    wavSaver->sampleRate = sampleRate;
    wavSaver->number_of_channels = number_of_channels;

    wavSaver->wavName = calloc(strlen(fileName)+1, sizeof(char));
    strcpy(wavSaver->wavName, fileName);

    wavSaver->rawName = calloc(strlen(fileName)+3, sizeof(char));
    strcpy(wavSaver->rawName, fileName);
    strcat(wavSaver->rawName, ".r");

    wavSaver->rawFile = fopen(wavSaver->rawName, "wb");
    if (wavSaver->rawFile == NULL) {
        wav_saver_close(wavSaver);
        return NULL;
    }

    return wavSaver;

}

void wav_saver_consume(WavSaver *wavSaver, Sint16 *samples, int number_of_samples) {
    wavSaver->dataLength += number_of_samples * sizeof(Sint16);
    fwrite(samples, sizeof(Sint16), number_of_samples, wavSaver->rawFile);
}

void _wav_saver_free(void **ptr) {
    if (*ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

void _wav_saver_close_file(FILE **file) {
    if (*file != NULL) {
        fclose(*file);
        *file = NULL;
    }
}

void _wav_saver_copy_file(FILE *target, FILE *src) {
    int bufSize = 16384;
    char buf[bufSize];

    while (!feof(src)) {
        size_t bytesRead = fread(buf, 1, bufSize, src);
        if (bytesRead > 0) {
            fwrite(buf, 1, bytesRead, target);
        }
    }

}

void wav_saver_close(WavSaver *wavSaver) {
    if (wavSaver != NULL) {
        _wav_saver_close_file(&wavSaver->rawFile);
        FILE *wavFile = fopen(wavSaver->wavName, "wb");
        FILE *rawFile = fopen(wavSaver->rawName, "rb");
        if (wavFile != NULL && rawFile != NULL) {
            _wav_saver_write_header(wavSaver, wavFile);
            _wav_saver_copy_file(wavFile, rawFile);
        }
        remove(wavSaver->rawName);
        _wav_saver_close_file(&wavFile);
        _wav_saver_close_file(&rawFile);
        _wav_saver_free((void**)&wavSaver->rawName);
        _wav_saver_free((void**)&wavSaver->wavName);
        free(wavSaver);
    }
}


