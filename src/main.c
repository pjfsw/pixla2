#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mixer.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Synth *synth;
    Mixer *mixer;
} Instance;

void destroy_instance(Instance *instance) {
    if (instance == NULL) {
        return;
    }
    if (instance->mixer != NULL) {
        mixer_stop(instance->mixer);
        mixer_destroy(instance->mixer);
    }
    if (instance->synth != NULL) {
        synth_destroy(instance->synth);
    }
    if (instance->renderer != NULL) {
        SDL_DestroyRenderer(instance->renderer);
    }
    if (instance->window != NULL) {
        SDL_DestroyWindow(instance->window);
    }
}

Instance *create_instance() {
    Instance *instance = calloc(1, sizeof(Instance));
    instance->synth = synth_create();
    instance->mixer = mixer_create(instance->synth);
    if (instance->mixer == NULL) {
        destroy_instance(instance);
        return NULL;
    }
    instance->window = SDL_CreateWindow(
        "Pixla2",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_INPUT_GRABBED);
    if (instance->window == NULL) {
        printf("Failed to create window: %s\n", SDL_GetError());
        destroy_instance(instance);
        return NULL;
    }
    instance->renderer = SDL_CreateRenderer(instance->window, -1, SDL_RENDERER_ACCELERATED);
    if (instance->renderer == NULL) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        destroy_instance(instance);
        return NULL;
    }
    return instance;
}

void draw(Instance *instance) {
    int lastHeight = 0;
    int y_offset = 128;
    int x_offset = 0;
    int width = instance->mixer->tap_size;
    SDL_Rect rect = {
        .x=x_offset,
        .y=0,
        .w=width,
        .h=y_offset*2
    };
    SDL_SetRenderDrawColor(instance->renderer, 31,31,31,255);
    SDL_RenderFillRect(instance->renderer, &rect);
    SDL_SetRenderDrawColor(instance->renderer, 255,255,255,255);
    SDL_RenderDrawLine(instance->renderer,x_offset,y_offset,x_offset+width,y_offset);
    for (int i = 0; i < width; i++) {
        float v = instance->mixer->left_tap[i];
        int height = y_offset * v;
        SDL_RenderDrawLine(instance->renderer,i, y_offset+lastHeight,i+1,y_offset+height);
        lastHeight = height;
    }

}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    Instance *instance = create_instance();
    if (instance == NULL) {
        return 1;
    }

    int scanCodeToNote[512];
    scanCodeToNote[SDL_SCANCODE_Z] = 12;
    scanCodeToNote[SDL_SCANCODE_S] = 13;
    scanCodeToNote[SDL_SCANCODE_X] = 14;
    scanCodeToNote[SDL_SCANCODE_D] = 15;
    scanCodeToNote[SDL_SCANCODE_C] = 16;
    scanCodeToNote[SDL_SCANCODE_V] = 17;
    scanCodeToNote[SDL_SCANCODE_G] = 18;
    scanCodeToNote[SDL_SCANCODE_B] = 19;
    scanCodeToNote[SDL_SCANCODE_H] = 20;
    scanCodeToNote[SDL_SCANCODE_N] = 21;
    scanCodeToNote[SDL_SCANCODE_J] = 22;
    scanCodeToNote[SDL_SCANCODE_M] = 23;
    scanCodeToNote[SDL_SCANCODE_COMMA] = 24;
    scanCodeToNote[SDL_SCANCODE_Q] = 24;
    scanCodeToNote[SDL_SCANCODE_2] = 25;
    scanCodeToNote[SDL_SCANCODE_W] = 26;
    scanCodeToNote[SDL_SCANCODE_3] = 27;
    scanCodeToNote[SDL_SCANCODE_E] = 28;
    scanCodeToNote[SDL_SCANCODE_R] = 29;
    scanCodeToNote[SDL_SCANCODE_5] = 30;
    scanCodeToNote[SDL_SCANCODE_T] = 31;
    scanCodeToNote[SDL_SCANCODE_6] = 32;
    scanCodeToNote[SDL_SCANCODE_Y] = 33;
    scanCodeToNote[SDL_SCANCODE_7] = 34;
    scanCodeToNote[SDL_SCANCODE_U] = 35;
    scanCodeToNote[SDL_SCANCODE_I] = 36;
    scanCodeToNote[SDL_SCANCODE_9] = 37;
    scanCodeToNote[SDL_SCANCODE_O] = 38;
    scanCodeToNote[SDL_SCANCODE_0] = 39;
    scanCodeToNote[SDL_SCANCODE_P] = 40;

    bool run = true;
    mixer_start(instance->mixer);
    SDL_Event event;
    int octave = 0;
    SDL_Scancode sc;
    while (run) {
        if (SDL_PollEvent(&event)) {
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
                    synth_note_on(instance->synth, scanCodeToNote[sc] + 12 * octave);
                }
                break;
            case SDL_KEYUP:
                sc = event.key.keysym.scancode;
                if (scanCodeToNote[sc] != 0) {
                    synth_note_off(instance->synth, scanCodeToNote[sc] + 12 * octave);
                }
                break;
            case SDL_QUIT:
                run = false;
                break;
            }
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);
        draw(instance);
        SDL_RenderPresent(instance->renderer);
        SDL_Delay(1);
    }

    destroy_instance(instance);

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
    return 0;
}

