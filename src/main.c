#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "mixer.h"
#include "font.h"
#include "song.h"
#include "vca.h"
#include "player.h"
#include "rack.h"

#include "ui_instrument.h"
#include "ui_track.h"
#include "ui_trackpos.h"
#include "ui_pattern.h"

typedef enum {
    EDIT_SYNTH,
    EDIT_TRACK
} EditorState;
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Rack *rack;
    UiInstrument *ui_instrument;
    UiTrack *ui_track;
    UiTrackPos *ui_trackpos;
    Player player;
    SDL_TimerID timer;
    Song song;
    EditorState editor_state;
    bool mouse_down;
    bool playing;
    int current_track;
    int octave;
    Sint8 current_instrument;
    Sint8 track_pos;
    Sint8 step;
} Instance;

int scanCodeToNote[512];
#define SCRW 1024
#define SCRH 768

#define INSTR_XPOS (SCRW-UI_INSTR_W-256)
#define INSTR_YPOS 0

#define VU_TABLE_SIZE 100000
int vu_table[VU_TABLE_SIZE];

void destroy_instance(Instance *instance) {
    if (instance == NULL) {
        return;
    }
    font_done();
    if (instance->rack != NULL) {
        rack_destroy(instance->rack);
    }
    if (instance->ui_instrument != NULL) {
        ui_instrument_destroy(instance->ui_instrument);
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

    instance->editor_state = EDIT_TRACK;
    instance->step = 1;
    instance->rack = rack_create();
    if (instance->rack == NULL) {
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

    instance->ui_instrument = ui_instrument_create(instance->renderer);
    if (instance->ui_instrument == NULL) {
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
    int width = instance->rack->mixer->tap_size/2;
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
        float v = fmax(instance->rack->mixer->left_tap[i*2], instance->rack->mixer->left_tap[i*2+1]);
        int height = vu_table[(int)fabs(v*VU_TABLE_SIZE)];
        if (v < 0) {
            height = -height;
        }
        SDL_RenderDrawLine(instance->renderer,xo+i, yo+half_height+lastHeight,xo+i+1,yo+half_height+height);
        lastHeight = height;
    }

}

void handle_mouse_down(Instance *instance, int mx, int my) {
    if (mx > INSTR_XPOS && mx < INSTR_XPOS + UI_INSTR_W &&
        my > INSTR_YPOS && my < INSTR_YPOS + UI_INSTR_H) {
        ui_instrument_click(instance->ui_instrument, &instance->rack->instruments[instance->current_instrument],
            mx-INSTR_XPOS, my-INSTR_YPOS);
    }
}

void handle_synth_edit_event(Instance *instance, SDL_Event *event) {
    SDL_Scancode sc;
    SDL_Keycode sym;
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
        if (sc == SDL_SCANCODE_UP) {
            ui_instrument_alter_parameter(instance->ui_instrument, &instance->rack->instruments[instance->current_instrument], shift ? 0.01 : 0.1);
        } else if (sc == SDL_SCANCODE_DOWN) {
            ui_instrument_alter_parameter(instance->ui_instrument, &instance->rack->instruments[instance->current_instrument], shift ? -0.01 : -0.1);
        } else if (sc == SDL_SCANCODE_LEFT) {
            ui_instrument_prev_parameter(instance->ui_instrument);
        } else if (sc == SDL_SCANCODE_RIGHT) {
            ui_instrument_next_parameter(instance->ui_instrument);
        }

        break;
    }
}

void modify_pattern_pos(Instance *instance, int delta) {
    int pos = instance->player.pattern_pos;
    pos += delta;

    if (pos < 0) {
        pos = 0;
    }

    if (pos >= NOTES_PER_TRACK) {
        pos = NOTES_PER_TRACK-1;
    }

    instance->player.pattern_pos = pos;
}

void set_note(Track *track, Uint8 position, Uint8 pitch, Uint8 velocity, Uint8 instrument) {
    track->note[position].pitch = pitch;
    track->note[position].velocity = velocity;
    track->note[position].instrument = instrument;
}

void handle_track_edit_event(Instance *instance, SDL_Event *event) {
    SDL_KeyboardEvent key = event->key;
    SDL_Keymod keymod = SDL_GetModState();
    SDL_Scancode sc;
    bool shift = (keymod & KMOD_LSHIFT) || (keymod & KMOD_RSHIFT);

    switch (event->type) {
    case SDL_KEYDOWN:
        sc = key.keysym.scancode;
        if (sc == SDL_SCANCODE_UP) {
            modify_pattern_pos(instance, -1);
        } else if (sc == SDL_SCANCODE_DOWN) {
            modify_pattern_pos(instance, 1);
        } else if (sc == SDL_SCANCODE_HOME) {
            instance->player.pattern_pos = 0;
        } else if (sc == SDL_SCANCODE_END) {
            instance->player.pattern_pos = NOTES_PER_TRACK-1;
        } else if (sc == SDL_SCANCODE_PAGEUP) {
            modify_pattern_pos(instance, -16);
        } else if (sc == SDL_SCANCODE_PAGEDOWN) {
            modify_pattern_pos(instance, 16);
        } else if (sc == SDL_SCANCODE_LEFT) {
            instance->track_pos--;
            if (instance->track_pos < 0) {
                if (--instance->current_track < 0) {
                    instance->current_track = 0;
                    instance->track_pos = 0;
                } else {
                    instance->track_pos = 1;
                }
            }
        } else if (sc == SDL_SCANCODE_RIGHT) {
            instance->track_pos++;
            if (instance->track_pos > 1) {
                if (++instance->current_track >= TRACKS_PER_PATTERN) {
                    instance->current_track = TRACKS_PER_PATTERN - 1;
                    instance->track_pos = 1;
                } else {
                    instance->track_pos = 0;
                }
            }
        } else if (sc == SDL_SCANCODE_TAB) {
            if (shift) {
                if (--instance->current_track < 0) {
                    instance->current_track = 0;
                }
            } else {
                if (++instance->current_track >= TRACKS_PER_PATTERN) {
                    instance->current_track = TRACKS_PER_PATTERN - 1;
                }
            }
        }
        Track *ct = &instance->song.patterns[0].track[instance->current_track];

        if (instance->track_pos == 0 && scanCodeToNote[sc] != 0) {
            set_note(ct, instance->player.pattern_pos,
                scanCodeToNote[sc] + 12 * instance->octave,
                255,
                instance->current_instrument);
            modify_pattern_pos(instance, instance->step);
        }
        if (instance->track_pos == 0 && sc == SDL_SCANCODE_NONUSBACKSLASH) {
            set_note(ct, instance->player.pattern_pos, 1, 0, 0);
            modify_pattern_pos(instance, instance->step);
        }
        if (sc == SDL_SCANCODE_DELETE) {
            set_note(ct, instance->player.pattern_pos, 0, 0, 0);
            modify_pattern_pos(instance, instance->step);
        }
        if (sc == SDL_SCANCODE_RETURN) {
            modify_pattern_pos(instance, instance->step);
        }


        break;
    }
}

bool handle_event(Instance *instance, SDL_Event *event) {
    SDL_Scancode sc;
    SDL_Keycode sym;
    bool run = true;

    if (instance->editor_state == EDIT_SYNTH) {
        handle_synth_edit_event(instance, event);
    } else if (!instance->playing) {
        handle_track_edit_event(instance, event);
    }

    SDL_KeyboardEvent key = event->key;
//    SDL_Keymod keymod = SDL_GetModState();

    switch (event->type) {
    case SDL_KEYDOWN:
        sc = key.keysym.scancode;
        sym = key.keysym.sym;
        bool shift = (event->key.keysym.mod & KMOD_LSHIFT) || (event->key.keysym.mod & KMOD_RSHIFT);
        if (sc == SDL_SCANCODE_F1) {
            instance->editor_state = EDIT_TRACK;
        } else if (sc == SDL_SCANCODE_F2) {
            instance->editor_state = EDIT_SYNTH;
        } else if (sc == SDL_SCANCODE_F9) {
            instance->current_instrument = 0;
        } else if (sc == SDL_SCANCODE_F10) {
            instance->current_instrument = 1;
        } else if (sc == SDL_SCANCODE_SPACE) {
            player_stop(&instance->player);
            instance->playing = false;
        } else if (sc == SDL_SCANCODE_RCTRL) {
            player_stop(&instance->player);
            instance->player.pattern_pos = 0;
            player_start(&instance->player);
            instance->playing = true;
        } else if (shift && sym == '+') {
            instance->octave++;
            if (instance->octave > 7) {
                instance->octave = 7;
            }
        } else if (shift && sym == '-') {
            instance->octave--;
            if (instance->octave < 0) {
                instance->octave = 0;
            }
        }
//        printf("sc %d sym %d\n", sc, sym);
        // From here on don't allow key repeat
        if (key.repeat > 0) {
            break;
        }
        if (scanCodeToNote[sc] != 0) {
            instrument_note_on(&instance->rack->instruments[instance->current_instrument], scanCodeToNote[sc] + 12 * instance->octave);
        }
        break;
    case SDL_KEYUP:
        sc = key.keysym.scancode;
        if (scanCodeToNote[sc] != 0) {
            instrument_note_off(&instance->rack->instruments[instance->current_instrument], scanCodeToNote[sc] + 12 * instance->octave);
        }
        break;
    case SDL_QUIT:
        run = false;
        break;
    }
    return run;
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
    int blipblop[] = {
        26,1,0,26,1,0,26,1,
        0,26,1,0,26,1,38,26,
        26,1,0,26,1,0,26,1,
        0,26,1,0,26,1,38,26,
        26,1,0,26,1,0,26,1,
        0,26,1,0,26,1,38,26,
        26,1,0,26,1,0,26,1,
        0,26,1,0,26,1,38,26
    };
    for (int i = 0; i < 64; i++) {
        instance->song.patterns[0].track[0].note[i].pitch = bassline[i]+36;
        instance->song.patterns[0].track[0].note[i].instrument = 0;
        int p = blipblop[i];
        if (blipblop[i] > 12) {
            p = p + 36;
        }

        instance->song.patterns[0].track[1].note[i].pitch = p;
        instance->song.patterns[0].track[1].note[i].instrument = 1;
    }
}

void render_pattern(Instance *instance) {
    int pattern_y = SCRH - 2 * UI_PATTERN_VISIBLE_NOTES * UI_PATTERN_ROW_SPACING;
    ui_trackpos_render(instance->ui_trackpos, instance->player.pattern_pos, 4, pattern_y);
    for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
        ui_track_render(instance->ui_track, &instance->song.patterns[0].track[i],
            instance->player.pattern_pos, i == instance->current_track ? instance->track_pos : -1,
            48+2*i*UI_TRACK_W, pattern_y);
    }

    if (instance->editor_state != EDIT_TRACK) {
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,150);
        SDL_Rect dimmer = {
            .x = 0,
            .y = pattern_y,
            .w = SCRW,
            .h = UI_TRACK_H*2
        };
        SDL_RenderFillRect(instance->renderer, &dimmer);
    }
}

void render_status_bar(Instance *instance) {
    char oct[2];
    oct[0] = instance->octave + 48;
    oct[1] = 0;

    SDL_SetRenderDrawColor(instance->renderer, 255,255,255,255);
    font_write(instance->renderer, oct, SCRW-8, SCRH-8);
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
    mixer_start(instance->rack->mixer);
    memset(&instance->player, 0, sizeof(Player));
    instance->player.song = &instance->song;
    instance->player.rack = instance->rack;
    instance->player.tempo = 30;
    while (run) {
        while (run && SDL_PollEvent(&event)) {
            run = handle_event(instance, &event);
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);

        draw_vu(instance,SCRW-256,0);
        render_pattern(instance);
        if (instance->editor_state == EDIT_SYNTH) {
            ui_instrument_render(instance->ui_instrument, &instance->rack->instruments[instance->current_instrument], INSTR_XPOS, INSTR_YPOS);
        }
        render_status_bar(instance);
        SDL_RenderPresent(instance->renderer);
        SDL_Delay(5);
    }
    player_stop(&instance->player);
    destroy_instance(instance);

    return 0;
}

