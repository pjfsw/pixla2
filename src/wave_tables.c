#include <math.h>
#include <stdio.h>
#include "wave_tables.h"
#include "midi_notes.h"

#define _WAVE_TABLE_NOTE_OFFSET 16
#define _WAVE_TABLE_HARMONICS 28
#define _WAVE_TABLE_HARMONIC_STEP (16000/_WAVE_TABLE_HARMONICS)
#define _WAVE_TABLE_SIZE 16384
double _wave_square_table[_WAVE_TABLE_HARMONICS][_WAVE_TABLE_SIZE];
double _wave_saw_table[_WAVE_TABLE_HARMONICS][_WAVE_TABLE_SIZE];
double _wave_triangle_table[_WAVE_TABLE_HARMONICS][_WAVE_TABLE_SIZE];

double _indexed_sin(int index) {
    return sin((double)index/_WAVE_TABLE_SIZE * 2 * M_PI);
}

void wave_tables_init() {
    for (int harmonic = 0; harmonic < _WAVE_TABLE_HARMONICS; harmonic++) {
        int note = (harmonic << 2) + _WAVE_TABLE_NOTE_OFFSET;
        double base_f = midi_get_frequency(note);
        printf("Generating wave tables %d/%d (Note %d, Base frequency %f)\n",
            harmonic+1, _WAVE_TABLE_HARMONICS, note, base_f);
        double saw_max = 1.0;
        for (int i = 0; i < _WAVE_TABLE_SIZE; i++) {
            // SQUARE
            int h1 = 1;
            double *amp = &_wave_square_table[harmonic][i];
            *amp = 0;
            while (h1 * base_f < 20000) {
                *amp += _indexed_sin(i * h1)/(double)(h1);
                h1 += 2;
            }
            //*amp *= 1.1;

            // SAW
            h1 = 1;
            int sign = -1;
            _wave_saw_table[harmonic][i] = 0;
            while (h1 * base_f < 20000) {
                _wave_saw_table[harmonic][i] += sign * _indexed_sin(i * h1)/(double)h1;
                sign *= -1;
                h1++;
            }
            if (fabs(_wave_saw_table[harmonic][i]) > saw_max) {
                saw_max = fabs(_wave_saw_table[harmonic][i]);
            }
            // TRIANGLE
            h1 = 1;
            sign = 1;
            _wave_triangle_table[harmonic][i] = 0;
            while (h1 * base_f < 20000) {
                _wave_triangle_table[harmonic][i] += sign * pow(h1, -2) * _indexed_sin(i * h1);
                h1+=2;
                sign *= -1;
            }
            _wave_triangle_table[harmonic][i] *= 8.0 / (M_PI * M_PI);
        }
        if (saw_max > 1.0) {
            fprintf(stderr, "Adjusting saw overflow - harmonic %d, max value %f\n", harmonic, saw_max);
            for (int i = 0; i < _WAVE_TABLE_SIZE; i++) {
                _wave_saw_table[harmonic][i] /= saw_max;
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

double _wave_table_get_amp(double frequency, double t, double (*table)[_WAVE_TABLE_SIZE]) {
    int harmonic = _wave_table_get_harmonic(frequency);
    double offset = frequency * t * _WAVE_TABLE_SIZE;
    double w2 = offset - floor(offset);
    double w1 = 1-w2;
    int index1 = (int)offset % _WAVE_TABLE_SIZE;
    int index2 = (index1 + 1) % _WAVE_TABLE_SIZE;
    //int index = (int)(frequency * t * _WAVE_TABLE_SIZE) % _WAVE_TABLE_SIZE ;
    double amp = w1 * table[harmonic][index1] + w2 * table[harmonic][index2];
    return amp;
}

double wave_table_square(double frequency, double t) {
    return _wave_table_get_amp(frequency, t, _wave_square_table);
}

double wave_table_saw(double frequency, double t) {
    return _wave_table_get_amp(frequency, t, _wave_saw_table);
}

double wave_table_triangle(double frequency, double t) {
    return _wave_table_get_amp(frequency, t, _wave_triangle_table);
}
