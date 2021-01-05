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
#include "ui_colors.h"
#include "lookup_tables.h"
#include "ui_boundary.h"
#include "wave_tables.h"
#include "midi_notes.h"
#include "song_exporter.h"

typedef enum {
    NOT_SELECTING,
    SELECTING,
    SELECT_PENDING
} SelectionState;

typedef struct {
    int first_track;
    int last_track;
    int first_row;
    int last_row;
    SelectionState state;
} Selection;

typedef struct {
    Pattern pattern;
    Selection selection;
} Clipboard;

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
    bool edit_mode;
    int current_track;
    Uint8 octave;
    Sint8 track_pos;
    Sint8 step;
    int loud;
    int red_line;
    int peak_color;
    Clipboard clipboard;
    Selection selection;
} Instance;

Uint8 scanCodeToNote[512];
Uint8 keyboard_voice[512];

#define RACK_XPOS 0
#define RACK_YPOS 0

#define VU_HEIGHT 100
#define VOLTAGE_TABLE_SIZE 256
#define DBFS_TABLE_SIZE 4096

#define UI_SEQ_ROW_SPACING 20

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

int get_current_pattern_id(Player *player) {
    return player->song->arrangement[player->song_pos].pattern;
}

Pattern *get_current_pattern(Instance *instance) {
    return &instance->song.patterns[instance->song.arrangement[instance->player.song_pos].pattern];
}

Instance *create_instance() {
    midi_notes_init();
    lookup_tables_init();
    vca_init_static();
    wave_tables_init();

    Instance *instance = calloc(1, sizeof(Instance));

    instance->edit_mode = false;
    instance->song.length = 2;
    instance->step = 1;
    instance->octave = 2;
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        synth_settings_set_default(&instance->song.synth_settings[i]);
    }
    instance->rack = rack_create(player_trigger, &instance->player, instance->song.synth_settings);
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
        memcpy(
            &instance->rack->mixer->settings, &instance->song.mixer_settings, sizeof(MixerSettings));
    }

}

void copy_to_song(Instance *instance) {
    memcpy(
        &instance->song.mixer_settings,
        &instance->rack->mixer->settings,
        sizeof(MixerSettings));
}

void export_song(Instance *instance) {
    printf("Exporting song to output.wav\n");
    copy_to_song(instance); // TODO use song directly in tracker
    song_exporter_export(
        "output.wav",
        &instance->song,
        instance->rack->mixer,
        instance->rack->audio_library
        );
}

