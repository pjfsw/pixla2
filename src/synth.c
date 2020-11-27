#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "oscillator.h"
#include "synth.h"
#include "vca.h"
#include "echo.h"
#include "filter.h"
#include "midi_notes.h"

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
    synth->combiner_settings.detune = 0.0;

    synth->combiner_vca_settings.attack = 0.5;
    synth->combiner_vca_settings.decay = 0.2;
    synth->combiner_vca_settings.sustain = 0.6;
    synth->combiner_vca_settings.release = 2.0;
    synth->combiner_vca_settings.inverse = true;

    synth->filter_vca_settings.attack =0.0;
    synth->filter_vca_settings.decay = 0.4;
    synth->filter_vca_settings.sustain = 0.3;
    synth->filter_vca_settings.release = 0.7;

    for (int i = 0; i < NUMBER_OF_OSCILLATORS; i++) {
        synth->oscillator_settings[i].waveform = SAW;
    }

    synth->modulation_settings.lfo[0].amount = 0.08;
    synth->modulation_settings.lfo[0].delay = 5;
    synth->modulation_settings.lfo[0].frequency = 5;
    synth->modulation_settings.lfo[0].oscillator.waveform = SINE;
    synth->modulation_settings.modulation_target = MOD_OSCILLATOR_FM;

    for (int i = 0; i < synth->number_of_voices; i++) {
        Voice *voice = &synth->voices[i];

        Vca *adsr = &voice->voice_vca;
        adsr->settings = &synth->voice_vca_settings;

        for (int osc = 0; osc < NUMBER_OF_OSCILLATORS; osc++) {
            voice->oscillators[osc].settings = &synth->oscillator_settings[osc];
            voice->oscillators[osc].tag = osc+1;
        }

        modulation_init(&voice->modulation, &voice->oscillators[0], &voice->oscillators[1]);
        voice->modulation.settings = &synth->modulation_settings;
        voice->modulation.lfo1.settings = &synth->modulation_settings.lfo[0];
        voice->modulation.lfo2.settings = &synth->modulation_settings.lfo[1];
        voice->modulation.lfo1.oscillator.settings = &synth->modulation_settings.lfo[0].oscillator;
        voice->modulation.lfo2.oscillator.settings = &synth->modulation_settings.lfo[1].oscillator;

        Processor *processor = &voice->processor;
        processor->number_of_stages = 3;
        processor->stages = calloc(processor->number_of_stages, sizeof(ProcessorStage));

        int stage = 0;

        voice->combiner.vca.settings = &synth->combiner_vca_settings;
        voice->combiner.settings = &synth->combiner_settings;
        voice->combiner.oscillator1 = &voice->oscillators[0];
        voice->combiner.oscillator2 = &voice->oscillators[1];

        processor_set_stage(&processor->stages[stage++],
            &voice->combiner, combiner_transform, combiner_trigger, combiner_off);

        voice->filter.vca.settings = &synth->filter_vca_settings;
        filter_set(&voice->filter, 0.8, 0.85);
        processor_set_stage(&processor->stages[stage++],
            &voice->filter, filter_transform, filter_trigger, filter_off);

        processor_set_stage(&processor->stages[stage++],
            &voice->voice_vca, vca_transform, vca_trigger, vca_off);
    }
    return synth;
}

void synth_destroy(Synth *synth) {
    if (synth != NULL) {
        for (int i = 0; i < synth->number_of_voices; i++) {
            free(synth->voices[i].processor.stages);
        }
        free(synth->voices);
        free(synth);
    }
}

void synth_note_on(Synth *synth, int note) {
    for (int i = 0; i < synth->number_of_voices; i++) {
        synth->next_voice = (synth->next_voice + 1) % synth->number_of_voices;
        Voice *voice = &synth->voices[synth->next_voice];
        if (voice->voice_vca.state == VCA_RELEASE || voice->voice_vca.state == VCA_OFF) {
            double frequency =  midi_get_frequency(note);
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
    if (synth->use_echo) {
        amplitude = echo_transform(&synth->echo, amplitude, delta_time);
    }
    return amplitude;
}
