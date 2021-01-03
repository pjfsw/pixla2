#include <SDL2/SDL.h>
#include "player.h"

bool _player_is_valid_pitch(Note *note) {
    return note->pitch > 11;
}

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
        if (player_track->last_note > 0 && (note->pitch == NOTE_OFF || _player_is_valid_pitch(note))) {
            Instrument *last_instrument = &instance->rack->instruments[player_track->last_instrument];
            instrument_note_off(
                last_instrument,
                player_track->voice_id
            );
            if ( _player_is_valid_pitch(note)) {
                instrument_portamento(last_instrument, player_track->voice_id, 0);
            }
            player_track->last_note = 0;
            player_track->last_instrument = 0;
        }
        if (_player_is_valid_pitch(note)) {
            player_track->voice_id = instrument_note_on(
                &instance->rack->instruments[note->instrument],
                note->pitch,
                note->velocity
            );
            player_track->last_note = note->pitch;
            player_track->last_instrument = note->instrument;
        }
        Instrument *instrument = &instance->rack->instruments[player_track->last_instrument];
        if (note->has_command && note->command == COMMAND_PORTAMENTO_UP) {
            if (note->parameter_value > 0) {
                instrument_portamento(instrument, player_track->voice_id, note->parameter_value);
            }
        } else if (note->has_command && note->command == COMMAND_PORTAMENTO_DOWN) {
            if (note->parameter_value > 0) {
                instrument_portamento(instrument, player_track->voice_id, -note->parameter_value);
            }
        } else {
            instrument_portamento(instrument, player_track->voice_id, 0);
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
