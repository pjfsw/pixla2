#include <stdio.h>
#include <stdbool.h>

#include "song_storage.h"
#include "rack.h"
#include "settings_func.h"
#include "parameter_functions.h"
#include "selection_functions.h"
#include "buffered_file.h"

#define _STORAGE_TYPE_MASK 0x80000000
#define _STORAGE_METADATA_MASK 0x80000000
#define _STORAGE_IS_METADATA(x) ((x & _STORAGE_METADATA_MASK) == _STORAGE_METADATA_MASK)
#define _STORAGE_METADATA_TYPE_MASK(x) ((x & 0x7F000000) >> 24)
#define _STORAGE_SYNTH_DATA_ID 0
// Sample id = 1
#define _STORAGE_MIXER_DATA_ID 2
#define _STORAGE_PATTERN_ID 3
#define _STORAGE_ARRANGEMENT_ID 4
#define _STORAGE_TRACK_ID 5
#define _STORAGE_ENCODE_ID(x) (x << 24)
#define _STORAGE_GET_ID(x) (_STORAGE_METADATA_TYPE_MASK(x))
#define STORAGE_DATA_MASK 0x7FFFFFFF
#define _STORAGE_PXL2_HEADER 0x324C5850


typedef struct {
    UiParameterFunc parameter;
    UiSelectionFunc selection;
} StorageSetting;

StorageSetting _song_storage_synth_settings[256];
StorageSetting _song_storage_mixer_settings[256];


void _song_storage_synth_init() {
    memset(_song_storage_synth_settings, 0, 256*sizeof(StorageSetting));
    // Oscillator settings
    _song_storage_synth_settings[0].selection = sf_synth_oscillator1_waveform;
    _song_storage_synth_settings[1].selection = sf_synth_oscillator1_transpose;
    _song_storage_synth_settings[2].selection = sf_synth_oscillator1_phase_mode;
    _song_storage_synth_settings[3].parameter = pf_synth_oscillator1_phase;

    _song_storage_synth_settings[8].selection = sf_synth_oscillator2_waveform;
    _song_storage_synth_settings[9].selection = sf_synth_oscillator2_transpose;
    _song_storage_synth_settings[10].selection = sf_synth_oscillator2_phase_mode;
    _song_storage_synth_settings[11].parameter = pf_synth_oscillator2_phase;

    // Voice VCA
    _song_storage_synth_settings[16].parameter = pf_synth_voice_attack;
    _song_storage_synth_settings[17].parameter = pf_synth_voice_decay;
    _song_storage_synth_settings[18].parameter = pf_synth_voice_sustain;
    _song_storage_synth_settings[19].parameter = pf_synth_voice_release;

    // Combinator settings
    _song_storage_synth_settings[24].parameter = pf_synth_comb_detune;
    _song_storage_synth_settings[25].parameter = pf_synth_comb_strength;
    _song_storage_synth_settings[26].selection = sf_synth_combiner_mode;
    _song_storage_synth_settings[27].parameter = pf_synth_comb_ring_amount;
    _song_storage_synth_settings[28].parameter = pf_synth_comb_ring_freq;

    // Filter settings
    _song_storage_synth_settings[32].parameter = pf_synth_filter_attack;
    _song_storage_synth_settings[33].parameter = pf_synth_filter_decay;
    _song_storage_synth_settings[34].parameter = pf_synth_filter_sustain;
    _song_storage_synth_settings[35].parameter = pf_synth_filter_release;
    _song_storage_synth_settings[36].parameter = pf_synth_filter_f;
    _song_storage_synth_settings[37].parameter = pf_synth_filter_q;
    _song_storage_synth_settings[38].selection = sf_synth_filter_vca_inverse;

    // Modulation settings
    _song_storage_synth_settings[64].parameter = pf_synth_lfo1_amount;
    _song_storage_synth_settings[65].parameter = pf_synth_lfo1_delay;
    _song_storage_synth_settings[66].parameter = pf_synth_lfo1_frequency;
    _song_storage_synth_settings[67].selection = sf_synth_lfo1_waveform;
    _song_storage_synth_settings[68].selection = sf_synth_lfo1_target;
    _song_storage_synth_settings[69].parameter = pf_synth_lfo2_amount;
    _song_storage_synth_settings[70].parameter = pf_synth_lfo2_delay;
    _song_storage_synth_settings[71].parameter = pf_synth_lfo2_frequency;
    _song_storage_synth_settings[72].selection = sf_synth_lfo2_waveform;
    _song_storage_synth_settings[73].selection = sf_synth_lfo2_target;
    _song_storage_synth_settings[74].parameter = pf_synth_mod_attack;
    _song_storage_synth_settings[75].parameter = pf_synth_mod_decay;
    _song_storage_synth_settings[76].parameter = pf_synth_mod_sustain;
    _song_storage_synth_settings[77].parameter = pf_synth_mod_release;
    _song_storage_synth_settings[78].selection = sf_synth_mod_vca_inverse;
    _song_storage_synth_settings[79].selection = sf_synth_mod_vca_target;
    _song_storage_synth_settings[80].parameter = pf_synth_mod_vca_strength;

    // Misc settings
    _song_storage_synth_settings[128].parameter = pf_synth_echo_delay;
    _song_storage_synth_settings[129].parameter = pf_synth_echo_wetness;
    _song_storage_synth_settings[130].parameter = pf_synth_echo_feedback;
    _song_storage_synth_settings[131].selection = sf_synth_echo;


    // Master level
    _song_storage_synth_settings[255].parameter = pf_synth_master_level;
}

