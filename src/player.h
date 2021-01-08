#ifndef SRC_PLAYER_H_
#define SRC_PLAYER_H_

#include "song.h"

typedef struct {
    int last_note;
    int last_instrument;
    int voice_id;
    Uint8 arpeggio;
    double pitch_offset;
    double last_portamento;
} PlayerTrack;

typedef struct {
    Song *song;
    Instrument *instruments;
    PlayerTrack track_data[TRACKS_PER_PATTERN];
    Uint8 pattern_pos;
    int song_pos;
    Uint32 tempo;
    bool playing;
    bool request_play;
    int substep;
    bool song_ended;
} Player;

void player_init(Player *player, Instrument *instruments, Song *song);

void player_start(Player *player);

int player_trigger(void *user_data);

void player_stop(Player *player);

#endif /* SRC_PLAYER_H_ */
