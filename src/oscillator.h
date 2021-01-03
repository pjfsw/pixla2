
#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

#include <stdbool.h>
#include <SDL2/SDL.h>

#define _OSCILLATOR_DISPLAY_SIZE 256

typedef enum {
    SINE=0,
    SAW=1,
    SQUARE=2,
    TRIANGLE=3,
} Waveform;

typedef enum {
    PHASE_OFF,
    PHASE_SUB,
    PHASE_MUL
} PhaseMode;

typedef enum {
    TRANSP_NONE,
    TRANSP_OCT_DOWN,
    TRANSP_2OCT_DOWN,
    TRANSP_3OCT_DOWN,
    TRANSP_OCT_UP,
    TRANSP_2OCT_UP,
    TRANSP_3OCT_UP
} Transpose;

typedef struct {
    int waveform;
    int transpose;
    int phase_mode;
    Uint8 phase;
} OscillatorSettings;

typedef struct {
    OscillatorSettings *settings;
    double fm;
    double pm;
    double frequency;
    double t;
    double state[3];
    int tag;
    double display[_OSCILLATOR_DISPLAY_SIZE];
    double display_pos;
} Oscillator;

void oscillator_reset(Oscillator *oscillator);

void oscillator_set_fm(Oscillator *oscillator, double fm);

void oscillator_set_pm(Oscillator *oscillator, double pm);

void oscillator_set_waveform(Oscillator *oscillator, Waveform waveform);

void oscillator_trigger(void *user_data, double frequency);

void oscillator_set_phase(Oscillator *oscillator, double phase);

double oscillator_transform(void *user_data, double value, double delta_time);

#endif /* SRC_OSCILLATOR_H_ */
