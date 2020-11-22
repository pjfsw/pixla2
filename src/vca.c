#include <SDL2/SDL.h>

#include "vca.h"

#define VCA_ZERO_THRESHOLD 0.00002 // -94 dBFS

void vca_initialize() {
}

void vca_set_inverse(Vca *vca, bool inverse) {
    vca->inverse = inverse;
}

void vca_trigger(void *user_data, double frequency) {
    Vca *vca = (Vca*)user_data;
    if (vca->attack > 0) {
        vca->state = VCA_ATTACK;
        vca->amp = 0.0;
    } else {
        vca->state = VCA_DECAY;
        vca->amp = 1.0;
    }
    vca->t = 0;
}

void vca_off(void *user_data) {
    Vca *vca = (Vca*)user_data;
    if (vca->state == VCA_ATTACK || vca->state == VCA_DECAY || vca->state == VCA_SUSTAIN) {
        vca->state = VCA_RELEASE;
        vca->t = 0;
    }
}

double _vca_get_decay_release(Vca *vca, double decay_or_release) {
    //return 0.5 / (3*vca->t + 0.455)-0.1;
    return decay_or_release/(decay_or_release+vca->t)-0.03;
}

double _vca_get_attack(Vca *vca) {
    return (vca->t*vca->t)/vca->attack;
}

double _vca_post_process(Vca *vca, double amp) {
    if (vca->inverse) {
        return 1-amp;
    } else {
        return amp;
    }
}

double vca_transform(void *user_data, double signal, double delta_time) {
    Vca *vca = (Vca*)user_data;

    if (vca->state == VCA_OFF) {
        return _vca_post_process(vca, 0);
    }

    double amp = 0;
    if (vca->state == VCA_ATTACK) {
        vca->amp = _vca_get_attack(vca);
        if (vca->amp > 1) {
            vca->amp = 1;
            vca->state = VCA_DECAY;
            vca->t = 0;
            return _vca_post_process(vca, vca->amp * signal);
        }
        amp = vca->amp;
    } else if (vca->state == VCA_DECAY) {
        vca->amp = vca->sustain + (1-vca->sustain) * _vca_get_decay_release(vca, vca->decay);
        if (vca->amp < vca->sustain) {
            vca->amp = vca->sustain;
            vca->state = VCA_SUSTAIN;
        }
        amp = vca->amp;
    } else if (vca->state == VCA_SUSTAIN) {
        return _vca_post_process(vca, vca->sustain * signal);
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
    return _vca_post_process(vca, amp * signal);
}
