#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mixer.h"
#include "font.h"
#include "song.h"
#include "vca.h"

#include "ui_synth.h"
#include "ui_track.h"
#include "ui_trackpos.h"
#include "ui_pattern.h"

typedef struct {
    int last_note;
    int song_pos;
    int substep;
    Uint32 tempo;
} Playback;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Synth *synth;
    Synth *bass;
    Synth *current_synth;
    Mixer *mixer;
    UiSynth *ui_synth;
    UiTrack *ui_track;
    UiTrackPos *ui_trackpos;
    Playback playback;
    SDL_TimerID timer;
    Song song;
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
    if (instance->ui_track != NULL) {
        ui_track_destroy(instance->ui_track);
    }
    if (instance->ui_trackpos != NULL) {
        ui_trackpos_destroy(instance->ui_trackpos);
    }
    if (instance->renderer != NULL) {
        SDL_DestroyRenderer(instance->renderer);
    }
    if (instance->window != NULL) {
        SDL_DestroyWindow(instance->window);
    }
}

Instance *create_instance() {
    vca_init_static();

    Instance *instance = calloc(1, sizeof(Instance));
    instance->synth = synth_create();
    instance->synth->voice_vca_settings.attack = 0.0;
    instance->synth->voice_vca_settings.sustain = 0.5;
    instance->synth->voice_vca_settings.decay = 0.3;
    instance->synth->voice_vca_settings.release = 0.3;
    instance->bass = synth_create();

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

    instance->ui_track = ui_track_create(instance->renderer);
    if (instance->ui_track == NULL) {
        destroy_instance(instance);
        return NULL;
    }

    instance->ui_trackpos = ui_trackpos_create(instance->renderer);
    if (instance->ui_trackpos == NULL) {
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
        if (!instance->mouse_down && event->motion.state != 0) {
            printf("DERPES %d\n", event->motion.state);
        }
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
    Uint32 t1 = SDL_GetTicks();
    if (instance->playback.substep == 0) {
        Uint8 note = instance->song.patterns[0].track[0].note[instance->playback.song_pos].pitch;
        if (note > 0) {
            synth_note_on(instance->bass, note);
            instance->playback.last_note = note;
        }
        instance->playback.song_pos = (instance->playback.song_pos + 1) % NOTES_PER_TRACK;
    } else if (instance->playback.last_note > 0) {
        synth_note_off(instance->bass, instance->playback.last_note);
        instance->playback.last_note = 0;
    }
    instance->playback.substep = (instance->playback.substep  + 1) % 2;
    return instance->playback.tempo - SDL_GetTicks() + t1;
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

void init_tmp_song(Instance *instance) {
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
    for (int i = 0; i < 64; i++) {
        instance->song.patterns[0].track[0].note[i].pitch = bassline[i]+24;
    }
}

void render_pattern(Instance *instance) {
    int pattern_y = SYNTH_YPOS+UI_SYNTH_H+UI_PATTERN_ROW_SPACING;
    ui_trackpos_render(instance->ui_trackpos, instance->playback.song_pos, 4, pattern_y);
    for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
        ui_track_render(instance->ui_track, &instance->song.patterns[0].track[i],
            instance->playback.song_pos, 48+2*i*UI_TRACK_W, pattern_y);
    }
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
    init_tmp_song(instance);

    bool run = true;
    SDL_Event event;
    mixer_start(instance->mixer);
    instance->playback.tempo = 60;
    instance->timer = SDL_AddTimer(instance->playback.tempo, play_song_callback, instance);
    while (run) {
        while (run && SDL_PollEvent(&event)) {
            run = handle_event(instance, &event);
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);

        draw_vu(instance,SCRW-256,0);
        ui_synth_render(instance->ui_synth, instance->current_synth, SYNTH_XPOS, SYNTH_YPOS);
        render_pattern(instance);
        SDL_RenderPresent(instance->renderer);
        SDL_Delay(1);
    }
    SDL_RemoveTimer(instance->timer);
    destroy_instance(instance);

    return 0;
}

