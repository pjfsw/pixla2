#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mixer.h"

typedef struct {
    int last_note;
    int song_pos;
    int substep;
} Playback;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Synth *synth;
    Synth *bass;
    Mixer *mixer;
    Playback playback;
    SDL_TimerID timer;
    int *song;
    int song_length;
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
    instance->synth->master_level = 1.2;
    instance->synth->use_echo = true;

    instance->synth->voice_vca_settings.attack = 0.0002;
    instance->synth->voice_vca_settings.decay = 0.01;
    instance->synth->voice_vca_settings.sustain = 0.5;
    instance->synth->voice_vca_settings.release = 0.07;

    instance->synth->oscillator_settings[1].waveform = SQUARE;
    instance->synth->combiner_settings.combine_mode = COMB_MULTIPLY;
    instance->synth->combiner_settings.strength_mode = STRENGTH_VCA;
    instance->synth->combiner_settings.oscillator2_strength = 0.7;
    instance->synth->combiner_settings.oscillator2_scale = 0.5;
    instance->synth->modulation_settings.lfo[0].oscillator.waveform = SINE;

    instance->synth->combiner_settings.detune = 0.0;
    instance->bass = synth_create();
    instance->bass->master_level = 1.0;
    instance->bass->voice_vca_settings.attack = 0.0;
    instance->bass->voice_vca_settings.decay = 0.02;
    instance->bass->voice_vca_settings.sustain = 0.4 ;
    instance->bass->voice_vca_settings.release = 0.065;
    instance->bass->combiner_settings.oscillator2_strength = 0.5;
    instance->bass->combiner_settings.oscillator2_scale = 0.5;
    instance->bass->combiner_settings.combine_mode = COMB_ADD;
    instance->bass->combiner_settings.strength_mode = STRENGTH_VCA;
    instance->bass->combiner_vca_settings.attack = 1.0;
    instance->bass->combiner_vca_settings.decay = 1.0;
    instance->bass->combiner_vca_settings.sustain = 0.5;
    instance->bass->combiner_vca_settings.release = 0.1;
    instance->bass->combiner_settings.detune = 0.1;

    /*instance->bass->combiner_settings.combine_mode = COMB_MODULATE;
    instance->bass->combiner_settings.strength_mode = STRENGTH_MANUAL;
    instance->bass->combiner_settings.oscillator2_strength = 1.0;
    instance->bass->combiner_settings.oscillator2_scale = 4.0;
    */

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
        1024,
        768, 0);
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
    SDL_RendererInfo renderer_info;
    SDL_GetRendererInfo(instance->renderer, &renderer_info);
    printf("Renderer name: %s\n", renderer_info.name);
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
    int octave = 4;
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
            instance->synth->oscillator_settings[0].waveform = instance->waveform;
            instance->synth->oscillator_settings[1].waveform = instance->waveform;
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

Uint32 play_song_callback(Uint32 interval, void *param) {
    Instance *instance = (Instance*)param;
    if (instance->playback.substep == 0) {
        int note = instance->song[instance->playback.song_pos];
        note += 36;
        if (note > 0) {
            synth_note_on(instance->bass, note);
            instance->playback.last_note = note;
        }
        instance->playback.song_pos = (instance->playback.song_pos + 1) % instance->song_length;
    } else if (instance->playback.last_note > 0) {
        synth_note_off(instance->bass, instance->playback.last_note);
        instance->playback.last_note = 0;
    }
    instance->playback.substep = (instance->playback.substep  + 1) % 2;

    return interval;
}

int main(int argc, char **argv) {
    //SDL_SetHint()
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
    Instance *instance = create_instance();
    if (instance == NULL) {
        return 1;
    }


    for (int i = 0; i < VU_TABLE_SIZE; i++) {
        //logTable[i] = 100+20*log10(((double)i+1)/LOG_TABLE_SIZE);
        vu_table[i]  = sqrt((double)i/VU_TABLE_SIZE) * 128.0;
    }

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
    int bassline[] = {
        2,14,2,2,14,2,2,14,
        2,14,2,2,14,2,26,14,
        2,14,2,2,14,2,2,14,
        2,14,2,2,14,2,26,14,
        5,17,5,5,17,5,5,17,
        5,17,5,5,17,5,29,17,
        5,17,5,5,17,5,5,17,
        5,17,5,5,17,5,29,31
    };
    instance->song = bassline;
    instance->song_length = sizeof(bassline)/sizeof(int);
    instance->timer = SDL_AddTimer(60, play_song_callback, instance);
    while (run) {
        if (SDL_PollEvent(&event)) {
            run = handle_event(instance, &event);
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);
        draw(instance);
        SDL_RenderPresent(instance->renderer);
        SDL_Delay(1);
    }
    SDL_RemoveTimer(instance->timer);
    destroy_instance(instance);

    return 0;
}

