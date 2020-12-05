#include <SDL2/SDL.h>

#include "vca.h"

#define VCA_ZERO_THRESHOLD 0.00002 // -94 dBFS

#define _VCA_TABLE_SIZE 512

double _vca_attack_table[_VCA_TABLE_SIZE];

void vca_init_static() {
    double db_scale = _VCA_TABLE_SIZE/96.0;

    for (int i = 0; i < _VCA_TABLE_SIZE; i++) {
        _vca_attack_table[i] = i/(double)_VCA_TABLE_SIZE;
//        double db = (double)(_VCA_TABLE_SIZE-i-1)/db_scale;
        //_vca_attack_table[i] = pow(10, -db/20.0);
        printf("Attack %d=%f\n", i, _vca_attack_table[i]);
    }
}

double _vca_get_scaled_decay_release(double decay_release) {
    double d2 = 2 * decay_release;
    return d2 * d2 * d2 * d2;
}

void vca_trigger(void *user_data, double frequency) {
    Vca *vca = (Vca*)user_data;
    vca->attack = vca->settings->attack;
    if (vca->attack > 0) {
        vca->attack_speed = (1.001-vca->attack);
        vca->attack_speed = 300.0 * (double)_VCA_TABLE_SIZE * vca->attack_speed * vca->attack_speed * vca->attack_speed* vca->attack_speed;
    }

    vca->decay = vca->settings->decay;
    vca->sustain = vca->settings->sustain;
    vca->release = vca->settings->release;
    vca->inverse = vca->settings->inverse;
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
        if (vca->release > 0) {
            vca->state = VCA_RELEASE;
        } else {
            vca->state = VCA_OFF;
        }
        vca->t = 0;
    }
}

double _vca_get_decay_release(Vca *vca, double decay_or_release) {
    //return 0.5 / (3*vca->t + 0.455)-0.1;
    //double dr = _vca_get_scaled_decay_release(decay_or_release);
    //return dr/(dr+vca->t)-0.03;
    if (decay_or_release > 0) {
        return 1-(0.5/decay_or_release)*vca->t;
    } else {
        return 0;
    }
}

double _vca_get_attack(Vca *vca) {
    if (vca->attack > 0 ) {
        double index = vca->attack_speed * vca->t;
        if (index > _VCA_TABLE_SIZE - 1) {
            return 1.1;
        }
        return _vca_attack_table[(int)index];
    } else {
        return 1.1;
    }
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
            if (vca->decay > 0) {
                vca->state = VCA_DECAY;
            } else {
                vca->state = VCA_SUSTAIN;
                vca->amp = vca->sustain;
            }
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
