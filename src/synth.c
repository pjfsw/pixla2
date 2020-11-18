#include "synth.h"
#include <stdlib.h>
#include <math.h>

Synth *synth_create() {
    Synth *synth = calloc(1, sizeof(Synth));
    synth->number_of_oscillators = sizeof(synth->oscillators)/sizeof(Oscillator);
    return synth;
}

void synth_destroy(Synth *synth) {
    if (synth != NULL) {
        for (int i = 0; i < sizeof(synth->oscillators)/sizeof(Oscillator); i++) {
            oscillator_reset(&synth->oscillators[i]);
        }
        free(synth);
    }
}

double _synth_note_to_frequency(int note) {
    return 55.0 * pow(2, note/(double)12);
}

void synth_note_on(Synth *synth, int note) {
    oscillator_trigger(&synth->oscillators[synth->nextOscillator], _synth_note_to_frequency(note));
    synth->notes[synth->nextOscillator] = note;
    synth->nextOscillator = (synth->nextOscillator + 1) % synth->number_of_oscillators;
}

void synth_note_off(Synth *synth, int note) {
    for (int i = 0; i < synth->number_of_oscillators; i++) {
        if (synth->notes[i] == note) {
            synth->notes[i] = 0;
            oscillator_off(&synth->oscillators[i]);
        }
    }
}

float synth_poll(Synth *synth) {
    double amplitude = 0;
    for (int i = 0; i < synth->number_of_oscillators; i++) {
        amplitude += oscillator_poll(&synth->oscillators[i]);
    }
    return amplitude/(double)synth->number_of_oscillators;
}
