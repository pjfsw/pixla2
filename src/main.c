#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "mixer.h"
#include "font.h"
#include "song.h"
#include "vca.h"
#include "player.h"
#include "rack.h"
#include "song_storage.h"
#include "ui_rack.h"
#include "ui_track.h"
#include "ui_trackpos.h"
#include "ui_pattern.h"
#include "lookup_tables.h"
#include "mixer.h"

typedef enum {
    EDIT_RACK,
    EDIT_TRACK
} EditorState;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Rack *rack;
    UiRack *ui_rack;
    UiTrack *ui_track;
    UiTrackPos *ui_trackpos;
    Player player;
    SDL_TimerID timer;
    Song song;
    EditorState editor_state;
    bool playing;
    int current_track;
    Uint8 octave;
    Sint8 track_pos;
    Sint8 step;
    int loud;
    int red_line;
    int peak_color;
} Instance;

Uint8 scanCodeToNote[512];
#define SCRW 1024
#define SCRH 768

#define RACK_XPOS 0
#define RACK_YPOS 0

#define VU_HEIGHT 100
#define VOLTAGE_TABLE_SIZE 256
#define DBFS_TABLE_SIZE 4096

int voltage_table[VOLTAGE_TABLE_SIZE];
int dbfs_table[DBFS_TABLE_SIZE];

void destroy_instance(Instance *instance) {
    if (instance == NULL) {
        return;
    }
    font_done();
    if (instance->rack != NULL) {
        rack_destroy(instance->rack);
    }
    if (instance->ui_rack != NULL) {
        ui_rack_destroy(instance->ui_rack);
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
    lookup_tables_init();
    vca_init_static();

    Instance *instance = calloc(1, sizeof(Instance));

    instance->editor_state = EDIT_TRACK;
    instance->step = 1;
    instance->octave = 2;
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
        SCRH, SDL_WINDOW_MOUSE_FOCUS);
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

    instance->ui_rack = ui_rack_create(instance->renderer);
    if (instance->ui_rack == NULL) {
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

void load_song(Instance *instance) {
    if (song_storage_load("song.px2", &instance->song)) {
        for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
            memcpy(
                &instance->rack->instruments[i].synth->settings,
                &instance->song.synth_settings[i],
                sizeof(SynthSettings)
            );
        }
        memcpy(
            &instance->rack->mixer->settings, &instance->song.mixer_settings, sizeof(MixerSettings));
    }

}

void save_song(Instance *instance) {
    printf("Saving song..");
    rename("song.px2", "song.px2.bak");
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        memcpy(
            &instance->song.synth_settings[i],
            &instance->rack->instruments[i].synth->settings,
            sizeof(SynthSettings)
        );
    }
    memcpy(
        &instance->song.mixer_settings,
        &instance->rack->mixer->settings,
        sizeof(MixerSettings));

    if (song_storage_save("song.px2", &instance->song)) {
        printf("success!\n");
    } else {
        printf("failed!\n");
    }
}

