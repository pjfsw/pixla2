#ifndef SRC_SONG_STORAGE_H_
#define SRC_SONG_STORAGE_H_

#include <stdbool.h>
#include "song.h"

bool song_storage_load(char *name, Song *song);

bool song_storage_save(char *name, Song *song);

#endif /* SRC_SONG_STORAGE_H_ */
