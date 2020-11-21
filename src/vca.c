#include <SDL2/SDL.h>

#include "vca.h"

void vca_reset(Vca *vca) {
    SDL_memset(vca, 0, sizeof(Vca));
}

void vca_trigger(void *user_data, double frequency) {
    Vca *vca = (Vca*)user_data;
    vca->state = VCA_ACTIVE;
    vca->t = 0;
}

void vca_off(void *user_data) {
    Vca *vca = (Vca*)user_data;

    if (vca->state == VCA_ACTIVE) {
        vca->state = VCA_RELEASE;
        vca->t = 0;
    }
}

double vca_transform(void *user_data, double signal, double delta_time) {
    Vca *vca = (Vca*)user_data;

    double amp = 0;
    if (vca->state == VCA_ACTIVE) {
        return signal;
    } else if (vca->state == VCA_RELEASE) {
        amp = 1/(15*vca->t+1) - 0.03;
        if (amp < 0) {
            vca->state = VCA_OFF;
            vca->t = 0;
            return 0;
        } else {
            vca->t += delta_time;
            return amp * signal;
        }
    } else {
        return 0;
    }
}
