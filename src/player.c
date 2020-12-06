#include <SDL2/SDL.h>
#include "player.h"

Uint32 player_callback(Uint32 interval, void *param) {
    Player *instance = (Player*)param;
    Uint32 t1 = SDL_GetTicks();
    if (instance->substep == 0) {
        Uint8 note = instance->song->patterns[0].track[0].note[instance->song_pos].pitch;
        if (note > 0) {
            synth_note_on(instance->synth, note);
            instance->last_note = note;
        }
        instance->song_pos = (instance->song_pos + 1) % NOTES_PER_TRACK;
    } else if (instance->last_note > 0) {
        synth_note_off(instance->synth, instance->last_note);
        instance->last_note = 0;
    }
    instance->substep = (instance->substep  + 1) % 8;
    return instance->tempo - SDL_GetTicks() + t1;
}

void player_start(Player *player) {
    player->timer = SDL_AddTimer(player->tempo, player_callback, player);

}

void player_stop(Player *player) {
    SDL_RemoveTimer(player->timer);
}
