#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mixer.h"
#include "ui_synth.h"
#include "font.h"

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
    Synth *current_synth;
    Mixer *mixer;
    UiSynth *ui_synth;
    Playback playback;
    SDL_TimerID timer;
    int *song;
    int song_length;
    int waveform;
    bool mouse_down;
} Instance;

int scanCodeToNote[512];
#define SCRW 1024
#define SCRH 768

#define SYNTH_XPOS (SCRW-UI_SYNTH_W-256)
#define SYNTH_YPOS 0

#define VU_TABLE_SIZE 100000
int vu_table[VU_TABLE_SIZE];

void destroy_instance(Instance *instance) {
    if (instance == NULL) {
        return;
    }
    font_done();
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
    if (instance->ui_synth != NULL) {
        ui_synth_destroy(instance->ui_synth);
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

//    instance->synth->voice_vca_settings.attack = 0.0002;
    instance->synth->voice_vca_settings.attack = 0.01;
    instance->synth->voice_vca_settings.decay = 0.01;
    instance->synth->voice_vca_settings.sustain = 0.5;
    instance->synth->voice_vca_settings.release = 0.01;

    instance->synth->oscillator_settings[1].waveform = SQUARE;
    instance->synth->combiner_settings.combine_mode = COMB_MULTIPLY;
    instance->synth->combiner_settings.strength_mode = STRENGTH_VCA;
    instance->synth->combiner_settings.oscillator2_strength = 0.7;
    instance->synth->modulation_settings.lfo[0].oscillator.waveform = SINE;
    instance->synth->echo.echo_time = 0.5;

    instance->synth->combiner_settings.detune = 0.0;
    instance->bass = synth_create();
    instance->bass->master_level = 1.0;
    instance->bass->voice_vca_settings.attack = 0.0;
    instance->bass->voice_vca_settings.decay = 0.01;
    instance->bass->voice_vca_settings.sustain = 0.4 ;
    instance->bass->voice_vca_settings.release = 0.01;
    instance->bass->combiner_settings.oscillator2_strength = 0.5;
    instance->bass->combiner_settings.combine_mode = COMB_ADD;
    instance->bass->combiner_settings.strength_mode = STRENGTH_VCA;
    instance->bass->combiner_vca_settings.attack = 0.1;
    instance->bass->combiner_vca_settings.decay = 0.1;
    instance->bass->combiner_vca_settings.sustain = 0.5;
    instance->bass->combiner_vca_settings.release = 0.05;
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
        SCRW,
        SCRH, 0);
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

    if (!font_init()) {
        destroy_instance(instance);
        return NULL;
    }

    instance->ui_synth = ui_synth_create(instance->renderer);
    if (instance->ui_synth == NULL) {
        destroy_instance(instance);
        return NULL;
    }

    return instance;
}

void draw_vu(Instance *instance, int xo, int yo) {
    int lastHeight = 0;
    int half_height = 128;
    int x_offset = 0;
    int width = instance->mixer->tap_size/2;
    SDL_Rect rect = {
        .x=xo+x_offset,
        .y=yo,
        .w=width,
        .h=half_height*2
    };
    SDL_SetRenderDrawColor(instance->renderer, 31,31,31,255);
    SDL_RenderFillRect(instance->renderer, &rect);
    SDL_SetRenderDrawColor(instance->renderer, 255,255,255,255);
    SDL_RenderDrawLine(instance->renderer,xo+x_offset,yo+half_height,xo+x_offset+width,yo+half_height);
    for (int i = 0; i < width; i++) {
        float v = fmax(instance->mixer->left_tap[i*2], instance->mixer->left_tap[i*2+1]);
        int height = vu_table[(int)fabs(v*VU_TABLE_SIZE)];
        if (v < 0) {
            height = -height;
        }
        SDL_RenderDrawLine(instance->renderer,xo+i, yo+half_height+lastHeight,xo+i+1,yo+half_height+height);
        lastHeight = height;
    }

}

void handle_mouse_down(Instance *instance, int mx, int my) {
    if (mx > SYNTH_XPOS && mx < SYNTH_XPOS + UI_SYNTH_W &&
        my > SYNTH_YPOS && my < SYNTH_YPOS + UI_SYNTH_H) {
        ui_synth_click(instance->ui_synth, instance->current_synth, mx-SYNTH_XPOS, my-SYNTH_YPOS);
    }
}

bool handle_event(Instance *instance, SDL_Event *event) {
    SDL_Scancode sc;
    SDL_Keycode sym;
    int octave = 4;
    bool run = true;

    SDL_KeyboardEvent key = event->key;
    SDL_Keymod keymod = SDL_GetModState();
    bool shift = (keymod & KMOD_LSHIFT) || (keymod & KMOD_RSHIFT);

    switch (event->type) {
    case SDL_MOUSEBUTTONUP:
        instance->mouse_down = false;
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event->button.button == 1) {
            instance->mouse_down = true;
            handle_mouse_down(instance, event->button.x, event->button.y);
        }
        break;
    case SDL_MOUSEMOTION:
        if (instance->mouse_down) {
            handle_mouse_down(instance, event->motion.x, event->motion.y);
        }
        break;
    case SDL_KEYDOWN:
        sc = key.keysym.scancode;
        sym = key.keysym.sym;
        if (sc == SDL_SCANCODE_ESCAPE) {
            run = false;
        } else if (sc == SDL_SCANCODE_SPACE) {
            instance->waveform = (instance->waveform + 1) % 4;
            printf("Set %d\n", instance->waveform);
            instance->synth->oscillator_settings[0].waveform = instance->waveform;
            instance->synth->oscillator_settings[1].waveform = instance->waveform;
        } else if (sc == SDL_SCANCODE_UP) {
            ui_synth_alter_parameter(instance->ui_synth, instance->current_synth, shift ? 0.01 : 0.1);
        } else if (sc == SDL_SCANCODE_DOWN) {
            ui_synth_alter_parameter(instance->ui_synth, instance->current_synth, shift ? -0.01 : -0.1);
        } else if (sc == SDL_SCANCODE_LEFT) {
            ui_synth_prev_parameter(instance->ui_synth);
        } else if (sc == SDL_SCANCODE_RIGHT) {
            ui_synth_next_parameter(instance->ui_synth);
        } else if (sc == SDL_SCANCODE_F1) {
            instance->current_synth = instance->bass;
        } else if (sc == SDL_SCANCODE_F2) {
            instance->current_synth = instance->synth;
        }
//        printf("sc %d sym %d\n", sc, sym);
        // From here on don't allow key repeat
        if (key.repeat > 0) {
            break;
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

void init_scan_codes() {

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

    init_scan_codes();

    bool run = true;
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
    instance->current_synth = instance->bass;
    instance->song = bassline;
    instance->song_length = sizeof(bassline)/sizeof(int);
    mixer_start(instance->mixer);
    instance->timer = SDL_AddTimer(60, play_song_callback, instance);
    while (run) {
        while (run && SDL_PollEvent(&event)) {
            run = handle_event(instance, &event);
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);

        draw_vu(instance,SCRW-256,0);
        ui_synth_render(instance->ui_synth, instance->current_synth, SYNTH_XPOS, SYNTH_YPOS);
        SDL_RenderPresent(instance->renderer);
        SDL_Delay(1);
    }
    SDL_RemoveTimer(instance->timer);
    destroy_instance(instance);

    return 0;
}

