#include <stdio.h>

#include "song_storage.h"
#include "rack.h"

#define STORAGE_META_DATA_MASK 0x80000000
#define STORAGE_SONG_MASK 0x7FFFFFFF

//
// Bit 31 = 1, meta data, Bit 31 = 0 song data
// META DATA
//   Bit 16-30 = Parameter no, Bit 0..15 Parameter value
//   TBD
// SONG DATA
//   0ppppppp_rrrrrrrr_vvvvvvvv_ttttiiii   r = row, p = pitch, v = velocity, t = track, i = instrument

bool song_storage_load(char *name, Song *song) {
    FILE *f = fopen(name, "r");
    if (f == NULL) {
        return false;
    }
    Uint32 value;

    int pattern = 0;
    while (!feof(f)) {
        if (1 == fscanf(f, "%x\n", &value)) {
            if (value & STORAGE_META_DATA_MASK) {
            } else {
                value = value & STORAGE_SONG_MASK;
                int pitch = (value >> 24);
                int row = (value >> 16) & 0xFF;
                int velocity = (value >> 8) & 0xFF;
                int track = (value >> 4) & 0x0F;
                int patch = value & 0x0F;

                song->patterns[pattern].track[track].note[row].instrument = patch;
                song->patterns[pattern].track[track].note[row].velocity = velocity;
                song->patterns[pattern].track[track].note[row].pitch = pitch;
            }
        }
    }
    fclose(f);
    return true;
}

bool _song_storage_should_save_note(Note *note) {
    return note->pitch > 0;
}

bool song_storage_save(char *name, Song *song) {
    FILE *f = fopen(name, "w");
    if (f  == NULL) {
        return false;
    }
    int pattern = 0;
    for (int track = 0; track < TRACKS_PER_PATTERN; track++) {
        for (int row = 0; row < NOTES_PER_TRACK; row++) {
            Note *note = &song->patterns[pattern].track[track].note[row];
        // SONG DATA
        //   0ppppppp_rrrrrrrr_vvvvvvvv_ttttiiii   r = row, p = pitch, v = velocity, t = track, i = instrument
            if (_song_storage_should_save_note(note)) {
                Uint32 value =
                    note->instrument | (track << 4) | (note->velocity << 8) | (row << 16) | (note->pitch << 24);
                fprintf(f, "%08x\n", value);
            }
        }

    }
    fclose(f);
    return true;
}
