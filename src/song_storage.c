#include <stdio.h>
#include <stdbool.h>

#include "song_storage.h"
#include "rack.h"
#include "settings_func.h"
#include "parameter_functions.h"
#include "selection_functions.h"

#define _STORAGE_TYPE_MASK 0x80000000
#define _STORAGE_METADATA_MASK 0x80000000
#define _STORAGE_IS_METADATA(x) ((x & _STORAGE_METADATA_MASK) == _STORAGE_METADATA_MASK)
#define _STORAGE_METADATA_TYPE_MASK(x) ((x & 0x7F000000) >> 24)
#define _STORAGE_IS_SYNTH_DATA(x) (_STORAGE_METADATA_TYPE_MASK(x) == 0)

#define STORAGE_DATA_MASK 0x7FFFFFFF


typedef struct {
    UiParameterFunc parameter;
    UiSelectionFunc selection;
} SynthSetting;

SynthSetting _song_storage_synth_settings[256];


void song_storage_init() {
    memset(_song_storage_synth_settings, 0, 256*sizeof(SynthSetting));
    // Voice and oscillator settings
    _song_storage_synth_settings[0].parameter = pf_synth_voice_attack;
    _song_storage_synth_settings[1].parameter = pf_synth_voice_decay;
    _song_storage_synth_settings[2].parameter = pf_synth_voice_sustain;
    _song_storage_synth_settings[3].parameter = pf_synth_voice_release;
    _song_storage_synth_settings[4].selection = sf_synth_oscillator1_waveform;
    _song_storage_synth_settings[5].selection = sf_synth_oscillator2_waveform;
    //  6 reserved
    _song_storage_synth_settings[7].selection = sf_synth_oscillator2_transpose;

    // Combinator settings
    _song_storage_synth_settings[16].parameter = pf_synth_comb_attack;
    _song_storage_synth_settings[17].parameter = pf_synth_comb_decay;
    _song_storage_synth_settings[18].parameter = pf_synth_comb_sustain;
    _song_storage_synth_settings[19].parameter = pf_synth_comb_release;
    _song_storage_synth_settings[20].parameter = pf_synth_comb_detune;
    _song_storage_synth_settings[21].parameter = pf_synth_comb_strength;
    _song_storage_synth_settings[22].selection = sf_synth_combiner_mode;
    _song_storage_synth_settings[23].selection = sf_synth_combiner_oscillator2_strength_mode;
    _song_storage_synth_settings[24].selection = sf_synth_combiner_vca_inverse;

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

    // Misc settings
    _song_storage_synth_settings[128].parameter = pf_synth_echo_delay;
    _song_storage_synth_settings[129].parameter = pf_synth_echo_wetness;
    _song_storage_synth_settings[130].parameter = pf_synth_echo_feedback;
    _song_storage_synth_settings[131].selection = sf_synth_echo;


    // Master level
    _song_storage_synth_settings[255].parameter = pf_synth_master_level;


}

//
// Bit 31 = 1, meta data, Bit 31 = 0 song data
// META DATA
//   Bit 24-30 = 0000000: Instrument data (synth)
//               0000001-1111111: TBD
//   Instrument data:
//   Bit 16-23 = Patch number
//   Bit 8-15 = Parameter no
//   Bit 0-7  = Parameter value

// SONG DATA
//   0ppppppp_rrrrrrrr_vvvvvvvv_ttttiiii   r = row, p = pitch, v = velocity, t = track, i = instrument

void _song_storage_load_pattern_data(Song *song, int pattern, Uint32 value) {
    int pitch = (value >> 24);
    int row = (value >> 16) & 0xFF;
    int velocity = (value >> 8) & 0xFF;
    int track = (value >> 4) & 0x0F;
    int patch = value & 0x0F;

    song->patterns[pattern].track[track].note[row].instrument = patch;
    song->patterns[pattern].track[track].note[row].velocity = velocity;
    song->patterns[pattern].track[track].note[row].pitch = pitch;
}

void _song_storage_decode_synth_data(SynthSettings *synth_settings, Uint8 parameter, Uint8 value) {
    SynthSetting *setting = &_song_storage_synth_settings[parameter];
    if (setting->parameter != NULL) {
        *setting->parameter(synth_settings) = value;
    } else if (setting->selection != NULL) {
        *setting->selection(synth_settings) = value;
    } else {
        fprintf(stderr, "Unrecognized synth parameter %d\n", parameter);
    }
}

bool _song_storage_is_parameter(Uint8 parameter) {
    SynthSetting *setting = &_song_storage_synth_settings[parameter];
    return setting->parameter != NULL || setting->selection != NULL;
}

Uint16 _song_storage_encode_synth_data(SynthSettings *synth_settings, Uint8 parameter) {
    SynthSetting *setting = &_song_storage_synth_settings[parameter];
    Uint16 value = parameter << 8;
    if (setting->parameter != NULL) {
        value |= *setting->parameter(synth_settings);
    } else if (setting->selection != NULL) {
        value |= *setting->selection(synth_settings);
    }
    return value;
}

bool song_storage_load(char *name, Song *song) {
    FILE *f = fopen(name, "r");
    if (f == NULL) {
        return false;
    }
    Uint32 value;

    int pattern = 0;
    while (!feof(f)) {
        if (1 == fscanf(f, "%x\n", &value)) {
            if (_STORAGE_IS_METADATA(value)) {
                if (_STORAGE_IS_SYNTH_DATA(value)) {
                    Uint8 patch = (value >> 16) & 0x7F;
                    Uint8 parameter = (value >> 8) & 0xFF;
                    Uint8 pvalue = value & 0xFF;
                    _song_storage_decode_synth_data(
                        &song->synth_settings[patch],
                        parameter,
                        pvalue
                    );
                }
            } else {
                _song_storage_load_pattern_data(song, pattern, value & STORAGE_DATA_MASK);
            }
        }
    }
    fclose(f);
    return true;
}

bool _song_storage_should_save_note(Note *note) {
    return note->pitch > 0;
}

bool song_storage_save(char *name, Song *song) {
    FILE *f = fopen(name, "w");
    if (f  == NULL) {
        return false;
    }
    int pattern = 0;
    printf("Saving track data\n");
    for (int track = 0; track < TRACKS_PER_PATTERN; track++) {
        for (int row = 0; row < NOTES_PER_TRACK; row++) {
            Note *note = &song->patterns[pattern].track[track].note[row];
        // SONG DATA
        //   0ppppppp_rrrrrrrr_vvvvvvvv_ttttiiii   r = row, p = pitch, v = velocity, t = track, i = instrument
            if (_song_storage_should_save_note(note)) {
                Uint32 value =
                    note->instrument | (track << 4) | (note->velocity << 8) | (row << 16) | (note->pitch << 24);
                fprintf(f, "%08x\n", value);
            }
        }
    }
    printf("Saving synth data\n");
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        for (int param = 0; param < 256; param++) {
            if (!_song_storage_is_parameter(param)) {
                continue;
            }
            Uint32 value =  _STORAGE_METADATA_MASK |
                (i << 16) |
                _song_storage_encode_synth_data(&song->synth_settings[i], param);
            fprintf(f, "%08x\n", value);
        }
    }
    fclose(f);
    return true;
}
