#ifndef SRC_SONG_H_
#define SRC_SONG_H_

#define NOTE_OFF 1
#define NO_NOTE 0

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "mixer.h"
#include "synth.h"
#include "tracker_limits.h"

#define COMMAND_PORTAMENTO_UP 1
#define COMMAND_PORTAMENTO_DOWN 2
#define COMMAND_INSTR_VOLUME 0x0C
#define COMMAND_TEMPO 0x1D  // T
#define COMMAND_FAST_TEMPO 0xF

typedef struct {
    Uint8 pitch;
    Uint8 velocity;
    Uint8 instrument;
    bool has_command;
    Uint8 command;
    Uint8 parameter_value;
} Note;

#define NOTES_PER_TRACK 64

typedef struct {
    Note note[NOTES_PER_TRACK];
} Track;

#define TRACKS_PER_PATTERN 9

typedef struct {
    Track track[TRACKS_PER_PATTERN];
} Pattern;

typedef struct {
    int pattern;
    // TODO automation?
} Arrangement;

#define PATTERNS_PER_SONG 1024

typedef struct {
    Pattern patterns[PATTERNS_PER_SONG];
    Arrangement arrangement[PATTERNS_PER_SONG];
    int length;
    SynthSettings synth_settings[NUMBER_OF_INSTRUMENTS];
    MixerSettings mixer_settings;
} Song;

#endif /* SRC_SONG_H_ */