void _song_storage_mixer_init() {
    memset(_song_storage_mixer_settings, 0, 256*sizeof(StorageSetting));
    _song_storage_mixer_settings[0].parameter = pf_mixer_level_instr1;
    _song_storage_mixer_settings[1].parameter = pf_mixer_level_instr2;
    _song_storage_mixer_settings[2].parameter = pf_mixer_level_instr3;
    _song_storage_mixer_settings[3].parameter = pf_mixer_level_instr4;
    _song_storage_mixer_settings[4].parameter = pf_mixer_level_instr5;
    _song_storage_mixer_settings[5].parameter = pf_mixer_level_instr6;
    _song_storage_mixer_settings[6].parameter = pf_mixer_level_instr7;
    _song_storage_mixer_settings[7].parameter = pf_mixer_level_instr8;

    _song_storage_mixer_settings[255].parameter = pf_mixer_master_level;


}

void song_storage_init() {
    _song_storage_synth_init();
    _song_storage_mixer_init();
}


void _song_storage_load_pattern_data(Song *song, int pattern, int track, Uint32 value) {
    int pitch = (value >> 24);
    int row = (value >> 16) & 0xFF;
    int velocity = (value >> 8) & 0xFF;
    int patch = value & 0xFF;

    song->patterns[pattern].track[track].note[row].instrument = patch;
    song->patterns[pattern].track[track].note[row].velocity = velocity;
    song->patterns[pattern].track[track].note[row].pitch = pitch;
}

void _song_storage_decode_data(void *settings, Uint8 parameter, Uint8 value, StorageSetting *parameter_table) {
    StorageSetting *setting = &parameter_table[parameter];
    if (setting->parameter != NULL) {
        *setting->parameter(settings) = value;
    } else if (setting->selection != NULL) {
        *setting->selection(settings) = value;
    } else {
        fprintf(stderr, "Unrecognized parameter %d\n", parameter);
    }
}

void _song_storage_decode_mixer_data(MixerSettings *mixer_settings, Uint8 parameter, Uint8 value) {
    _song_storage_decode_data(mixer_settings, parameter, value, _song_storage_mixer_settings);
}
void _song_storage_decode_synth_data(SynthSettings *synth_settings, Uint8 parameter, Uint8 value) {
    _song_storage_decode_data(synth_settings, parameter, value, _song_storage_synth_settings);
}

bool _song_storage_is_mixer_parameter(Uint8 parameter) {
    StorageSetting *setting = &_song_storage_mixer_settings[parameter];
    return setting->parameter != NULL || setting->selection != NULL;
}

bool _song_storage_is_synth_parameter(Uint8 parameter) {
    StorageSetting *setting = &_song_storage_synth_settings[parameter];
    return setting->parameter != NULL || setting->selection != NULL;
}

Uint16 _song_storage_encode_data(void *settings, Uint8 parameter, StorageSetting *parameter_table) {
    StorageSetting *setting = &parameter_table[parameter];
    Uint16 value = parameter << 8;
    if (setting->parameter != NULL) {
        value |= *setting->parameter(settings);
    } else if (setting->selection != NULL) {
        value |= *setting->selection(settings);
    }
    return value;
}

Uint16 _song_storage_encode_mixer_data(MixerSettings *mixer_settings, Uint8 parameter) {
    return _song_storage_encode_data(mixer_settings, parameter, _song_storage_mixer_settings);

}
Uint16 _song_storage_encode_synth_data(SynthSettings *synth_settings, Uint8 parameter) {
    return _song_storage_encode_data(synth_settings, parameter, _song_storage_synth_settings);
}

Uint16 _song_storage_get_param16(Uint32 data) {
    return data & 0xFFFF;
}

Uint8 _song_storage_get_parameter(Uint32 data) {
    return (data >> 8) & 0xFF;
}

Uint8 _song_storage_get_pvalue(Uint32 data) {
    return data & 0xFF;
}