void save_song(Instance *instance) {
    printf("Saving song..");
    rename("song.px2", "song.px2.bak");
    copy_to_song(instance);

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

void handle_rack_edit_event(Instance *instance, SDL_Event *event) {
    SDL_Scancode sc;
    SDL_Keycode sym;
    SDL_KeyboardEvent key = event->key;
    SDL_Keymod keymod = SDL_GetModState();
    bool shift = (keymod & KMOD_SHIFT) != 0;

    switch (event->type) {
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

void copy_arrangement(Instance *instance, int to, int from) {
    if (to < 0 || from < 0 || to >= PATTERNS_PER_SONG || from >= PATTERNS_PER_SONG) {
        fprintf(stderr, "Arrangement out of bounds (to/from) %d/%d\n", to, from);
        return;
    }
    memcpy(&instance->song.arrangement[to],
        &instance->song.arrangement[from],
        sizeof(Arrangement)
    );
}

bool is_option_meta_key(SDL_Event *event) {
    // TODO check windows? (CMD on Mac, CTRL on Win)
    return (event->key.keysym.mod & KMOD_GUI) != 0;
}

void handle_sequencer_edit_event(Instance *instance, SDL_Event *event) {
    Arrangement *arr = &instance->song.arrangement[instance->player.song_pos];
    bool option = is_option_meta_key(event);
    bool alt = event->key.keysym.mod & KMOD_ALT;
    SDL_Scancode sc = event->key.keysym.scancode;

    switch (event->type) {
    case SDL_KEYDOWN:

        if (instance->player.playing) {
            return; // don't support sequence editing during playback
        }
        // EDITING
        if (option && sc == SDL_SCANCODE_C) {
            printf("Copy\n");
        }

        if (alt && event->key.keysym.scancode == SDL_SCANCODE_LEFT) {
            if (arr->pattern > 0) {
                arr->pattern--;
            }
        }
        if (alt && event->key.keysym.scancode == SDL_SCANCODE_RIGHT) {
            if (arr->pattern < PATTERNS_PER_SONG-1) {
                arr->pattern++;
            }
        }
        if (event->key.keysym.scancode == SDL_SCANCODE_INSERT) {
            if (instance->song.length < PATTERNS_PER_SONG) {
                for (int i = PATTERNS_PER_SONG-1; i > instance->player.song_pos; i--) {
                    copy_arrangement(instance, i, i-1);
                }
                instance->song.length++;
            }
        }
        if (event->key.keysym.scancode == SDL_SCANCODE_DELETE) {
            if (instance->song.length > 1 && instance->player.song_pos < instance->song.length-1) {
                for (int i = instance->player.song_pos; i < PATTERNS_PER_SONG-1; i++) {
                    copy_arrangement(instance, i, i+1);
                }
                instance->song.length--;
            }
        }
        if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
            if (instance->song.length > 1 && instance->player.song_pos > 0) {
                for (int i = instance->player.song_pos; i < PATTERNS_PER_SONG; i++) {
                    copy_arrangement(instance, i-1, i);
                }
                instance->player.song_pos--;
                instance->song.length--;
            }
        }
    }
}

void update_selection(Instance *instance, int old_pos, int new_pos, int old_track) {
    if (instance->selection.state == NOT_SELECTING) {
        instance->selection.first_row = old_pos;
        instance->selection.first_track = old_track;
        instance->selection.state = SELECTING;
    }
    instance->selection.last_row = new_pos;
    instance->selection.last_track = instance->current_track;
}

void modify_pattern_pos(Instance *instance, int delta, bool move_selection) {
    int old_pos = instance->player.pattern_pos;
    int pos = old_pos;

    pos += delta;

    if (pos < 0) {
        pos = 0;
    }

    if (pos >= NOTES_PER_TRACK) {
        pos = NOTES_PER_TRACK-1;
    }
    if (move_selection) {
        update_selection(instance, old_pos, pos, instance->current_track);
    }

    instance->player.pattern_pos = pos;
}

void set_note(Track *track, Uint8 position, Uint8 pitch, Uint8 velocity, Uint8 instrument) {
    track->note[position].pitch = pitch;
    track->note[position].velocity = velocity;
    track->note[position].instrument = instrument;
}

void reset_pattern_selection(Instance *instance) {
    instance->selection.state = NOT_SELECTING;
    instance->selection.first_row = -1;
    instance->selection.last_row = -1;
    instance->selection.first_track = -1;
    instance->selection.last_track = -1;
}

void select_all(Instance *instance) {
    instance->selection.state = SELECT_PENDING;
    instance->selection.first_row = 0;
    instance->selection.last_row = NOTES_PER_TRACK-1;
    instance->selection.first_track = 0;
    instance->selection.last_track = TRACKS_PER_PATTERN-1;
}

void select_track(Instance *instance) {
    instance->selection.state = SELECT_PENDING;
    instance->selection.first_row = 0;
    instance->selection.last_row = NOTES_PER_TRACK-1;
    instance->selection.first_track = instance->current_track;
    instance->selection.last_track = instance->current_track;
}

void copy_track_contents(Track *to, Track *from, int first_row, int last_row, int target_row) {
    if (first_row < 0 || last_row < 0) {
        return;
    }
    int rows_to_copy = 1 + last_row - first_row;
    if (target_row + rows_to_copy > NOTES_PER_TRACK) {
        rows_to_copy = NOTES_PER_TRACK - target_row;
    }
    printf("Copy track data, %d-%d\n", first_row, last_row);

    memcpy(&to->note[target_row], &from->note[first_row], sizeof(Note) * rows_to_copy);
}

void fix_selection(Selection *selection) {
    if (selection->last_row < selection->first_row) {
        int tmp = selection->last_row;
        selection->last_row = selection->first_row;
        selection->first_row = tmp;
    }
    if (selection->last_track < selection->first_track) {
        int tmp = selection->last_track;
        selection->last_track = selection->first_track;
        selection->first_track = tmp;
    }
}

void copy_selection(Instance *instance) {
    if (instance->selection.state != NOT_SELECTING) {
        memcpy(&instance->clipboard.pattern, get_current_pattern(instance), sizeof(Pattern));
        memcpy(&instance->clipboard.selection, &instance->selection, sizeof(Selection));
        fix_selection(&instance->clipboard.selection);
    }
}

void cut_selection(Instance *instance) {
    if (instance->selection.state == NOT_SELECTING) {
        return;
    }

    copy_selection(instance);
    reset_pattern_selection(instance);

    Track empty_track;
    memset(&empty_track, 0, sizeof(Track));
    for (int i = instance->clipboard.selection.first_track; i <= instance->clipboard.selection.last_track; i++) {
        copy_track_contents(
            &get_current_pattern(instance)->track[i],
            &empty_track,
            instance->clipboard.selection.first_row,
            instance->clipboard.selection.last_row,
            instance->clipboard.selection.first_row
        );
    }
}
void paste_selection(Instance *instance) {
    Selection *selection = &instance->clipboard.selection;
    if (selection->state == NOT_SELECTING || selection->first_track == -1 || selection->last_track == -1) {
        return;
    }
    for (int i = selection->first_track; i <= selection->last_track; i++) {
        int target_track = i + instance->current_track - selection->first_track;
        if (target_track >= TRACKS_PER_PATTERN) {
            break;
        }
        copy_track_contents(
            &get_current_pattern(instance)->track[target_track],
            &instance->clipboard.pattern.track[i],
            selection->first_row,
            selection->last_row,
            instance->player.pattern_pos
        );
    }

}

void move_previous_track(Instance *instance, bool shift) {
    int old_track = instance->current_track;
    if (instance->track_pos > 0) {
        instance->track_pos = 0;
    } else if (--instance->current_track < 0) {
        instance->current_track = 0;
    }

    if (shift) {
        update_selection(instance, instance->player.pattern_pos, instance->player.pattern_pos, old_track);
    }
}

void move_next_track(Instance *instance, bool shift) {
    int old_track = instance->current_track;

    if (++instance->current_track >= TRACKS_PER_PATTERN) {
        instance->current_track = TRACKS_PER_PATTERN - 1;
    } else {
        instance->track_pos = 0;
    }

    if (shift) {
        update_selection(instance, instance->player.pattern_pos, instance->player.pattern_pos, old_track);
    }
}

void handle_edit_note(Instance *instance, Track *ct, SDL_Scancode sc) {
    if (scanCodeToNote[sc] != 0) {
        reset_pattern_selection(instance);

        set_note(ct, instance->player.pattern_pos,
            scanCodeToNote[sc] + 12 * instance->octave,
            255,
            instance->ui_rack->current_instrument);
        modify_pattern_pos(instance, instance->step, false);
    }
    if (sc == SDL_SCANCODE_NONUSBACKSLASH) {
        reset_pattern_selection(instance);
        set_note(ct, instance->player.pattern_pos, 1, 0, 0);
        modify_pattern_pos(instance, instance->step, false);
    }
    if (sc == SDL_SCANCODE_DELETE) {
        reset_pattern_selection(instance);
        set_note(ct, instance->player.pattern_pos, 0, 0, 0);
        modify_pattern_pos(instance, instance->step, false);
    }
}

int read_digit(SDL_Scancode sc, int upper_bound ) {
    if (sc >= SDL_SCANCODE_1 && sc <= SDL_SCANCODE_9) {
        return sc-SDL_SCANCODE_1+1;
    } else if (sc == SDL_SCANCODE_0) {
        return 0;
    } else if (sc >= SDL_SCANCODE_A && sc <= SDL_SCANCODE_A + upper_bound - 10) {
        return sc-SDL_SCANCODE_A+10;
    } else {
        return -1;
    }
}

void handle_edit_velocity(Instance *instance, Track *ct, SDL_Scancode sc) {
    Note *note = &ct->note[instance->player.pattern_pos];

    if (sc == SDL_SCANCODE_DELETE) {
        reset_pattern_selection(instance);
        if (note->pitch > 1) {
            note->velocity = 0xFF;
        } else {
            note->velocity = 0;
        }
        modify_pattern_pos(instance, instance->step, false);
    }

    int digit = read_digit(sc, 15);
    if (digit < 0) {
        return;
    }
    if (instance->track_pos == 1) {
        note->velocity = (note->velocity & 15) | (digit << 4);
    } else {
        note->velocity = (note->velocity & 240) | digit;
    }
    modify_pattern_pos(instance, instance->step, false);
}

void clear_command_and_advance(Instance *instance, Note *note) {
    reset_pattern_selection(instance);
    note->has_command = false;
    note->command = 0;
    note->parameter_value = 0;
    modify_pattern_pos(instance, instance->step, false);
}

void handle_edit_command(Instance *instance, Track *ct, SDL_Scancode sc) {
    Note *note = &ct->note[instance->player.pattern_pos];

    if (sc == SDL_SCANCODE_DELETE) {
        clear_command_and_advance(instance, note);
        return;
    }

    int digit = read_digit(sc, 31);
    if (digit < 0) {
        return;
    }
    note->command = digit;
    note->has_command = true;
    modify_pattern_pos(instance, instance->step, false);
}

void handle_edit_parameter(Instance *instance, Track *ct, SDL_Scancode sc) {
    Note *note = &ct->note[instance->player.pattern_pos];

    if (sc == SDL_SCANCODE_DELETE) {
        clear_command_and_advance(instance, note);
        return;
    }

    int digit = read_digit(sc, 15);
    if (digit < 0) {
        return;
    }
    if (instance->track_pos == 4) {
        note->parameter_value = (note->parameter_value & 15) | (digit << 4);
    } else {
        note->parameter_value = (note->parameter_value & 240) | digit;
    }
    note->has_command = true;
    modify_pattern_pos(instance, instance->step, false);
}

void handle_track_edit_event(Instance *instance, SDL_Event *event) {
    SDL_KeyboardEvent key = event->key;
    SDL_Keymod keymod = SDL_GetModState();
    SDL_Scancode sc;
    bool shift = (keymod & KMOD_LSHIFT) || (keymod & KMOD_RSHIFT);
    bool option =  is_option_meta_key(event);
    bool selection = option;

    switch (event->type) {
    case SDL_KEYUP:
        sc = key.keysym.scancode;
        if (instance->selection.state == SELECTING && (sc == SDL_SCANCODE_LSHIFT || sc == SDL_SCANCODE_RSHIFT)) {
            instance->selection.state = SELECT_PENDING;
        }
        break;
    case SDL_KEYDOWN:
        sc = key.keysym.scancode;
        if (instance->selection.state == SELECT_PENDING) {
            if (sc == SDL_SCANCODE_LSHIFT || sc == SDL_SCANCODE_RSHIFT) {
                instance->selection.state = SELECTING;
            }
        }
        if (sc == SDL_SCANCODE_ESCAPE) {
            reset_pattern_selection(instance);
        } else if (sc == SDL_SCANCODE_A && option) {
            select_all(instance);
        } else if (sc == SDL_SCANCODE_C && option) {
            copy_selection(instance);
        } else if (sc == SDL_SCANCODE_T && option) {
            select_track(instance);
        } else if (sc == SDL_SCANCODE_X && option) {
            cut_selection(instance);
        } else if (sc == SDL_SCANCODE_V && option) {
            paste_selection(instance);
        } else if (sc == SDL_SCANCODE_UP) {
            modify_pattern_pos(instance, -1, selection);
        } else if (sc == SDL_SCANCODE_DOWN) {
            modify_pattern_pos(instance, 1, selection);
        } else if (sc == SDL_SCANCODE_HOME) {
            if (selection) {
                update_selection(instance, instance->player.pattern_pos, 0, instance->current_track);
            }
            instance->player.pattern_pos = 0;
        } else if (sc == SDL_SCANCODE_END) {
            if (selection) {
                update_selection(instance, instance->player.pattern_pos, NOTES_PER_TRACK-1, instance->current_track);
            }
            instance->player.pattern_pos = NOTES_PER_TRACK-1;
        } else if (sc == SDL_SCANCODE_PAGEUP) {
            modify_pattern_pos(instance, -16, selection);
        } else if (sc == SDL_SCANCODE_PAGEDOWN) {
            modify_pattern_pos(instance, 16, selection);
        } else if (sc == SDL_SCANCODE_LEFT) {
            if (selection) {
                move_previous_track(instance, true);
            } else {
                instance->track_pos--;
                if (instance->track_pos < 0) {
                    if (--instance->current_track < 0) {
                        instance->current_track = 0;
                        instance->track_pos = 0;
                    } else {
                        instance->track_pos = 5;
                    }
                }
            }
        } else if (sc == SDL_SCANCODE_RIGHT) {
            if (selection) {
                move_next_track(instance, true);
            } else {
                instance->track_pos++;
                if (instance->track_pos > 5) {
                    if (++instance->current_track >= TRACKS_PER_PATTERN) {
                        instance->current_track = TRACKS_PER_PATTERN - 1;
                        instance->track_pos = 5;
                    } else {
                        instance->track_pos = 0;
                    }
                }
            }
        } else if (sc == SDL_SCANCODE_TAB) {
            if (shift) {
                move_previous_track(instance, false);
            } else {
                move_next_track(instance, false);
            }
        } else if (sc == SDL_SCANCODE_GRAVE) {
            if (shift) {
                if (instance->step > 0) {
                    instance->step--;
                }
            } else {
                if (instance->step < 8) {
                    instance->step++;
                }
            }
        }
        if (shift || option) {
           return;
        }
        Track *ct = &get_current_pattern(instance)->track[instance->current_track];

        if (sc == SDL_SCANCODE_INSERT) {
            reset_pattern_selection(instance);
            for (int i = NOTES_PER_TRACK-1; i > instance->player.pattern_pos; i--) {
                ct->note[i] = ct->note[i-1];
            }
            memset(&ct->note[instance->player.pattern_pos], 0, sizeof(Note));
        }
        if (sc == SDL_SCANCODE_BACKSPACE) {
            reset_pattern_selection(instance);
            if (instance->player.pattern_pos > 0) {
                for (int i = instance->player.pattern_pos; i < NOTES_PER_TRACK; i++) {
                    ct->note[i-1] = ct->note[i];
                }
                memset(&ct->note[NOTES_PER_TRACK-1], 0, sizeof(Note));
                modify_pattern_pos(instance, -1, false);
            }
        }

        if (instance->track_pos == 0) {
            handle_edit_note(instance, ct, sc);
        } else if (instance->track_pos > 0 && instance->track_pos < 3) {
            handle_edit_velocity(instance, ct, sc);
        } else if (instance->track_pos == 3) {
            handle_edit_command(instance, ct, sc);
        } else if (instance->track_pos < 6) {
            handle_edit_parameter(instance, ct, sc);
        }

        if (sc == SDL_SCANCODE_RETURN) {
            modify_pattern_pos(instance, instance->step, false);
        }

        break;
    }
}

bool handle_event(Instance *instance, SDL_Event *event) {
    SDL_Scancode sc;
    SDL_Keycode sym;
    bool run = true;

    bool option = is_option_meta_key(event);
    bool shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
    bool alt = (event->key.keysym.mod & KMOD_ALT) != 0;

    if (event->type == SDL_KEYDOWN) {
        if (shift) {
            if (event->key.keysym.scancode >= SDL_SCANCODE_F1 && event->key.keysym.scancode <= SDL_SCANCODE_F8) {
                instance->octave = event->key.keysym.scancode - SDL_SCANCODE_F1;
                return true;
            }
        }

        if (option) {
            if (event->key.keysym.scancode == SDL_SCANCODE_S) {
                save_song(instance);
                return true;
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_0) {
                export_song(instance);
                return true;
            }
        }
    }

    if (instance->edit_mode && !alt) {
        handle_track_edit_event(instance, event);
    } else if (instance->ui_rack->mode == UI_RACK_NONE) {
        handle_sequencer_edit_event(instance, event);
    } else if (!alt) {
        handle_rack_edit_event(instance, event);
    }

    SDL_KeyboardEvent key = event->key;
//    SDL_Keymod keymod = SDL_GetModState();

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
        //bool shift = (event->key.keysym.mod & KMOD_SHIFT) != 0;
        if (sc == SDL_SCANCODE_ESCAPE) {
            rack_all_off(instance->rack);
        } else if (sc == SDL_SCANCODE_F5) {
            instance->edit_mode = false;
            ui_rack_set_mode(instance->ui_rack, UI_RACK_NONE);
        } else if (sc == SDL_SCANCODE_F6) {
            instance->edit_mode = false;
            ui_rack_set_mode(instance->ui_rack, UI_RACK_INSTRUMENT);
        } else if (sc == SDL_SCANCODE_F7) {
            instance->edit_mode = false;
            ui_rack_set_mode(instance->ui_rack, UI_RACK_MIXER);
        } else if (sc == SDL_SCANCODE_F9) {
            ui_rack_prev_instrument(instance->ui_rack);
        } else if (sc == SDL_SCANCODE_F10) {
            ui_rack_next_instrument(instance->ui_rack);
        } else if (sc == SDL_SCANCODE_SPACE) {
            if (instance->player.playing) {
                player_stop(&instance->player);
            } else if (instance->edit_mode) {
                instance->edit_mode = false;
                ui_rack_set_mode(instance->ui_rack, UI_RACK_NONE);
            } else {
                instance->edit_mode = true;
                ui_rack_set_mode(instance->ui_rack, UI_RACK_NONE);
            }
        } else if (sc == SDL_SCANCODE_F1) {
            player_stop(&instance->player);
            instance->player.pattern_pos = 0;
            instance->edit_mode = false;
            player_start(&instance->player);
        } else if (alt) {
            if (sc == SDL_SCANCODE_UP) {
                if (instance->player.song_pos > 0) {
                    instance->player.song_pos--;
                }
            } else if (sc  == SDL_SCANCODE_DOWN) {
                if (instance->player.song_pos < instance->song.length - 1) {
                    instance->player.song_pos++;
                }
            } else if (sc == SDL_SCANCODE_HOME) {
                instance->player.song_pos = 0;
            } else if (sc == SDL_SCANCODE_END) {
                instance->player.song_pos = instance->song.length - 1;
            }
        }

        // From here on don't allow key repeat
        if (key.repeat > 0) {
            break;
        }
        if (scanCodeToNote[sc] != 0 && !option && !shift && !alt && (!instance->edit_mode || instance->track_pos == 0)) {
            keyboard_voice[sc] = instrument_note_on(&instance->rack->instruments[instance->ui_rack->current_instrument],
                scanCodeToNote[sc] + 12 * instance->octave,
                255);

        }
        break;
    case SDL_KEYUP:
        sc = key.keysym.scancode;
        if (scanCodeToNote[sc] != 0) {
            instrument_note_off(&instance->rack->instruments[instance->ui_rack->current_instrument], keyboard_voice[sc]);
            keyboard_voice[sc] = 0;
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

void dim(SDL_Renderer *renderer, int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, 0,0,0,150);
    SDL_Rect dimmer = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    SDL_RenderFillRect(renderer, &dimmer);
}

void render_pattern(Instance *instance) {
    int pattern_y = SCRH - 2 * UI_PATTERN_VISIBLE_NOTES * UI_PATTERN_ROW_SPACING;
    ui_trackpos_render(instance->ui_trackpos, instance->player.pattern_pos, 4, pattern_y);
    int first_track = instance->selection.first_track;
    int last_track = instance->selection.last_track;
    if (last_track < first_track) {
        first_track = instance->selection.last_track;
        last_track = instance->selection.first_track;
    }

    for (int i = 0; i < TRACKS_PER_PATTERN; i++) {
        bool is_selected = i >= first_track && i <= last_track;
        ui_track_render(
            instance->ui_track,
            &get_current_pattern(instance)->track[i],
            instance->player.pattern_pos,
            instance->edit_mode && i == instance->current_track ? instance->track_pos : -1,
            i == instance->current_track,
            32+i*UI_TRACK_W, pattern_y,
            is_selected ? instance->selection.first_row : -1,
            is_selected ? instance->selection.last_row : -1
        );
    }

    if (!instance->edit_mode) {
        dim(instance->renderer, 0, pattern_y, SCRW, UI_TRACK_H*2);
    }
}

void render_status_bar(Instance *instance, int x, int y) {
    char oct[10];
    char ins[10];
    char stp[10];
    sprintf(oct, "Oct: %d", instance->octave);
    sprintf(ins, "Ins: %d", instance->ui_rack->current_instrument);
    sprintf(stp, "Stp: %d", instance->step);

    ui_colors_set(instance->renderer, ui_colors_sequencer_status());
    font_write_scale(instance->renderer, oct, x, y, 2);
    font_write_scale(instance->renderer, ins, x, y+UI_SEQ_ROW_SPACING, 2);
    font_write_scale(instance->renderer, stp, x, y+2*UI_SEQ_ROW_SPACING, 2);
}

void render_sequencer(Instance *instance, int x, int y) {
    char seq[10];
    ui_colors_set(instance->renderer, ui_colors_sequencer_highlight());
    SDL_Rect rect = {
        .x = x,
        .y = UI_SEQ_ROW_SPACING * 4 + y-1,
        .w = 48,
        .h = 18
    };
    SDL_RenderFillRect(instance->renderer, &rect);
    ui_colors_set(instance->renderer, ui_colors_sequencer_status());
    int y_pos = y;
    for (int i = -4; i < 4; i++) {
        int pos = instance->player.song_pos + i;
        if (pos >= 0 && pos < instance->song.length) {
            int pattern = instance->song.arrangement[pos].pattern;
            sprintf(seq, "%03X %03X", pos, pattern);
            font_write_scale(instance->renderer, seq, x, y_pos, 2);
        }
        y_pos += UI_SEQ_ROW_SPACING;
    }
    if (instance->edit_mode) {
        dim(instance->renderer, x,y, 600,400);
    }
}

int main(int argc, char **argv) {
    //SDL_SetHint()
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
    Instance *instance = create_instance();
    if (instance == NULL) {
        return 1;
    }

    song_storage_init();

    reset_pattern_selection(instance);

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
    instance->player.instruments = instance->rack->instruments;
    instance->player.tempo = 120;
    while (run) {
        while (run && SDL_PollEvent(&event)) {
            run = handle_event(instance, &event);
        }
        SDL_SetRenderDrawColor(instance->renderer, 0,0,0,0);
        SDL_RenderClear(instance->renderer);

        draw_vu(instance,16, 200);
        render_status_bar(instance, 0, 216 + VU_HEIGHT);
        render_pattern(instance);
        ui_rack_render(instance->ui_rack, instance->rack, RACK_XPOS, RACK_YPOS);
        if (instance->ui_rack->mode == UI_RACK_NONE) {
            render_sequencer(instance, RACK_INSTR_W + 16,0);
        }

        SDL_RenderPresent(instance->renderer);
        SDL_Delay(10);
    }
    player_stop(&instance->player);
    destroy_instance(instance);

    return 0;
}
