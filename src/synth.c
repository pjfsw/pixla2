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
    synth->master_level = 1;
    synth->number_of_voices = 4;
    synth->voices = calloc(synth->number_of_voices, sizeof(Voice));

    synth->voice_vca_settings.attack = 0.0002;
    synth->voice_vca_settings.decay = 0.03;
    synth->voice_vca_settings.sustain = 0.6;
    synth->voice_vca_settings.release = 0.07;

    synth->combiner_settings.combine_mode = COMB_MULTIPLY;
    synth->combiner_settings.strength_mode = STRENGTH_VCA;
    synth->combiner_settings.oscillator2_strength = 0.7;
    synth->combiner_settings.oscillator2_scale = 0.5;

    synth->combiner_vca_settings.attack = 0.5;
    synth->combiner_vca_settings.decay = 0.2;
    synth->combiner_vca_settings.sustain = 0.6;
    synth->combiner_vca_settings.release = 2.0;
    synth->combiner_vca_settings.inverse = true;

    synth->filter_vca_settings.attack =0.0;
    synth->filter_vca_settings.decay = 0.4;
    synth->filter_vca_settings.sustain = 0.3;
    synth->filter_vca_settings.release = 0.7;

    for (int i = 0; i < synth->number_of_voices; i++) {
        Voice *voice = &synth->voices[i];
        Vca *adsr = &voice->voice_vca;
        adsr->settings = &synth->voice_vca_settings;

        modulation_init(&voice->modulation, &voice->oscillator, &voice->oscillator2);
        voice->modulation.lfo1.amount = 0.08;
        voice->modulation.lfo1.delay = 5;
        voice->modulation.lfo1.frequency = 5;
        voice->modulation.lfo1.oscillator.waveform = SINE;
        voice->modulation.modulation_target = MOD_OSCILLATOR_FM;

        Processor *processor = &voice->processor;
        processor->number_of_stages = 3;
        processor->stages = calloc(processor->number_of_stages, sizeof(ProcessorStage));

        int stage = 0;

        voice->combiner.vca.settings = &synth->combiner_vca_settings;
        voice->combiner.settings = &synth->combiner_settings;
        voice->combiner.oscillator1 = &voice->oscillator;
        voice->combiner.oscillator2 = &voice->oscillator2;
        oscillator_set_waveform(&voice->oscillator2, SQUARE);

        processor_set_stage(&processor->stages[stage++],
            &voice->combiner, combiner_transform, combiner_trigger, combiner_off);

        voice->filter.vca.settings = &synth->filter_vca_settings;
        filter_set(&voice->filter, 0.8, 0.85);
        processor_set_stage(&processor->stages[stage++],
            &voice->filter, filter_transform, filter_trigger, filter_off);

        processor_set_stage(&processor->stages[stage++],
            &voice->voice_vca, vca_transform, vca_trigger, vca_off);
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
        for (int i = 0; i < synth->number_of_voices; i++) {
            free(synth->voices[i].processor.stages);
        }
        free(synth->voices);
        free(synth);
    }
}

double _synth_note_to_frequency(int note) {
    return 27.5 * pow(2, note/(double)12);
}

void synth_note_on(Synth *synth, int note) {
    for (int i = 0; i < synth->number_of_voices; i++) {
        synth->next_voice = (synth->next_voice + 1) % synth->number_of_voices;
        Voice *voice = &synth->voices[synth->next_voice];
        if (voice->voice_vca.state == VCA_RELEASE || voice->voice_vca.state == VCA_OFF) {
            double frequency =  _synth_note_to_frequency(note);
            modulation_trigger(&voice->modulation, frequency);
            Processor *processor = &voice->processor;
            for (int i = 0; i < processor->number_of_stages; i++) {
                ProcessorStage *stage = &processor->stages[i];
                if (stage->triggerFunc != NULL) {
                    stage->triggerFunc(stage->userData, frequency);
                }
            }
            voice->note = note;
            return;
        }
    }
}

void synth_note_off(Synth *synth, int note) {
    for (int i = 0; i < synth->number_of_voices; i++) {
        Voice *voice = &synth->voices[i];
        if (voice->note == note) {
            voice->note = 0;
            modulation_off(&voice->modulation);
            Processor *processor = &voice->processor;
            for (int j = 0; j < processor->number_of_stages; j++) {
                ProcessorStage *stage = &processor->stages[j];
                if (stage->offFunc != NULL) {
                    stage->offFunc(stage->userData);
                }
            }
        }
    }
}

void synth_set_waveform(Synth *synth, Waveform waveform) {
    for (int i = 0; i < synth->number_of_voices; i++) {
        oscillator_set_waveform(&synth->voices[i].oscillator, waveform);
        //oscillator_set_waveform(&synth->voices[i].oscillator2, waveform);
    }
}

double synth_poll(Synth *synth, double delta_time) {
    double amplitude = 0;
    for (int i = 0; i < synth->number_of_voices; i++) {
        Voice *voice = &synth->voices[i];
        modulation_transform(&voice->modulation, 1, delta_time);
        Processor *processor =&voice->processor;

        double processor_amp = 1;
        for (int j = 0; j < processor->number_of_stages; j++) {
            ProcessorStage *stage = &processor->stages[j];
            if (stage->transformFunc != NULL) {
                processor_amp = stage->transformFunc(stage->userData, processor_amp, delta_time);
            }
        }
        amplitude += processor_amp;
    }
    amplitude =  amplitude/(double)synth->number_of_voices;
    for (int i = 0; i < synth->number_of_post_stages; i++) {
        ProcessorStage *stage = &synth->post_stages[i];
        if (stage->transformFunc != NULL) {
            amplitude = stage->transformFunc(stage->userData, amplitude, delta_time);
        }
    }
    return amplitude;
}
