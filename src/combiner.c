#include <math.h>
#include "combiner.h"
#include "vca.h"
#include "lookup_tables.h"

void _combiner_set_frequency(Combiner* combiner) {
    double frequency = combiner->base_frequency * pow(2, combiner->pitch_offset);
    combiner->frequency = fmax(fmin(13000,frequency), 16);
    double detune_up = pow(2, lookup_detune_fine(combiner->settings->detune)/12);
    double detune_down = pow(2, -lookup_detune_fine(combiner->settings->detune)/12);
    oscillator_trigger(combiner->oscillator1,
        detune_up * combiner->frequency);
    oscillator_trigger(combiner->oscillator2,
        detune_down * combiner->frequency);
}

void combiner_trigger(void *user_data, double frequency, Uint8 velocity) {
    Combiner* combiner = (Combiner*)user_data;
    combiner->base_frequency = frequency;
    combiner->pitch_offset = 0;
    _combiner_set_frequency(combiner);
}

void combiner_off(void *user_data) {
}

void combiner_set_osc2_mix(Combiner *combiner, double mix) {
    combiner->osc2_mix = mix;
}

void combiner_set_ring_freq_mod(Combiner *combiner, double ring_freq_mod) {
    combiner->ring_freq_mod = ring_freq_mod;
}

void combiner_set_ring_amount_mod(Combiner *combiner, double ring_amount_mod) {
    combiner->ring_amount_mod = ring_amount_mod;
}

void combiner_set_pitch_offset(Combiner *combiner, double pitch_offset) {
    combiner->pitch_offset = pitch_offset;
    _combiner_set_frequency(combiner);
}

double combiner_transform(void *user_data, double value, double delta_time) {
    Combiner* combiner = (Combiner*)user_data;

    double osc2 = oscillator_transform(combiner->oscillator2, value, delta_time);
    double osc = 0;
    double osc2_mix = combiner->osc2_mix;
    double amp2 = lookup_mix_balance(combiner->settings->oscillator2_strength);

    amp2 *= osc2_mix;
    if (amp2 < 0) {
        amp2 = 0;
    }
    if (amp2 > 1) {
        amp2 = 1;
    }

    if (combiner->settings->combine_mode == COMB_ADD) {
        osc = oscillator_transform(combiner->oscillator1, value, delta_time);
        osc += amp2 * osc2;
    } else if (combiner->settings->combine_mode == COMB_MODULATE) {
        oscillator_trigger(
            combiner->oscillator1,
            combiner->frequency * pow(2, 0.2 * amp2 * osc2));
        osc = oscillator_transform(combiner->oscillator1, value, delta_time);
    }

    combiner->settings->ring_oscillator.waveform = SINE;
    oscillator_trigger(&combiner->ring_modulator,
        pow(2, combiner->ring_freq_mod) *
        lookup_ring_mod_frequency(combiner->settings->ring_mod_freq));
    double ring_modulation = oscillator_transform(&combiner->ring_modulator, value, delta_time);
    double ring_amount = combiner->ring_amount_mod * lookup_ring_mod_amount(combiner->settings->ring_mod_amount);

    double v =  osc * (1.0 - ring_amount) +
        osc * ring_amount * ring_modulation;

    return v;
}
