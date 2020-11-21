#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "oscillator.h"
#include "synth.h"
#include "vca.h"
#include "echo.h"
#include "filter.h"

Synth *synth_create() {
    Synth *synth = calloc(1, sizeof(Synth));
    synth->number_of_processors = 4;
    synth->processors = calloc(synth->number_of_processors, sizeof(Processor));
    synth->notes = calloc(synth->number_of_processors, sizeof(int));
    for (int i = 0; i < synth->number_of_processors; i++) {
        synth->processors[i].number_of_stages = 3;
        synth->processors[i].stages = calloc(synth->processors[i].number_of_stages, sizeof(ProcessorStage));

        int stage = 0;
        processor_set_stage(&synth->processors[i].stages[stage++],
            calloc(1, sizeof(Oscillator)), oscillator_transform, oscillator_trigger, NULL);

        processor_set_stage(&synth->processors[i].stages[stage++],
            filter_create(0.7, 0.82), filter_transform, filter_trigger, NULL);

        processor_set_stage(&synth->processors[i].stages[stage++],
            calloc(1, sizeof(Vca)), vca_transform, vca_trigger, vca_off);
    }
    synth->number_of_post_stages = 1;
    synth->post_stages = calloc(synth->number_of_post_stages, sizeof(ProcessorStage));
    synth->post_stages[0].offFunc = NULL;
    synth->post_stages[0].transformFunc = echo_transform;
    synth->post_stages[0].triggerFunc = NULL;
    synth->post_stages[0].userData = calloc(1, sizeof(Echo));

    return synth;
}


void synth_destroy(Synth *synth) {
    if (synth != NULL) {
        for (int i = 0; i < synth->number_of_post_stages; i++) {
            free(synth->post_stages[i].userData);
        }
        free(synth->post_stages);
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
    amplitude =  amplitude/(double)synth->number_of_processors;
    for (int i = 0; i < synth->number_of_post_stages; i++) {
        ProcessorStage *stage = &synth->post_stages[i];
        if (stage->transformFunc != NULL) {
            amplitude = stage->transformFunc(stage->userData, amplitude, delta_time);
        }
    }
    if (fabs(amplitude) > 1.0) {
        printf("Overflow %0.2f\n", amplitude);
    }
    return amplitude;
}
