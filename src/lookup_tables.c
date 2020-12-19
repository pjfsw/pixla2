#include <math.h>
#include "lookup_tables.h"
#include "echo.h"

// Oscillator
double _lookup_oscillator_phase[256];

// Filter
double _lookup_filter_frequency[256];
double _lookup_filter_q[256];
double _lookup_volume[256];
double _lookup_detune_fine[256];
double _lookup_mix_balance[256];
double _lookup_echo_time[256];
double _lookup_echo_wetness[256];
double _lookup_echo_feedback[256];
// Modulation
double _lookup_delay[256];
double _lookup_lfo_amount[256];
double _lookup_lfo_frequency[256];
double _lookup_mod_frequency[256];
double _lookup_ring_mod_amount[256];
double _lookup_ring_mod_frequency[256];

#define _BTD(x) ((double)x/255.0)

void _create_squared_table(double *table, double low, double high) {
    for (int i = 0; i < 256; i++) {
        double v = low + (high - low) * _BTD(i) * _BTD(i);
        table[i] = v;
    }
}

void _create_squareroot_table(double *table, double low, double high) {
    for (int i = 0; i < 256; i++) {
        double v = low + (high - low) * sqrt(_BTD(i));
        table[i] = v;
    }
}

void _create_linear_table(double *table, double low, double high) {
    for (int i = 0; i < 256; i++) {
        double v = low + (high - low) * _BTD(i);
        table[i] = v;
    }
}

void _create_octave_table(double *table, double base, double fraction) {
    for (int i = 0; i < 256; i++) {
        table[i] = base * pow(2, (double)i*fraction/12.0);
    }
}

void lookup_tables_init() {
    _create_squared_table(_lookup_filter_frequency, 0.001, 0.999);
    _create_squareroot_table(_lookup_filter_q, 0.0, 0.999);
    _create_squared_table(_lookup_delay, 0.0, 20.0);
    _create_linear_table(_lookup_lfo_frequency, 0.0, 20.0);
    _create_linear_table(_lookup_lfo_amount, 0.0, 0.5);
    _create_squared_table(_lookup_volume, 0.0, 1.2);
    _create_squared_table(_lookup_detune_fine, 0.0, 0.5);
    _create_linear_table(_lookup_mix_balance, 0.0, 1.0);
    _create_squareroot_table(_lookup_echo_feedback, 0.01, 0.99);
    _create_squareroot_table(_lookup_echo_wetness, 0.01, 0.99);
    _create_squared_table(_lookup_echo_time, 0.01, (double)ECHO_BUFFER);
    _create_linear_table(_lookup_mod_frequency, 0, 1.0);
    _create_linear_table(_lookup_oscillator_phase, 0.005, 0.995);
    _create_linear_table(_lookup_ring_mod_amount, 0.0, 1.0);
    _create_octave_table(_lookup_ring_mod_frequency, 1.0, 0.6);

}

double lookup_oscillator_phase(Uint8 value) {
    return _lookup_oscillator_phase[value];
}


double lookup_ring_mod_amount(Uint8 value) {
    return _lookup_ring_mod_amount[value];
}

double lookup_ring_mod_frequency(Uint8 value) {
    return _lookup_ring_mod_frequency[value];
}

double lookup_delay(Uint8 value) {
    return _lookup_delay[value];
}

double lookup_lfo_frequency(Uint8 value) {
    return _lookup_lfo_frequency[value];
}

double lookup_lfo_amount(Uint8 value) {
    return _lookup_lfo_amount[value];
}

double lookup_filter_frequency(Uint8 value) {
    return _lookup_filter_frequency[value];
}

double lookup_filter_q(Uint8 value) {
    return _lookup_filter_q[value];
}

double lookup_volume(Uint8 value) {
    return _lookup_volume[value];
}

double lookup_detune_fine(Uint8 value) {
    return _lookup_detune_fine[value];
}

double lookup_mix_balance(Uint8 value) {
    return _lookup_mix_balance[value];
}

double lookup_echo_time(Uint8 value) {
    return _lookup_echo_time[value];
}

double lookup_echo_wetness(Uint8 value) {
    return _lookup_echo_wetness[value];
}

double lookup_echo_feedback(Uint8 value) {
    return _lookup_echo_feedback[value];
}

double lookup_mod_frequency(Uint8 value) {
    return _lookup_mod_frequency[value];
}
