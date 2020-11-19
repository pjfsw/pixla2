#include <SDL2/SDL.h>

#include "vca.h"

void vca_reset(Vca *vca) {
    SDL_memset(vca, 0, sizeof(Vca));
}

void vca_trigger(Vca *vca) {
    vca->state = VCA_ACTIVE;
    vca->t = 0;
}

void vca_off(Vca *vca) {
    if (vca->state == VCA_ACTIVE) {
        vca->state = VCA_RELEASE;
        vca->t = 0;
    }
}

double vca_poll(Vca *vca, double delta_time) {
    double amp = 0;
    if (vca->state == VCA_ACTIVE) {
        return 1;
    } else if (vca->state == VCA_RELEASE) {
        amp = 1/(20*vca->t+1) - 0.03;
        if (amp < 0) {
            vca->state = VCA_OFF;
            vca->t = 0;
            return 0;
        } else {
            vca->t += delta_time;
            return amp;
        }
    } else {
        return 0;
    }
}
