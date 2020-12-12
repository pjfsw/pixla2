#include "rack.h"
#include "song.h"

Rack *rack_create() {
    Rack *rack = calloc(1, sizeof(Rack));
    rack->audio_library = audio_library_create("");
    if (rack->audio_library == NULL) {
        rack_destroy(rack);
        return NULL;
    }
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        rack->instruments[i].synth = synth_create();
        rack->instruments[i].type = i == 7 ? INSTR_SAMPLER : INSTR_SYNTH;
        rack->instruments[i].sampler = sampler_create(rack->audio_library);
    }

    rack->mixer = mixer_create(rack->instruments, NUMBER_OF_INSTRUMENTS, TRACKS_PER_PATTERN);
    if (rack->mixer == NULL) {
        rack_destroy(rack);
        return NULL;
    }

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
