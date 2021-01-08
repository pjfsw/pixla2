#include <SDL2/SDL.h>
#include "player.h"

#define _PLAYER_PORTAMENTO_SCALE 1440.0 // 0.25 halfnotes per number, 360 = 1 halfnote per number
#define _PLAYER_SUBSTEP_COUNT 30

bool _player_is_valid_pitch(Note *note) {
    return note->pitch > 11;
}

int player_trigger(void *user_data) {
    Player *instance = (Player*)user_data;
    if (instance->playing && !instance->request_play) {
        for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
            PlayerTrack *player_track = &instance->track_data[i];

            if (player_track->last_note > 0) {
                instrument_off(&instance->instruments[player_track->last_instrument]);
            }
        }
        instance->playing = false;
        return instance->tempo;
    } else if (!instance->playing && !instance->request_play) {
        return instance->tempo;
    } else if (!instance->playing && instance->request_play) {
        instance->substep = 0;
        instance->song_ended = false;
        instance->playing = true;
    }
    for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
        PlayerTrack *player_track = &instance->track_data[i];
        Note *note = &instance->song->patterns[instance->song->arrangement[instance->song_pos].pattern].
            track[i].
            note[instance->pattern_pos];
        if (instance->substep == 0) {
            if (player_track->last_note > 0 && (note->pitch == NOTE_OFF || _player_is_valid_pitch(note))) {
                Instrument *last_instrument = &instance->instruments[player_track->last_instrument];
                instrument_note_off(
                    last_instrument,
                    player_track->voice_id
                );
                player_track->last_note = 0;
            }
            if (_player_is_valid_pitch(note)) {
                player_track->voice_id = instrument_note_on(
                    &instance->instruments[note->instrument],
                    note->pitch,
                    note->velocity
                );
                player_track->last_note = note->pitch;
                player_track->last_instrument = note->instrument;
                player_track->pitch_offset = 0;
                player_track->last_portamento = 0;
                player_track->arpeggio = 0;
            }
            if (note->has_command && note->command == COMMAND_TEMPO) {
                if (note->parameter_value > 0) {
                    instance->tempo = note->parameter_value;
                }
            } else if (note->has_command && note->command == COMMAND_FAST_TEMPO) {
                instance->tempo = note->parameter_value + 256;
            } else if (note->has_command && note->command == COMMAND_INSTR_VOLUME) {
                Instrument *last_instrument = &instance->instruments[player_track->last_instrument];
                instrument_set_volume(last_instrument, note->parameter_value);
            } else if (note->has_command && note->command == COMMAND_ARPEGGIO) {
                player_track->arpeggio = note->parameter_value;

            }
        }
        Instrument *instrument = &instance->instruments[player_track->last_instrument];
        if (note->has_command && note->command == COMMAND_PORTAMENTO_UP) {
            if (note->parameter_value > 0) {
                player_track->last_portamento = note->parameter_value / _PLAYER_PORTAMENTO_SCALE;
            }
            player_track->pitch_offset += player_track->last_portamento;
            instrument_pitch_offset(instrument, player_track->voice_id, player_track->pitch_offset);
        } else if (note->has_command && note->command == COMMAND_PORTAMENTO_DOWN) {
            if (note->parameter_value > 0) {
                player_track->last_portamento = -note->parameter_value / _PLAYER_PORTAMENTO_SCALE;
            }
            player_track->pitch_offset += player_track->last_portamento;
            instrument_pitch_offset(instrument, player_track->voice_id, player_track->pitch_offset);
        } else if (player_track->arpeggio > 0) {
            if (instance->substep < 10) {
                player_track->pitch_offset = 0;
            } else if (instance->substep < 20) {
                player_track->pitch_offset = (player_track->arpeggio >> 4) / 12.0;
            } else {
                player_track->pitch_offset = (player_track->arpeggio & 15) / 12.0;
            }
            instrument_pitch_offset(instrument, player_track->voice_id, player_track->pitch_offset);
        }
    }
    instance->substep = (instance->substep + 1) % _PLAYER_SUBSTEP_COUNT;
    if (instance->substep == 0) {
        instance->pattern_pos = (instance->pattern_pos + 1) % NOTES_PER_TRACK;
        if (instance->pattern_pos == 0) {
            if (instance->song_pos == instance->song->length-1) {
                instance->song_ended = true;
            }
            instance->song_pos = (instance->song_pos +1) % instance->song->length;
        }
    }
    return instance->tempo;
}

void player_init(Player *player, Instrument *instruments, Song *song) {
    memset(player, 0, sizeof(Player));
    player->song = song;
    player->instruments = instruments;
    player->tempo = 120;
}

void player_start(Player *player) {
    player->request_play = true;

}

void player_stop(Player *player) {
    player->request_play = false;
}

