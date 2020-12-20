#ifndef SRC_LOOKUP_TABLES_H_
#define SRC_LOOKUP_TABLES_H_

#include <SDL2/SDL.h>

void lookup_tables_init();

double lookup_oscillator_phase(Uint8 value);

double lookup_ring_mod_amount(Uint8 value);

double lookup_ring_mod_frequency(Uint8 value);

double lookup_filter_frequency(Uint8 value);

double lookup_filter_q(Uint8 value);

double lookup_delay(Uint8 value);

double lookup_lfo_frequency(Uint8 value);

double lookup_lfo_amount(Uint8 value);

double lookup_volume(Uint8 value);

Uint8 lookup_volume_minus_6dbfs();

double lookup_detune_fine(Uint8 value);

double lookup_mix_balance(Uint8 value);

double lookup_echo_time(Uint8 value);

double lookup_echo_wetness(Uint8 value);

double lookup_echo_feedback(Uint8 value);

double lookup_mod_frequency(Uint8 value);

double lookup_mod_vca_strength(Uint8 value);

#endif /* SRC_LOOKUP_TABLES_H_ */
