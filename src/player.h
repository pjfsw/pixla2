#ifndef SRC_PLAYER_H_
#define SRC_PLAYER_H_

#include "song.h"
#include "synth.h"

typedef struct {
    Song *song;
    Synth *synth;
    int last_note;
    int song_pos;
    int substep;
    Uint32 tempo;
    SDL_TimerID timer;
} Player;

void player_start(Player *player);

Uint32 player_callback(Uint32 interval, void *param);

void player_stop(Player *player);


#endif /* SRC_PLAYER_H_ */
