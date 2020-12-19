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
    synth->settings.master_level = 255;
    synth->number_of_voices = 4;
    synth->voices = calloc(synth->number_of_voices, sizeof(Voice));

    synth->settings.voice_vca_settings.attack = 8;
    synth->settings.voice_vca_settings.decay = 16;
    synth->settings.voice_vca_settings.sustain = 192;
    synth->settings.voice_vca_settings.release = 64;

    synth->settings.combiner_settings.combine_mode = COMB_ADD;
    synth->settings.combiner_settings.oscillator2_strength = 0;

    synth->settings.filter_vca_settings.attack =0;
    synth->settings.filter_vca_settings.decay = 0;
    synth->settings.filter_vca_settings.sustain = 255;
    synth->settings.filter_vca_settings.release = 255;
    synth->settings.filter_settings.f = 255;
    synth->settings.filter_settings.q = 0;

    for (int i = 0; i < NUMBER_OF_OSCILLATORS; i++) {
        synth->settings.oscillator_settings[i].waveform = SAW;
    }

    synth->settings.modulation_settings.lfo[0].amount = 16;
    synth->settings.modulation_settings.lfo[0].delay = 128;
    synth->settings.modulation_settings.lfo[0].frequency = 64;
    synth->settings.modulation_settings.lfo[0].oscillator.waveform = SINE;

    for (int i = 0; i < synth->number_of_voices; i++) {
        Voice *voice = &synth->voices[i];

        Vca *adsr = &voice->voice_vca;
        adsr->settings = &synth->settings.voice_vca_settings;

        for (int osc = 0; osc < NUMBER_OF_OSCILLATORS; osc++) {
            voice->oscillators[osc].settings = &synth->settings.oscillator_settings[osc];
            voice->oscillators[osc].tag = osc+1;
        }

        modulation_init(&voice->modulation, &voice->oscillators[0], &voice->oscillators[1],
            &voice->filter, &voice->combiner);
        voice->modulation.vca.settings = &synth->settings.modulation_settings.vca;
        voice->modulation.settings = &synth->settings.modulation_settings;
        for (int mods = 0; mods < NUMBER_OF_MODULATORS; mods++) {
            voice->modulation.lfo[mods].settings = &synth->settings.modulation_settings.lfo[mods];
            voice->modulation.lfo[mods].oscillator.settings = &synth->settings.modulation_settings.lfo[mods].oscillator;
        }

        Processor *processor = &voice->processor;
        processor->number_of_stages = 3;
        processor->stages = calloc(processor->number_of_stages, sizeof(ProcessorStage));

        int stage = 0;

        voice->combiner.settings = &synth->settings.combiner_settings;
        voice->combiner.oscillator1 = &voice->oscillators[0];
        voice->combiner.oscillator2 = &voice->oscillators[1];
        voice->combiner.ring_modulator.settings = &voice->combiner.settings->ring_oscillator;

        processor_set_stage(&processor->stages[stage++],
            &voice->combiner, combiner_transform, combiner_trigger, combiner_off);

        voice->filter.vca.settings = &synth->settings.filter_vca_settings;
        voice->filter.settings = &synth->settings.filter_settings;
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

void _synth_voice_off(Voice *voice) {
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

void synth_note_off(Synth *synth, int note) {
    for (int i = 0; i < synth->number_of_voices; i++) {
        Voice *voice = &synth->voices[i];

        if (voice->note == note) {
            _synth_voice_off(voice);
        }
    }
}

void synth_off(Synth *synth) {
    for (int i = 0; i < synth->number_of_voices; i++) {
        _synth_voice_off(&synth->voices[i]);
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
    if (synth->settings.use_echo) {
        amplitude = echo_transform(&synth->settings.echo, amplitude, delta_time);
    }
    return amplitude;
}
