#include <stdio.h>
#include "mixer.h"

int main(int argc, char **argv) {
    Synth *synth = synth_create();
    Mixer *mixer = mixer_create(synth);
    if (mixer == NULL) {
        return 1;
    }
    mixer_start(mixer);
    int track1[] = {
        12,24,0,12,0,12,24,12,
        15,27,0,15,0,15,27,15,
        10,22,0,10,0,10,22,10,
        8,20,0,8,0,8,20,8
    };
    int track1Len = sizeof(track1)/sizeof(int);
    int track2[] = {36,38,39,41,43,0,46,0,34,39,39,41,39,0,39,0};
    int track2Len = sizeof(track2)/sizeof(int);

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < track1Len; i++) {
            int t1 = i;
            int t2 = i % track2Len;
            if (track1[t1] > 0) {
                synth_note_on(synth, track1[t1]);
            }
            if (track2[t2] > 0) {
                synth_note_on(synth, track2[t2]);
            }
            SDL_Delay(80);
            synth_note_off(synth,track1[t1]);
            synth_note_off(synth,track2[t2]);
            SDL_Delay(160);
        }
    }
    SDL_Delay(6000);
    mixer_stop(mixer);
    mixer_destroy(mixer);
    return 0;
}

