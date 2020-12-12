#include "rack.h"
#include "song.h"

Rack *rack_create() {
    Rack *rack = calloc(1, sizeof(Rack));
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        rack->instruments[i].synth = synth_create();
        rack->instruments[i].type = INSTR_SYNTH;
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
    }

}
