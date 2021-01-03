#include <SDL2/SDL.h>
#include "player.h"

int player_trigger(void *user_data) {
    Player *instance = (Player*)user_data;
    if (instance->playing && !instance->request_play) {
        for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
            PlayerTrack *player_track = &instance->track_data[i];

            if (player_track->last_note > 0) {
                instrument_off(&instance->rack->instruments[player_track->last_instrument]);
            }
        }
        instance->playing = false;
        return instance->tempo;
    } else if (!instance->playing && !instance->request_play) {
        return instance->tempo;
    }
    instance->playing = true;
    for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
        PlayerTrack *player_track = &instance->track_data[i];
        Note *note = &instance->song->patterns[instance->song->arrangement[instance->song_pos].pattern].
            track[i].
            note[instance->pattern_pos];
        if (player_track->last_note > 0 && (note->pitch == NOTE_OFF || note->pitch > 11)) {
            instrument_note_off(
                &instance->rack->instruments[player_track->last_instrument],
                player_track->voice_id
            );
            player_track->last_note = 0;
            player_track->last_instrument = 0;
        }
        if (note->pitch > 11) {
            player_track->voice_id = instrument_note_on(
                &instance->rack->instruments[note->instrument],
                note->pitch,
                note->velocity
            );
            player_track->last_note = note->pitch;
            player_track->last_instrument = note->instrument;
        }
    }
    instance->pattern_pos = (instance->pattern_pos + 1) % NOTES_PER_TRACK;
    if (instance->pattern_pos == 0) {
        instance->song_pos = (instance->song_pos +1) % instance->song->length;
    }
    return instance->tempo;
}

void player_start(Player *player) {
    player->request_play = true;

}

void player_stop(Player *player) {
    player->request_play = false;
}
