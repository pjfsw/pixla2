#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mixer.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Synth *synth;
    Synth *bass;
    Mixer *mixer;
    int waveform;
} Instance;

int scanCodeToNote[512];
#define VU_TABLE_SIZE 100000
int vu_table[VU_TABLE_SIZE];

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
    if (instance->bass != NULL) {
        synth_destroy(instance->bass);
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
    instance->synth->master_level = 0.9;
    instance->bass = synth_create();
    instance->bass->master_level = 1.0;
    instance->bass->voice_vca_settings.attack = 0.0;
    instance->bass->voice_vca_settings.decay = 1.0;
    instance->bass->voice_vca_settings.release = 0.0;
    Synth *synths[] = {instance->synth, instance->bass};
    instance->mixer = mixer_create(synths, 2);
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
        int height = vu_table[(int)fabs(v*VU_TABLE_SIZE)];
        if (v < 0) {
            height = -height;
        }
        SDL_RenderDrawLine(instance->renderer,i, y_offset+lastHeight,i+1,y_offset+height);
        lastHeight = height;
    }

}

bool handle_event(Instance *instance, SDL_Event *event) {
    SDL_Scancode sc;
    int octave = 3;
    bool run = true;

    SDL_KeyboardEvent key = event->key;

    switch (event->type) {
    case SDL_KEYDOWN:
        if (key.repeat > 0) {
            break;
        }
        sc = key.keysym.scancode;
        if (key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            run = false;
        }
        if (key.keysym.scancode == SDL_SCANCODE_SPACE) {
            instance->waveform = (instance->waveform + 1) % 4;
            printf("Set %d\n", instance->waveform);
            synth_set_waveform(instance->synth, instance->waveform);
        }
        if (scanCodeToNote[sc] != 0) {
            synth_note_on(instance->synth, scanCodeToNote[sc] + 12 * octave);
        }
        break;
    case SDL_KEYUP:
        sc = key.keysym.scancode;
        if (scanCodeToNote[sc] != 0) {
            synth_note_off(instance->synth, scanCodeToNote[sc] + 12 * octave);
        }
        break;
    case SDL_QUIT:
        run = false;
        break;
    }
    return run;
}

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    Instance *instance = create_instance();
    if (instance == NULL) {
        return 1;
    }

    for (int i = 0; i < VU_TABLE_SIZE; i++) {
        //logTable[i] = 100+20*log10(((double)i+1)/LOG_TABLE_SIZE);
        vu_table[i]  = sqrt((double)i/VU_TABLE_SIZE) * 128.0;
    }

    scanCodeToNote[SDL_SCANCODE_Z] = 3;
    scanCodeToNote[SDL_SCANCODE_S] = 4;
    scanCodeToNote[SDL_SCANCODE_X] = 5;
    scanCodeToNote[SDL_SCANCODE_D] = 6;
    scanCodeToNote[SDL_SCANCODE_C] = 7;
    scanCodeToNote[SDL_SCANCODE_V] = 8;
    scanCodeToNote[SDL_SCANCODE_G] = 9;
    scanCodeToNote[SDL_SCANCODE_B] = 10;
    scanCodeToNote[SDL_SCANCODE_H] = 11;
    scanCodeToNote[SDL_SCANCODE_N] = 12;
    scanCodeToNote[SDL_SCANCODE_J] = 13;
    scanCodeToNote[SDL_SCANCODE_M] = 14;
    scanCodeToNote[SDL_SCANCODE_COMMA] = 15;
    scanCodeToNote[SDL_SCANCODE_Q] = 15;
    scanCodeToNote[SDL_SCANCODE_2] = 16;
    scanCodeToNote[SDL_SCANCODE_W] = 17;
    scanCodeToNote[SDL_SCANCODE_3] = 18;
    scanCodeToNote[SDL_SCANCODE_E] = 19;
    scanCodeToNote[SDL_SCANCODE_R] = 20;
    scanCodeToNote[SDL_SCANCODE_5] = 21;
    scanCodeToNote[SDL_SCANCODE_T] = 22;
    scanCodeToNote[SDL_SCANCODE_6] = 23;
    scanCodeToNote[SDL_SCANCODE_Y] = 24;
    scanCodeToNote[SDL_SCANCODE_7] = 25;
    scanCodeToNote[SDL_SCANCODE_U] = 26;
    scanCodeToNote[SDL_SCANCODE_I] = 27;
    scanCodeToNote[SDL_SCANCODE_9] = 28;
    scanCodeToNote[SDL_SCANCODE_O] = 29;
    scanCodeToNote[SDL_SCANCODE_0] = 30;
    scanCodeToNote[SDL_SCANCODE_P] = 31;

    bool run = true;
    mixer_start(instance->mixer);
    SDL_Event event;
    long t = 0;
    int bassline[] = { 5,17,5,5,17,5,5,17,
        5,17,5,5,17,5,5,17,
        5,17,5,5,17,5,5,17,
        5,17,5,5,17,5,5,17,
        8,20,8,8,20,8,8,20,
        8,20,8,8,20,8,8,20,
        8,20,8,8,20,8,8,20,
        8,20,8,8,20,8,8,20,
        1,13,1,1,13,1,1,13,
        1,13,1,1,13,1,1,13,
        1,13,1,1,13,1,1,13,
        1,13,1,1,13,1,1,13,
        3,15,3,3,15,3,3,15,
        3,15,3,3,15,3,3,15,
        3,15,3,3,15,3,3,15,
        3,15,3,3,15,3,20,22,
    };
    int songsize = sizeof(bassline)/sizeof(int);
    int songpos = 0;
    int playspeed=150;
    int notelength=50;
    int lastnote =0 ;
    long lasttick = SDL_GetTicks();
    while (run) {
        if (SDL_PollEvent(&event)) {
            run = handle_event(instance, &event);
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);
        draw(instance);
        SDL_RenderPresent(instance->renderer);
        SDL_Delay(1);
        long tick = SDL_GetTicks();
        t += (tick-lasttick);
        lasttick = tick;
        if (t > playspeed) {
            int note = bassline[songpos];
            if (note > 0) {
                synth_note_on(instance->bass,note);
            }
            lastnote =note;
            songpos = (songpos + 1) % songsize;
            t-= playspeed;
        }
        if (t > notelength && lastnote != 0) {
            synth_note_off(instance->bass,lastnote);
            lastnote=0;
        }

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

