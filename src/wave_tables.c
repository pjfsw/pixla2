#include <math.h>
#include <stdio.h>
#include "wave_tables.h"

#define _WAVE_TABLE_HARMONICS 32
#define _WAVE_TABLE_HARMONIC_STEP (16000/_WAVE_TABLE_HARMONICS)
#define _WAVE_TABLE_SIZE 48000
double _wave_square_table[_WAVE_TABLE_HARMONICS][_WAVE_TABLE_SIZE];
double _wave_saw_table[_WAVE_TABLE_HARMONICS][_WAVE_TABLE_SIZE];

double _indexed_sin(int index) {
    return sin((double)index/_WAVE_TABLE_SIZE * 2 * M_PI);
}

void wave_tables_init() {
    for (int harmonic = 0; harmonic < _WAVE_TABLE_HARMONICS; harmonic++) {
        // TODO non linear harmonic distribution..
        printf("Generating wave tables %d/%d\n", harmonic+1, _WAVE_TABLE_HARMONICS);
        double base_f = 1.0 + harmonic * _WAVE_TABLE_HARMONIC_STEP;  //(0-25600 Hz)
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

double wave_table_square(double frequency, double t) {
    int harmonic = frequency / _WAVE_TABLE_HARMONIC_STEP;
    if (harmonic < 0) {
        harmonic = 0;
    }
    if (harmonic > _WAVE_TABLE_HARMONICS-1) {
        harmonic = _WAVE_TABLE_HARMONICS-1;
    }
    int index = (int)(frequency * t * _WAVE_TABLE_SIZE) % _WAVE_TABLE_SIZE ;
    double amp = _wave_square_table[harmonic][index];
    return amp;
}

double wave_table_saw(double frequency, double t) {
    int harmonic = frequency / _WAVE_TABLE_HARMONIC_STEP;
    if (harmonic < 0) {
        harmonic = 0;
    }
    if (harmonic > _WAVE_TABLE_HARMONICS-1) {
        harmonic = _WAVE_TABLE_HARMONICS-1;
    }

    int index = (int)(frequency * t * _WAVE_TABLE_SIZE) % _WAVE_TABLE_SIZE ;
    double amp = _wave_saw_table[harmonic][index];
    return amp;
}
