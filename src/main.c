#include <stdio.h>
#include "mixer.h"

int main(int argc, char **argv) {
    Mixer *mixer = mixer_create();
    if (mixer == NULL) {
        return 1;
    }
    mixer_start(mixer);
    SDL_Delay(1000);
    mixer_stop(mixer);
    printf("Hello world\n");
    mixer_destroy(mixer);
    return 0;
}

