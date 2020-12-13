#ifndef SRC_PLAYER_H_
#define SRC_PLAYER_H_

#include "song.h"
#include "rack.h"

typedef struct {
    Song *song;
    Rack *rack;
    int last_note[TRACKS_PER_PATTERN];
    int last_instrument[TRACKS_PER_PATTERN];
    Uint8 pattern_pos;
    int substep;
    Uint32 tempo;
    SDL_TimerID timer;
} Player;

void player_start(Player *player);

Uint32 player_callback(Uint32 interval, void *param);

void player_stop(Player *player);


#endif /* SRC_PLAYER_H_ */
