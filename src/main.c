#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mixer.h"

int main(int argc, char **argv) {
    Synth *synth = synth_create();
    Mixer *mixer = mixer_create(synth);
    if (mixer == NULL) {
        return 1;
    }
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
        "Pixla2",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_INPUT_GRABBED);
    if (window == NULL) {
        printf("Failed to create window: %s\n", SDL_GetError());
        synth_destroy(synth);
        mixer_stop(mixer);
        mixer_destroy(mixer);
        return 1;
    }

    int scanCodeToNote[512];
    scanCodeToNote[SDL_SCANCODE_Q] = 12;
    scanCodeToNote[SDL_SCANCODE_2] = 13;
    scanCodeToNote[SDL_SCANCODE_W] = 14;
    scanCodeToNote[SDL_SCANCODE_3] = 15;
    scanCodeToNote[SDL_SCANCODE_E] = 16;
    scanCodeToNote[SDL_SCANCODE_R] = 17;
    scanCodeToNote[SDL_SCANCODE_5] = 18;
    scanCodeToNote[SDL_SCANCODE_T] = 19;
    scanCodeToNote[SDL_SCANCODE_6] = 20;
    scanCodeToNote[SDL_SCANCODE_Y] = 21;
    scanCodeToNote[SDL_SCANCODE_7] = 22;
    scanCodeToNote[SDL_SCANCODE_U] = 23;
    scanCodeToNote[SDL_SCANCODE_I] = 24;
    scanCodeToNote[SDL_SCANCODE_9] = 25;
    scanCodeToNote[SDL_SCANCODE_O] = 26;
    scanCodeToNote[SDL_SCANCODE_0] = 27;
    scanCodeToNote[SDL_SCANCODE_P] = 28;

    bool run = true;
    mixer_start(mixer);
    SDL_Event event;
    int octave = 1;
    SDL_Scancode sc;
    while (run) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.repeat > 0) {
                    break;
                }
                sc = event.key.keysym.scancode;
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    run = false;
                }
                if (scanCodeToNote[sc] != 0) {
                    synth_note_on(synth, scanCodeToNote[sc] + 12 * octave);
                }
                break;
            case SDL_KEYUP:
                sc = event.key.keysym.scancode;
                if (scanCodeToNote[sc] != 0) {
                    synth_note_off(synth, scanCodeToNote[sc] + 12 * octave);
                }
                break;
            case SDL_QUIT:
                run = false;
                break;
            }
        }
        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);

    /*int track1[] = {
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
            SDL_Delay(100);
            synth_note_off(synth,track1[t1]);
            synth_note_off(synth,track2[t2]);
            SDL_Delay(120);
        }
    }
    SDL_Delay(6000);*/
    mixer_stop(mixer);
    mixer_destroy(mixer);
    return 0;
}

