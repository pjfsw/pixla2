#include <SDL2/SDL.h>

#include "vca.h"

#define VCA_ZERO_THRESHOLD 0.0001 // -100 dBFS

void vca_initialize() {
}

void vca_trigger(void *user_data, double frequency) {
    Vca *vca = (Vca*)user_data;
    vca->state = VCA_DECAY;
    vca->t = 0;
    vca->amp = 1.0;
}

void vca_off(void *user_data) {
    Vca *vca = (Vca*)user_data;
    if (vca->state == VCA_ATTACK || vca->state == VCA_DECAY || vca->state == VCA_SUSTAIN) {
        vca->state = VCA_RELEASE;
        vca->t = 0;
    }
}

double _vca_get_decay_release(Vca *vca, double v) {
    //return 0.5 / (3*vca->t + 0.455)-0.1;
    return v/(v+vca->t)-0.03;
}

double vca_transform(void *user_data, double signal, double delta_time) {
    Vca *vca = (Vca*)user_data;

    if (vca->state == VCA_OFF) {
        return 0;
    }

    double amp = 0;
    if (vca->state == VCA_DECAY) {
        vca->amp = vca->sustain + (1-vca->sustain) * _vca_get_decay_release(vca, vca->decay);
        if (vca->amp < vca->sustain) {
            vca->amp = vca->sustain;
            vca->state = VCA_SUSTAIN;
        }
        amp = vca->amp;
    } else if (vca->state == VCA_SUSTAIN) {
        return vca->sustain * signal;
    } else if (vca->state == VCA_RELEASE) {
        amp = vca->amp * _vca_get_decay_release(vca, vca->release);
        if (amp < VCA_ZERO_THRESHOLD) {
            vca->state = VCA_OFF;
            vca->t = 0;
            vca->amp = 0;
            amp = 0;
        }
    }
    vca->t += delta_time;
    return amp * signal;
}