bool song_storage_load(char *name, Song *song) {
    BufferedFile *f = buffered_file_open(name, false);
    if (f == NULL) {
        return false;
    }
    Uint32 value;

    song->length = 0;
    int pattern = 0;
    int track = 0;
    bool valid_file = false;
    while (buffered_file_read(f, &value)) {
        if (!valid_file) {
            if (_STORAGE_PXL2_HEADER != value) {
                song->length = 1;
                song->arrangement[0].pattern = 0;
                fprintf(stderr, "Not a valid PX2 file\n");
                return false;
            }
            valid_file = true;
            continue;
        }
        if (_STORAGE_IS_METADATA(value)) {
            if (_STORAGE_GET_ID(value) == _STORAGE_SYNTH_DATA_ID) {
                Uint8 patch = (value >> 16) & 0x7F;
                Uint8 parameter = _song_storage_get_parameter(value);
                Uint8 pvalue = _song_storage_get_pvalue(value);
                _song_storage_decode_synth_data(
                    &song->synth_settings[patch],
                    parameter,
                    pvalue
                );
            } else if (_STORAGE_GET_ID(value) == _STORAGE_MIXER_DATA_ID) {
                Uint8 parameter = _song_storage_get_parameter(value);
                Uint8 pvalue = _song_storage_get_pvalue(value);
                _song_storage_decode_mixer_data(
                    &song->mixer_settings,
                    parameter,
                    pvalue
                );

            } else if (_STORAGE_GET_ID(value) == _STORAGE_PATTERN_ID) {
                pattern = _song_storage_get_param16(value);
            } else if (_STORAGE_GET_ID(value) == _STORAGE_ARRANGEMENT_ID) {
                Uint16 arr = _song_storage_get_param16(value);
                song->arrangement[song->length].pattern = arr;
                song->length++;
            } else if (_STORAGE_GET_ID(value) == _STORAGE_TRACK_ID) {
                track = _song_storage_get_pvalue(value);
            }
        } else {
            _song_storage_load_pattern_data(song, pattern, track, value & STORAGE_DATA_MASK);
        }
    }
    buffered_file_close(f);
    if (song->length == 0) {
        song->length = 1;
        song->arrangement[0].pattern = 0;
    }
    return true;
}

bool _song_storage_should_save_note(Note *note) {
    return note->pitch > 0;
}

Uint32 _song_create_metadata(Uint8 metadata_id, Uint8 high, Uint16 low) {
    return _STORAGE_METADATA_MASK | _STORAGE_ENCODE_ID(metadata_id) | (high << 16) | low;
}

bool _song_storage_should_save_track(Track *track) {
    bool should_save_track = false;
    for (int row = 0; row < NOTES_PER_TRACK; row++) {
        Note *note = &track->note[row];
        should_save_track |= _song_storage_should_save_note(note);
    }
    return should_save_track;
}


bool song_storage_save(char *name, Song *song) {
    BufferedFile *f = buffered_file_open(name, true);
    if (f  == NULL) {
        return false;
    }
    Uint64 header = _STORAGE_PXL2_HEADER;
    buffered_file_write(f, header);
    printf("Saving pattern data\n");
    for (int pattern = 0; pattern < PATTERNS_PER_SONG; pattern++) {
        bool should_save_pattern = false;
        for (int track = 0; track < TRACKS_PER_PATTERN; track++) {
            should_save_pattern |= _song_storage_should_save_track(&song->patterns[pattern].track[track]);
        }

        if (!should_save_pattern) {
            continue;
        }

        Uint32 encoded_pattern = _song_create_metadata(_STORAGE_PATTERN_ID, 0, pattern & 0xFFFF);
        buffered_file_write(f, encoded_pattern);

        for (int track = 0; track < TRACKS_PER_PATTERN; track++) {
            if (!_song_storage_should_save_track(&song->patterns[pattern].track[track])) {
                continue;
            }
            Uint32 encoded_track = _song_create_metadata(_STORAGE_TRACK_ID, 0, track & 0xFF);
            buffered_file_write(f, encoded_track);

            for (int row = 0; row < NOTES_PER_TRACK; row++) {
                Note *note = &song->patterns[pattern].track[track].note[row];
                // SONG DATA
                //   0ppppppp_rrrrrrrr_vvvvvvvv_ttttiiii   r = row, p = pitch, v = velocity, t = track, i = instrument
                if (_song_storage_should_save_note(note)) {
                    Uint32 value =
                        note->instrument | (note->velocity << 8) | (row << 16) | (note->pitch << 24);
                    buffered_file_write(f, value);
                }
            }
        }
    }
    printf("Saving arrangement\n");
    for (int i = 0; i < song->length; i++) {
        Uint32 pattern = _song_create_metadata(_STORAGE_ARRANGEMENT_ID, 0, song->arrangement[i].pattern & 0xFFFF);
        buffered_file_write(f, pattern);
    }

    printf("Saving synth data\n");
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        for (int param = 0; param < 256; param++) {
            if (!_song_storage_is_synth_parameter(param)) {
                continue;
            }
            Uint32 value = _song_create_metadata(_STORAGE_SYNTH_DATA_ID, i,
                _song_storage_encode_synth_data(&song->synth_settings[i], param));
            buffered_file_write(f, value);
        }
    }
    printf("Saving mixer data\n");
    for (int param = 0; param < 256; param++) {
        if (!_song_storage_is_mixer_parameter(param)) {
            continue;
        }
        Uint32 value = _song_create_metadata(_STORAGE_MIXER_DATA_ID, 0,
            _song_storage_encode_mixer_data(&song->mixer_settings, param));
        buffered_file_write(f, value);
    }
    return buffered_file_close(f);
}