void draw_vu(Instance *instance, int xo, int yo) {
    int lastHeight = 0;
    int half_height = VU_HEIGHT/2;
    int x_offset = 0;
    int width = 128;
    SDL_Rect rect = {
        .x=xo+x_offset,
        .y=yo,
        .w=width,
        .h=half_height*2
    };
    SDL_SetRenderDrawBlendMode(instance->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(instance->renderer, 31,31,31,255);
    SDL_RenderFillRect(instance->renderer, &rect);
    int loud = voltage_table[(int)fabsf(instance->rack->mixer->loudness*VOLTAGE_TABLE_SIZE)];
    SDL_SetRenderDrawColor(instance->renderer,255,255,255,32);
    rect.y=yo+half_height-loud;
    rect.h = loud*2;
    SDL_RenderFillRect(instance->renderer, &rect);

    SDL_SetRenderDrawColor(instance->renderer, 255,255,255,255);
    SDL_RenderDrawLine(instance->renderer,xo+x_offset,yo+half_height,xo+x_offset+width,yo+half_height);

    for (int i = 0; i < width; i++) {
        float v = instance->rack->mixer->left_tap[i * instance->rack->mixer->tap_size / width];
        if (fabsf(v) > MIXER_THRESHOLD) {
            instance->peak_color = 254;
        }
        int height = voltage_table[(int)fabsf(v*VOLTAGE_TABLE_SIZE)];
        if (v < 0) {
            height = -height;
        }
        SDL_RenderDrawLine(instance->renderer,xo+i, yo+half_height+lastHeight,xo+i+1,yo+half_height+height);
        lastHeight = height;
    }
    SDL_SetRenderDrawColor(instance->renderer,255,0,0,instance->peak_color);
    SDL_RenderDrawLine(instance->renderer,xo+x_offset,yo+half_height-instance->red_line,xo+x_offset+width,yo+half_height-instance->red_line);
    SDL_RenderDrawLine(instance->renderer,xo+x_offset,yo+half_height+instance->red_line,xo+x_offset+width,yo+half_height+instance->red_line);

    rect.y = yo + VU_HEIGHT;
    rect.h = 8;
    int currentloud = dbfs_table[(int)(instance->rack->mixer->loudness * DBFS_TABLE_SIZE)];
    if (currentloud*2 > instance->loud) {
        instance->loud = currentloud*2;
    } else if (instance->loud > 0) {
        instance->loud--;
    }
    int loud_dbfs = instance->loud/2 - 64;
    rect.w = 2;
    int opacity = 255;
    int r = 0;
    int g = 255;
    for (int i = 0; i < 32; i++) {
        if (i > loud_dbfs) {
            opacity = 32;
        }
        if (i > 25) {
            r = 255;
            g = 0;
        }
        rect.x=xo + i*4;

        SDL_SetRenderDrawColor(instance->renderer,r,g,0,opacity);
        SDL_RenderFillRect(instance->renderer, &rect);
    }

    if (instance->peak_color > 0) {
        instance->peak_color-=2;
    }
}

void handle_mouse_down(Instance *instance, int mx, int my) {
    if (mx > RACK_XPOS && mx < RACK_XPOS + UI_RACK_W &&
        my > RACK_YPOS && my < RACK_YPOS + UI_RACK_H) {
        ui_rack_click(instance->ui_rack, instance->rack,
            mx-RACK_XPOS, my-RACK_YPOS);
    }
}

void handle_synth_edit_event(Instance *instance, SDL_Event *event) {
    SDL_Scancode sc;
    SDL_Keycode sym;
    SDL_KeyboardEvent key = event->key;
    SDL_Keymod keymod = SDL_GetModState();
    bool shift = (keymod & KMOD_LSHIFT) || (keymod & KMOD_RSHIFT);


    switch (event->type) {
    case SDL_MOUSEBUTTONDOWN:
        if (event->button.button > 0 || event->button.state > 0) {
            handle_mouse_down(instance, event->button.x, event->button.y);
        }
        break;
    case SDL_MOUSEMOTION:
        if (event->motion.state > 0) {
            handle_mouse_down(instance, event->motion.x, event->motion.y);
        }
        break;
    case SDL_KEYDOWN:
        sc = key.keysym.scancode;
        sym = key.keysym.sym;
        if (sc == SDL_SCANCODE_UP) {
            ui_rack_alter_parameter(instance->ui_rack, instance->rack, shift ? 1 : 16);
        } else if (sc == SDL_SCANCODE_DOWN) {
            ui_rack_alter_parameter(instance->ui_rack, instance->rack, shift ? -1 : -16);
        } else if (sc == SDL_SCANCODE_LEFT) {
            ui_rack_prev_parameter(instance->ui_rack);
        } else if (sc == SDL_SCANCODE_RIGHT) {
            ui_rack_next_parameter(instance->ui_rack);
        }

        break;
    }
}

void modify_pattern_pos(Instance *instance, int delta) {
    Uint8 pos = instance->player.pattern_pos;
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
                instance->ui_rack->current_instrument);
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

    if (event->type == SDL_KEYDOWN) {
        bool ctrl = (event->key.keysym.mod & KMOD_CTRL) != 0;

        if (ctrl && event->key.keysym.scancode == SDL_SCANCODE_S) {
            save_song(instance);
            return true;
        }
    }

    if (instance->editor_state == EDIT_RACK) {
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
        bool shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
        if (sc == SDL_SCANCODE_ESCAPE) {
            rack_all_off(instance->rack);
        } else if (sc == SDL_SCANCODE_F1) {
            instance->editor_state = EDIT_TRACK;
        } else if (sc == SDL_SCANCODE_F2) {
            instance->editor_state = EDIT_RACK;
            ui_rack_set_mode(instance->ui_rack, UI_RACK_INSTRUMENT);
        } else if (sc == SDL_SCANCODE_F3) {
            instance->editor_state = EDIT_RACK;
            ui_rack_set_mode(instance->ui_rack, UI_RACK_MIXER);
        } else if (sc == SDL_SCANCODE_F9) {
            ui_rack_prev_instrument(instance->ui_rack);
        } else if (sc == SDL_SCANCODE_F10) {
            ui_rack_next_instrument(instance->ui_rack);
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
            instrument_note_on(&instance->rack->instruments[instance->ui_rack->current_instrument], scanCodeToNote[sc] + 12 * instance->octave);
        }
        break;
    case SDL_KEYUP:
        sc = key.keysym.scancode;
        if (scanCodeToNote[sc] != 0) {
            instrument_note_off(&instance->rack->instruments[instance->ui_rack->current_instrument], scanCodeToNote[sc] + 12 * instance->octave);
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
    oct[0] = (char)(instance->octave + 48);
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

    song_storage_init();


    for (int i = 0; i < VOLTAGE_TABLE_SIZE; i++) {
        voltage_table[i] = (double)i*0.5*VU_HEIGHT/VOLTAGE_TABLE_SIZE;
    }
    for (int i = 0; i < DBFS_TABLE_SIZE; i++) {
        double dbfs = 20 * log10((double)(i+1)/DBFS_TABLE_SIZE) + 96.0;
        dbfs_table[i] = dbfs;
        //printf("Index %d, dbfs %f\n", i, dbfs);
    }
    dbfs_table[0] = 0;

    instance->red_line = voltage_table[(int)(MIXER_CLIPPING * VOLTAGE_TABLE_SIZE)];
    init_scan_codes();
//    init_tmp_song(instance);
    load_song(instance);

    bool run = true;
    SDL_Event event;
    mixer_start(instance->rack->mixer);
    memset(&instance->player, 0, sizeof(Player));
    instance->player.song = &instance->song;
    instance->player.rack = instance->rack;
    instance->player.tempo = 120;
    while (run) {
        while (run && SDL_PollEvent(&event)) {
            run = handle_event(instance, &event);
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);

        draw_vu(instance,0, 248);
        render_pattern(instance);
        if (instance->editor_state == EDIT_RACK) {
            ui_rack_render(instance->ui_rack, instance->rack, RACK_XPOS, RACK_YPOS);
        }
        render_status_bar(instance);
        SDL_RenderPresent(instance->renderer);
        SDL_Delay(1);
    }
    player_stop(&instance->player);
    destroy_instance(instance);

    return 0;
}

