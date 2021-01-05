#ifndef SRC_SONG_EXPORTER_H_
#define SRC_SONG_EXPORTER_H_

#include "mixer.h"
#include "song.h"
#include "audio_library.h"

void song_exporter_export(char *filename, Song *song, Mixer *source_mixer, AudioLibrary *audio_library);

#endif /* SRC_SONG_EXPORTER_H_ */
