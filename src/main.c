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
            synth_note_on(synth, song[i]+36);
            int a = i%2;
            int b = 1-a;
            SDL_Delay(160-a*140);
            synth_note_off(synth,song[i]+36);
            SDL_Delay(160-b*140);
        }
    }
    SDL_Delay(1000);
    mixer_stop(mixer);
    mixer_destroy(mixer);
    return 0;
}

