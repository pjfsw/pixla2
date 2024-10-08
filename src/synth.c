#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "oscillator.h"
#include "synth.h"
#include "vca.h"
#include "echo.h"
#include "filter.h"
#include "midi_notes.h"
#include "lookup_tables.h"

void synth_settings_set_default(SynthSettings* settings) {
    settings->master_level = 255;
    settings->voice_vca_settings.attack = 8;
    settings->voice_vca_settings.decay = 16;
    settings->voice_vca_settings.sustain = 192;
    settings->voice_vca_settings.release = 64;

    settings->combiner_settings.combine_mode = COMB_ADD;
    settings->combiner_settings.oscillator2_strength = 0;

    settings->filter_vca_settings.attack =0;
    settings->filter_vca_settings.decay = 0;
    settings->filter_vca_settings.sustain = 255;
    settings->filter_vca_settings.release = 255;
    settings->filter_settings.f = 255;
    settings->filter_settings.q = 0;

    for (int i = 0; i < NUMBER_OF_OSCILLATORS; i++) {
        settings->oscillator_settings[i].waveform = SAW;
    }

    settings->modulation_settings.lfo[0].amount = 16;
    settings->modulation_settings.lfo[0].delay = 128;
    settings->modulation_settings.lfo[0].frequency = 64;
    settings->modulation_settings.lfo[0].oscillator.waveform = SINE;
}

Synth *synth_create(SynthSettings *settings, Uint32 *song_bpm, Uint32 sample_rate) {
    Synth *synth = calloc(1, sizeof(Synth));
    synth->settings = settings;
    synth->number_of_voices = 4;
    synth->voices = calloc(synth->number_of_voices, sizeof(Voice));
    synth->echo.settings = &settings->echo_settings;
    synth->echo.song_bpm = song_bpm;
    synth->echo.sample_rate = sample_rate;

    for (int i = 0; i < synth->number_of_voices; i++) {
        Voice *voice = &synth->voices[i];

        Vca *adsr = &voice->voice_vca;
        adsr->settings = &synth->settings->voice_vca_settings;

        for (int osc = 0; osc < NUMBER_OF_OSCILLATORS; osc++) {
            voice->oscillators[osc].settings = &synth->settings->oscillator_settings[osc];
            voice->oscillators[osc].tag = osc+1;
        }

        modulation_init(&voice->modulation, &voice->oscillators[0], &voice->oscillators[1],
            &voice->filter, &voice->combiner);
        voice->modulation.vca.settings = &synth->settings->modulation_settings.vca;
        voice->modulation.settings = &synth->settings->modulation_settings;
        for (int mods = 0; mods < NUMBER_OF_MODULATORS; mods++) {
            voice->modulation.lfo[mods].settings = &synth->settings->modulation_settings.lfo[mods];
            voice->modulation.lfo[mods].oscillator.settings = &synth->settings->modulation_settings.lfo[mods].oscillator;
        }

        Processor *processor = &voice->processor;
        processor->number_of_stages = 3;
        processor->stages = calloc(processor->number_of_stages, sizeof(ProcessorStage));

        int stage = 0;

        voice->combiner.settings = &synth->settings->combiner_settings;
        voice->combiner.oscillator1 = &voice->oscillators[0];
        voice->combiner.oscillator2 = &voice->oscillators[1];
        voice->combiner.ring_modulator.settings = &voice->combiner.settings->ring_oscillator;

        processor_set_stage(&processor->stages[stage++],
            &voice->combiner, combiner_transform, combiner_trigger, combiner_off);

        voice->filter.vca.settings = &synth->settings->filter_vca_settings;
        voice->filter.settings = &synth->settings->filter_settings;
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

int synth_note_on(Synth *synth, int note, int velocity) {
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
                    stage->triggerFunc(stage->userData, frequency, velocity);
                }
            }
            voice->note = note;
            voice->velocity = velocity;
            return synth->next_voice;
        }
    }
    return 0;
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

void synth_note_off(Synth *synth, int voice_id) {
    if (voice_id < 0 || voice_id >= synth->number_of_voices) {
        return;
    }
    Voice *voice = &synth->voices[voice_id];
    _synth_voice_off(voice);
}

void synth_off(Synth *synth) {
    for (int i = 0; i < synth->number_of_voices; i++) {
        _synth_voice_off(&synth->voices[i]);
    }
}

void synth_pitch_offset(Synth *synth, int voice_id, double offset) {
    if (voice_id < 0 || voice_id >= synth->number_of_voices) {
        return;
    }
    combiner_set_pitch_offset(&synth->voices[voice_id].combiner, offset);
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
    amplitude = echo_transform(&synth->echo, amplitude, delta_time);
    amplitude = lookup_volume(synth->settings->master_level) * amplitude;
    return amplitude;
}
