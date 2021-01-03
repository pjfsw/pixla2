#ifndef SRC_PLAYER_H_
#define SRC_PLAYER_H_

#include "song.h"
#include "rack.h"

typedef struct {
    int last_note;
    int last_instrument;
    int voice_id;
} PlayerTrack;

typedef struct {
    Song *song;
    Rack *rack;
    PlayerTrack track_data[TRACKS_PER_PATTERN];
    Uint8 pattern_pos;
    int song_pos;
    Uint32 tempo;
    bool playing;
    bool request_play;
} Player;

void player_start(Player *player);

int player_trigger(void *user_data);

void player_stop(Player *player);


#endif /* SRC_PLAYER_H_ */
