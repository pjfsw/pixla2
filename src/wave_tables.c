#include <math.h>
#include <stdio.h>
#include "wave_tables.h"
#include "midi_notes.h"

#define _WAVE_TABLE_NOTE_OFFSET 15
#define _WAVE_TABLE_HARMONICS 29
#define _WAVE_TABLE_HARMONIC_STEP (16000/_WAVE_TABLE_HARMONICS)
#define _WAVE_TABLE_SIZE 48000
double _wave_square_table[_WAVE_TABLE_HARMONICS][_WAVE_TABLE_SIZE];
double _wave_saw_table[_WAVE_TABLE_HARMONICS][_WAVE_TABLE_SIZE];

double _indexed_sin(int index) {
    return sin((double)index/_WAVE_TABLE_SIZE * 2 * M_PI);
}

void wave_tables_init() {
    for (int harmonic = 0; harmonic < _WAVE_TABLE_HARMONICS; harmonic++) {
        int note = (harmonic << 2) + _WAVE_TABLE_NOTE_OFFSET;
        double base_f = midi_get_frequency(note);
        printf("Generating wave tables %d/%d (Note %d, Base frequency %f)\n",
            harmonic+1, _WAVE_TABLE_HARMONICS, note, base_f);
        for (int i = 0; i < _WAVE_TABLE_SIZE; i++) {
            //_wave_square_table[harmonic][i] = _indexed_sin(i);
            int h1 = 1;
            _wave_square_table[harmonic][i] = 0;
            while (h1 * base_f < 20000) {
                _wave_square_table[harmonic][i] += _indexed_sin(i * h1)/(double)(h1);
                h1 += 2;
            }
            h1 = 1;
            int sign = -1;
            _wave_saw_table[harmonic][i] = 0;
            while (h1 * base_f < 20000) {
                _wave_saw_table[harmonic][i] += sign * _indexed_sin(i * h1)/(double)h1;
                sign *= -1;
                h1++;
            }
            _wave_saw_table[harmonic][i] *= 0.53;

            if (fabs(_wave_square_table[harmonic][i]) > 1.0) {
                fprintf(stderr, "Square overflow detected, harmonic %d, index %d, value %f\n", harmonic, i, _wave_square_table[harmonic][i]);
            }
            if (fabs(_wave_saw_table[harmonic][i]) > 1.0) {
                fprintf(stderr, "Saw overflow detected, harmonic %d, index %d, value %f\n", harmonic, i, _wave_saw_table[harmonic][i]);
            }
        }
    }
}

int _wave_table_get_harmonic(double frequency) {
    int harmonic = ((int)midi_get_note(frequency) - _WAVE_TABLE_NOTE_OFFSET) >> 2;
    if (harmonic < 0) {
        harmonic = 0;
    }
    if (harmonic > _WAVE_TABLE_HARMONICS-1) {
        harmonic = _WAVE_TABLE_HARMONICS-1;
    }
    return harmonic;
}

double wave_table_square(double frequency, double t) {
    int harmonic = _wave_table_get_harmonic(frequency);
    int index = (int)(frequency * t * _WAVE_TABLE_SIZE) % _WAVE_TABLE_SIZE ;
    double amp = _wave_square_table[harmonic][index];
    return amp;
}

double wave_table_saw(double frequency, double t) {
    int harmonic = _wave_table_get_harmonic(frequency);

    int index = (int)(frequency * t * _WAVE_TABLE_SIZE) % _WAVE_TABLE_SIZE ;
    double amp = _wave_saw_table[harmonic][index];
    return amp;
}

