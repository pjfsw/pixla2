#include <SDL2/SDL.h>
#include "player.h"

Uint32 player_callback(Uint32 interval, void *param) {
    Player *instance = (Player*)param;
    Uint32 t1 = SDL_GetTicks();
    if (instance->substep == 0) {
        for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
            Note *note = &instance->song->patterns[0].track[i].note[instance->pattern_pos];
            if (instance->last_note[i] > 0 && (note->pitch == NOTE_OFF || note->pitch > 11)) {
                synth_note_off(instance->synths[instance->last_instrument[i]], instance->last_note[i]);
                instance->last_note[i] = 0;
                instance->last_instrument[i] = 0;
            }
            if (note->pitch > 11) {
                synth_note_on(instance->synths[note->instrument], note->pitch);
                instance->last_note[i] = note->pitch;
                instance->last_instrument[i] = note->instrument;
            }
        }
        instance->pattern_pos = (instance->pattern_pos + 1) % NOTES_PER_TRACK;
    }
    instance->substep = (instance->substep  + 1) % 4;
    return instance->tempo - SDL_GetTicks() + t1;
}

void player_start(Player *player) {
    if (player->timer != 0) {
        SDL_RemoveTimer(player->timer);
    }
    player->timer = SDL_AddTimer(player->tempo, player_callback, player);

}

void player_stop(Player *player) {
    if (player->timer != 0) {
        SDL_RemoveTimer(player->timer);
        for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
            if (player->last_note[i] > 0) {
                synth_note_off(player->synths[player->last_instrument[i]], player->last_note[i]);
            }
        }
        player->timer = 0;
    }
}
