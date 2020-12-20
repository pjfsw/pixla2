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
#define _STORAGE_MIXER_DATA_ID 2
#define _STORAGE_ENCODE_ID(x) (x << 24)
#define _STORAGE_IS_MIXER_DATA(x) (_STORAGE_METADATA_TYPE_MASK(x) == _STORAGE_MIXER_DATA_ID)
#define STORAGE_DATA_MASK 0x7FFFFFFF


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

//
// Bit 31 = 1, meta data, Bit 31 = 0 song data
// META DATA
//   Bit 24-30 = 0000000: Instrument data (synth)
//               0000001: Instrument data (sampler)
//               0000010: Mixer data
//               0000011-1111111: TBD
//   Instrument data:
//   Bit 16-23 = Patch number
//   Bit 8-15 = Parameter no
//   Bit 0-7  = Parameter value
//
//   Mixer data:
//   Bit 16-23 = Unused (set to zero)
//   Bit 8-15  = Parameter no
//   Bit 0-7   = Parameter value

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

Uint8 _song_storage_get_parameter(Uint32 data) {
    return (data >> 8) & 0xFF;
}

Uint8 _song_storage_get_pvalue(Uint32 data) {
    return data & 0xFF;
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
                    Uint8 parameter = _song_storage_get_parameter(value);
                    Uint8 pvalue = _song_storage_get_pvalue(value);
                    _song_storage_decode_synth_data(
                        &song->synth_settings[patch],
                        parameter,
                        pvalue
                    );
                } else if (_STORAGE_IS_MIXER_DATA(value)) {
                    Uint8 parameter = _song_storage_get_parameter(value);
                    Uint8 pvalue = _song_storage_get_pvalue(value);
                    _song_storage_decode_mixer_data(
                        &song->mixer_settings,
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
            if (!_song_storage_is_synth_parameter(param)) {
                continue;
            }
            Uint32 value =  _STORAGE_METADATA_MASK |
                (i << 16) |
                _song_storage_encode_synth_data(&song->synth_settings[i], param);
            fprintf(f, "%08x\n", value);
        }
    }
    printf("Saving mixer data\n");
    for (int param = 0; param < 256; param++) {
        if (!_song_storage_is_mixer_parameter(param)) {
            continue;
        }
        Uint32 value = _STORAGE_METADATA_MASK | _STORAGE_ENCODE_ID(_STORAGE_MIXER_DATA_ID) |
            _song_storage_encode_mixer_data(&song->mixer_settings, param);
        fprintf(f, "%08x\n", value);
    }
    fclose(f);
    return true;
}
