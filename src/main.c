#include <stdio.h>
#include "mixer.h"

int main(int argc, char **argv) {
    Synth *synth = synth_create();
    Mixer *mixer = mixer_create(synth);
    if (mixer == NULL) {
        return 1;
    }
    mixer_start(mixer);
    int song[] = {-12,3,7,12,0,3,7,10,-16,3,7,8,-4,3,5,7};
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < sizeof(song)/sizeof(int); i++) {
            synth_play(synth, song[i]+36);
            SDL_Delay(300);
        }
    }
    mixer_stop(mixer);
    printf("Hello world\n");
    mixer_destroy(mixer);
    return 0;
}

