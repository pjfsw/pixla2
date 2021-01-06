#include "rack.h"
#include "song.h"

Rack *rack_create(MixerTriggerFunc trigger_func, void *trigger_func_user_data,
    SynthSettings *synth_settings, MixerSettings *mixer_settings,
    Uint32 *song_bpm) {
    Rack *rack = calloc(1, sizeof(Rack));
    rack->audio_library = audio_library_create("/Users/johanfr/Music/samples");
    if (rack->audio_library == NULL) {
        rack_destroy(rack);
        return NULL;
    }
    rack->mixer = mixer_create(
        mixer_settings,
        trigger_func, trigger_func_user_data, true);
    if (rack->mixer == NULL) {
        rack_destroy(rack);
        return NULL;
    }
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        rack->instruments[i].synth = synth_create(&synth_settings[i], song_bpm, rack->mixer->sample_rate);
        rack->instruments[i].type = i == 7 ? INSTR_SAMPLER : INSTR_SYNTH;
        rack->instruments[i].sampler = sampler_create(rack->audio_library);
    }
    mixer_add_instruments(rack->mixer, rack->instruments, NUMBER_OF_INSTRUMENTS);

    return rack;
}

void rack_destroy(Rack *rack) {
    if (rack == NULL) {
        return;
    }
    if (rack->mixer != NULL) {
        mixer_stop(rack->mixer);
        mixer_destroy(rack->mixer);
    }
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        if (rack->instruments[i].synth != NULL) {
            synth_destroy(rack->instruments[i].synth);
        }
        if (rack->instruments[i].sampler != NULL) {
            sampler_destroy(rack->instruments[i].sampler);
        }
    }
    if (rack->audio_library != NULL) {
        audio_library_destroy(rack->audio_library);
    }
}

void rack_all_off(Rack *rack) {
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        instrument_off(&rack->instruments[i]);
    }
}
