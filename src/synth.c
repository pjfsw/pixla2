#include <stdlib.h>
#include <math.h>
#include "oscillator.h"
#include "synth.h"
#include "vca.h"

Synth *synth_create() {
    Synth *synth = calloc(1, sizeof(Synth));
    synth->number_of_processors = 4;
    synth->processors = calloc(synth->number_of_processors, sizeof(Processor));
    synth->notes = calloc(synth->number_of_processors, sizeof(int));
    for (int i = 0; i < synth->number_of_processors; i++) {
        synth->processors[i].number_of_stages = 2;
        synth->processors[i].stages = calloc(synth->processors[i].number_of_stages, sizeof(ProcessorStage));

        synth->processors[i].stages[0].userData = calloc(1, sizeof(Oscillator));
        synth->processors[i].stages[0].offFunc = NULL;
        synth->processors[i].stages[0].triggerFunc = oscillator_trigger;
        synth->processors[i].stages[0].transformFunc = oscillator_transform;

        synth->processors[i].stages[1].userData = calloc(1, sizeof(Vca));
        synth->processors[i].stages[1].offFunc = vca_off;
        synth->processors[i].stages[1].triggerFunc = vca_trigger;
        synth->processors[i].stages[1].transformFunc = vca_transform;
    }

    return synth;
}


void synth_destroy(Synth *synth) {
    if (synth != NULL) {
        for (int i = 0; i < synth->number_of_processors; i++) {
            for (int j = 0; j < synth->processors[i].number_of_stages; j++) {
                free(synth->processors[i].stages[j].userData);
            }
            free(synth->processors[i].stages);
        }
        free(synth->processors);
        free(synth->notes);
        free(synth);
    }
}

double _synth_note_to_frequency(int note) {
    return 55.0 * pow(2, note/(double)12);
}

void synth_note_on(Synth *synth, int note) {
    Processor *processor = &synth->processors[synth->nextProcessor];
    for (int i = 0; i < processor->number_of_stages; i++) {
        ProcessorStage *stage = &processor->stages[i];
        if (stage->triggerFunc != NULL) {
            stage->triggerFunc(stage->userData, _synth_note_to_frequency(note));
        }
    }
    synth->notes[synth->nextProcessor] = note;
    synth->nextProcessor = (synth->nextProcessor + 1) % synth->number_of_processors;
}

void synth_note_off(Synth *synth, int note) {
    for (int i = 0; i < synth->number_of_processors; i++) {
        if (synth->notes[i] == note) {
            synth->notes[i] = 0;
            Processor *processor = &synth->processors[i];
            for (int j = 0; j < processor->number_of_stages; j++) {
                ProcessorStage *stage = &processor->stages[j];
                if (stage->offFunc != NULL) {
                    stage->offFunc(stage->userData);
                }
            }
        }
    }
}

float synth_poll(Synth *synth, double delta_time) {
    double amplitude = 0;
    for (int i = 0; i < synth->number_of_processors; i++) {
        Processor *processor = &synth->processors[i];
        double processor_amp = 1;
        for (int j = 0; j < processor->number_of_stages; j++) {
            ProcessorStage *stage = &processor->stages[j];
            if (stage->transformFunc != NULL) {
                processor_amp = stage->transformFunc(stage->userData, processor_amp, delta_time);
            }
        }
        amplitude += processor_amp;
    }
    return amplitude/(double)synth->number_of_processors;
}
