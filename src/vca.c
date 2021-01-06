#include <SDL2/SDL.h>

#include "vca.h"
#include "lookup_tables.h"

#define _VCA_TABLE_SIZE 1024

double _vca_attack_table[_VCA_TABLE_SIZE];
double _vca_decay_table[_VCA_TABLE_SIZE];

void vca_init_static() {

    for (int i = 0; i < _VCA_TABLE_SIZE; i++) {
        double a = i/(double)_VCA_TABLE_SIZE;
        _vca_attack_table[i] = a * a;

        double d = i/(double)(_VCA_TABLE_SIZE);
        _vca_decay_table[i] = 1 - pow(d+0.00091,0.25);

        //printf("Decay %d=%f\n", i, _vca_decay_table[i]);
    }
}

double _vca_decay_speed_function(double setting, double speed) {
    double s = 1.0001 - setting;
    s = s + 0.2;
    return speed * (double)_VCA_TABLE_SIZE * s * s * s - 0.008;
}
void vca_trigger(void *user_data, double frequency, Uint8 velocity) {
    Vca *vca = (Vca*)user_data;
    vca->level = lookup_tracker_volume(velocity);
    vca->attack = vca->settings->attack/255.0;
    if (vca->attack > 0) {
        vca->attack_speed = (1.0001-vca->attack);
        vca->attack_speed = 400.0 * (double)_VCA_TABLE_SIZE * vca->attack_speed * vca->attack_speed * vca->attack_speed;
    }

    vca->decay = vca->settings->decay/255.0;
    vca->decay_speed = _vca_decay_speed_function(vca->decay, 2.0);
    vca->sustain = vca->settings->sustain/255.0;
    vca->release = vca->settings->release/255.0;
    vca->release_speed = _vca_decay_speed_function(vca->release, 2.0);

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

double _vca_get_weighted_table_value(double *table, double index, double edge) {
    double offset = floor(index);
    double v2 = index+1 >= _VCA_TABLE_SIZE ? edge : table[(int)index+1];
    return (1.0-offset) * table[(int)index] + offset * v2;
}

double _vca_get_decay_release(Vca *vca, double speed, double decay_or_release) {
    double index = speed * vca->t;
    if (index > _VCA_TABLE_SIZE - 1) {
        return -1;
    }
    return _vca_get_weighted_table_value(_vca_decay_table, index, 0.000015);
}

double _vca_get_attack(Vca *vca) {
    if (vca->attack > 0 ) {
        double index = vca->attack_speed * vca->t;
        if (index > _VCA_TABLE_SIZE - 1) {
            return 1.1;
        }
        return _vca_get_weighted_table_value(_vca_attack_table, index, 1.0);
    } else {
        return 1.1;
    }
}

double _vca_post_process(Vca *vca, double amp) {
    if (vca->inverse) {
        return vca->level * (1-amp);
    } else {
        return vca->level * amp;
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
        vca->amp = vca->sustain + (1-vca->sustain) * _vca_get_decay_release(vca, vca->decay_speed, vca->decay);
        if (vca->amp < vca->sustain) {
            vca->amp = vca->sustain;
            vca->state = VCA_SUSTAIN;
        }
        amp = vca->amp;
    } else if (vca->state == VCA_SUSTAIN) {
        return _vca_post_process(vca, vca->sustain * signal);
    } else if (vca->state == VCA_RELEASE) {
        amp = vca->amp * _vca_get_decay_release(vca, vca->release_speed, vca->release);
        if (amp < 0) {
            vca->state = VCA_OFF;
            vca->t = 0;
            vca->amp = 0;
            amp = 0;
        }
    }
    vca->t += delta_time;
    return _vca_post_process(vca, amp * signal);
}
