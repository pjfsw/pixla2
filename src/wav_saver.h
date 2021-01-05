#ifndef WAV_SAVER_H_
#define WAV_SAVER_H_

#include <SDL2/SDL.h>

typedef struct _WavSaver WavSaver;

WavSaver *wav_saver_init(char *filename, Uint32 sampleRate, Uint16 number_of_channels);

void wav_saver_consume(WavSaver *wavSaver, Sint16 *samples, int number_of_samples);

void wav_saver_close(WavSaver *wavSaver);

#endif /* WAV_SAVER_H_ */
