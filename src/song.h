#ifndef SRC_SONG_H_
#define SRC_SONG_H_

#define NOTE_OFF 1
#define NO_NOTE 0

#include <SDL2/SDL.h>

typedef struct {
    Uint8 pitch;
    Uint8 velocity;
    Uint8 instrument;
} Note;

#define NOTES_PER_TRACK 64

typedef struct {
    Note note[NOTES_PER_TRACK];
} Track;

#define TRACKS_PER_PATTERN 6

typedef struct {
    Track track[TRACKS_PER_PATTERN];
} Pattern;

#define PATTERNS_PER_SONG 256

typedef struct {
    Pattern patterns[PATTERNS_PER_SONG];
} Song;

#endif /* SRC_SONG_H_ */
