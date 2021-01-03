#include <stdlib.h>
#include "ui_track.h"
#include "font.h"

#define _UI_TRACK_PITCH_W 24
#define _UI_TRACK_PITCH_H 8

void _ui_track_set_main_color(UiTrack *ui) {
    SDL_Color *color = ui_pattern_get_note_color();
    SDL_SetRenderDrawColor(ui->renderer, color->r, color->g, color->b, color->a);
}

void _ui_track_set_note_off_color(UiTrack *ui) {
    SDL_Color *color = ui_pattern_get_note_off_color();
    SDL_SetRenderDrawColor(ui->renderer, color->r, color->g, color->b, color->a);
}


void _ui_track_set_bg_color(UiTrack *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 0,0,0,255);
}


void _ui_track_set_selection_color(UiTrack *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 0,0,191,127);
}

SDL_Texture *_ui_track_create_pitch_texture(UiTrack *ui, int pitch, SDL_Color *color) {
    char *no_note = "---";
    char *note_off = "===";
    char *note_names[] = {
        "C-",
        "C#",
        "D-",
        "D#",
        "E-",
        "F-",
        "F#",
        "G-",
        "G#",
        "A-",
        "A#",
        "B-"
    };

    char *note = calloc(1, 4);
    strcpy(note, note_names[pitch % 12]);
    int octave = pitch/12-1;
    note[2] = octave + 48;
    char *str;
    if (pitch == NO_NOTE) {
        str = no_note;
    } else if (pitch == NOTE_OFF) {
        str = note_off;
    } else {
        str = note;
    }

    SDL_Texture *texture = font_create_texture(ui->renderer, str, color);
    free(note);
    return texture;
}

SDL_Texture *_ui_track_create_instrument_texture(UiTrack *ui, int digit, SDL_Color *color) {
    char *digits[] = {"0","1","2","3","4","5","6","7","8","9"};
    return font_create_texture(ui->renderer, digits[digit], color);
}

SDL_Texture *_ui_track_create_digit_texture(UiTrack *ui, int digit, SDL_Color *set_color) {
    char str[2];
    if (digit < 10) {
        sprintf(str, "%d", digit);
    } else {
        sprintf(str, "%c", 'A' + (digit-10));
    }
    return font_create_texture(ui->renderer, str, set_color);
}


SDL_Texture *_ui_track_create_track_background(UiTrack *ui) {
    int h = (NOTES_PER_TRACK + UI_PATTERN_VISIBLE_NOTES) * UI_PATTERN_ROW_SPACING;
    SDL_Texture *texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_TRACK_W,
        h);
    if (texture == NULL) {
        return NULL;
    }
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(ui->renderer, texture);
    _ui_track_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);

    SDL_Color *color = ui_pattern_get_beat_highlight_color();
    SDL_SetRenderDrawColor(ui->renderer, color->r, color->g, color->b, color->a);
    SDL_Rect rect;
    rect.y = 0;
    rect.x = 0;
    rect.w = 2;
    rect.h = h;

    SDL_RenderFillRect(ui->renderer, &rect);

    rect.w = UI_TRACK_W;
    rect.h = UI_PATTERN_ROW_SPACING;
    for (int i = 0; i < 8; i++) {
        rect.y = (i * 8 + UI_PATTERN_EDIT_NOTE_OFFSET) * UI_PATTERN_ROW_SPACING;
        SDL_RenderFillRect(ui->renderer, &rect);
    }
    SDL_SetRenderTarget(ui->renderer, NULL);
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_NONE);

    return texture;
}

UiTrack *ui_track_create(SDL_Renderer *renderer) {
    UiTrack *ui = calloc(1, sizeof(UiTrack));
    ui->renderer = renderer;
    ui->texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_TRACK_W,
        UI_TRACK_H);
    if (ui->texture == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        ui_track_destroy(ui);
        return NULL;
    }

    ui->cursor_pos_on_screen[0] = 0;
    ui->cursor_pos_on_screen[1] = 48;
    ui->cursor_pos_on_screen[2] = 56;
    ui->cursor_pos_on_screen[3] = 72;
    ui->cursor_pos_on_screen[4] = 80;
    ui->cursor_pos_on_screen[5] = 88;

    for (int i = 0; i < 128; i++) {

        ui->pitch_texture[i] = _ui_track_create_pitch_texture(
            ui,i,
            i == 0 ? ui_pattern_get_note_off_color() : ui_pattern_get_note_color()
                );
        if (ui->pitch_texture[i] == NULL) {
            fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
            ui_track_destroy(ui);
            return NULL;
        }
    }
    for (int i = 0; i < 10; i++) {
        ui->instrument_texture[i] = _ui_track_create_instrument_texture(ui,i, ui_pattern_get_note_color());
        if (ui->instrument_texture[i] == NULL) {
            fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
            ui_track_destroy(ui);
            return NULL;
        }
    }
    ui->no_instrument_texture = font_create_texture(renderer, "-", ui_pattern_get_note_off_color());
    if (ui->no_instrument_texture == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        ui_track_destroy(ui);
        return NULL;
    }
    for (int i = 0; i < 16; i++) {

        if (NULL == (ui->velocity_texture[i] =
            _ui_track_create_digit_texture(ui, i, ui_pattern_get_note_modulation_color()))
            || NULL == (ui->parameter_texture[i] =
                _ui_track_create_digit_texture(ui, i, ui_pattern_get_note_modulation_color()))
        ) {
            fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
            ui_track_destroy(ui);
            return NULL;
        }
    }
    for (int i = 0; i < 32; i++) {
        if (NULL == (ui->command_texture[i] =
            _ui_track_create_digit_texture(ui, i, ui_pattern_get_note_modulation_color()))) {
            fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
            ui_track_destroy(ui);
            return NULL;
        }
    }
    ui->track_background = _ui_track_create_track_background(ui);
    if (ui->track_background == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        ui_track_destroy(ui);
        return NULL;

    }

    ui->target_rect.w = UI_TRACK_W;
    ui->target_rect.h = UI_TRACK_H*2;

    return ui;
}

void ui_track_destroy(UiTrack *ui) {
    if (ui != NULL) {
        if (ui->texture != NULL) {
            SDL_DestroyTexture(ui->texture);
        }
        for (int i = 0; i < 128; i++) {
            if (ui->pitch_texture[i] != NULL) {
                SDL_DestroyTexture(ui->pitch_texture[i]);
            }
        }
        for (int i = 0; i < 10; i++) {
            if (ui->instrument_texture[i] != NULL) {
                SDL_DestroyTexture(ui->instrument_texture[i]);
            }
        }
        if (ui->no_instrument_texture != NULL) {
            SDL_DestroyTexture(ui->no_instrument_texture);
        }
        for (int i = 0; i < 16; i++) {
            if (ui->velocity_texture[i] != NULL) {
                SDL_DestroyTexture(ui->velocity_texture[i]);
            }
            if (ui->parameter_texture[i] != NULL) {
                SDL_DestroyTexture(ui->parameter_texture[i]);
            }
        }
        for (int i = 0; i < 32; i++) {
            if (ui->command_texture[i] != NULL) {
                SDL_DestroyTexture(ui->command_texture[i]);
            }
        }
        if (ui->track_background != NULL) {
            SDL_DestroyTexture(ui->track_background);
        }
        free(ui);
    }
}

void _ui_track_draw_notes(UiTrack *ui, Track *track, int pos, int first_selection, int last_selection) {
    _ui_track_set_main_color(ui);
    SDL_Rect pitch_rect;
    pitch_rect.w = _UI_TRACK_PITCH_W;
    pitch_rect.h = _UI_TRACK_PITCH_H;
    pitch_rect.x = 8;
    SDL_Rect target_rect;
    target_rect.h = 8;
    SDL_Rect sel_rect;
    sel_rect.w = UI_TRACK_W;
    sel_rect.h = UI_PATTERN_ROW_SPACING;
    sel_rect.x = 8;
    for (int i = 0; i < UI_PATTERN_VISIBLE_NOTES; i++) {
        target_rect.w = 8;
        int row = pos + i - UI_PATTERN_EDIT_NOTE_OFFSET;
        if (row < 0 || row >= NOTES_PER_TRACK) {
            continue;
        }
        Note *note = &track->note[row];
        pitch_rect.y = i * UI_PATTERN_ROW_SPACING;
        target_rect.y = i * UI_PATTERN_ROW_SPACING;
        SDL_RenderCopy(ui->renderer, ui->pitch_texture[note->pitch], NULL, &pitch_rect);
        target_rect.x = _UI_TRACK_PITCH_W + pitch_rect.x + 8;
        SDL_Texture *t = note->pitch > 1 ?  ui->instrument_texture[note->instrument] :
            ui->no_instrument_texture;
        SDL_RenderCopy(ui->renderer, t, NULL, &target_rect);
        target_rect.x+=16;
        target_rect.w=8;
        if (note->pitch > 1) {
            SDL_RenderCopy(ui->renderer, ui->velocity_texture[note->velocity >> 4], NULL, &target_rect);
            target_rect.x+=8;
            SDL_RenderCopy(ui->renderer, ui->velocity_texture[note->velocity & 15], NULL, &target_rect);
        } else {
            SDL_RenderCopy(ui->renderer, ui->no_instrument_texture, NULL, &target_rect);
            target_rect.x+=8;
            SDL_RenderCopy(ui->renderer, ui->no_instrument_texture, NULL, &target_rect);
        }
        target_rect.x += 16;
        target_rect.w = 8;
        if (note->has_command) {
            SDL_RenderCopy(ui->renderer, ui->command_texture[note->command & 0x1F], NULL, &target_rect);
            target_rect.x += 8;
            SDL_RenderCopy(ui->renderer, ui->parameter_texture[note->parameter >> 4], NULL, &target_rect);
            target_rect.x += 8;
            SDL_RenderCopy(ui->renderer, ui->parameter_texture[note->parameter & 15], NULL, &target_rect);
        } else {
            SDL_RenderCopy(ui->renderer, ui->no_instrument_texture, NULL, &target_rect);
            target_rect.x += 8;
            SDL_RenderCopy(ui->renderer, ui->no_instrument_texture, NULL, &target_rect);
            target_rect.x += 8;
            SDL_RenderCopy(ui->renderer, ui->no_instrument_texture, NULL, &target_rect);
        }
        if (row >= first_selection && row <= last_selection) {
            _ui_track_set_selection_color(ui);
            sel_rect.y = target_rect.y;
            SDL_RenderFillRect(ui->renderer, &sel_rect);
        }

        //font_write(ui->renderer, "C-4 1 01", 0, i * UI_PATTERN_ROW_SPACING);
    }
}

void _ui_track_draw_background(UiTrack *ui, int pos) {
    SDL_Rect rect = {
        .x = 0,
        .y = pos * UI_PATTERN_ROW_SPACING,
        .w = UI_TRACK_W,
        .h = UI_TRACK_H
    };

    SDL_RenderCopy(ui->renderer, ui->track_background, &rect, NULL);
    rect.x = 2;
    rect.y = UI_PATTERN_ROW_SPACING * UI_PATTERN_EDIT_NOTE_OFFSET-1;
    rect.w = UI_TRACK_W-2;
    rect.h = UI_PATTERN_ROW_SPACING;
    SDL_Color *color = ui_pattern_get_active_row_color();
    SDL_SetRenderDrawColor(ui->renderer, color->r, color->g,color->b,color->a );
    SDL_RenderFillRect(ui->renderer, &rect);

}

void _ui_track_draw_cursor(UiTrack *ui, int cursor_pos, bool edit) {
    SDL_Color *color = edit ? ui_pattern_get_active_cursor_color() : ui_pattern_get_cursor_color();
    SDL_SetRenderDrawColor(ui->renderer, color->r, color->g, color->b, color->a);
    SDL_Rect rect = {
        .x = 7 + ui->cursor_pos_on_screen[cursor_pos],
        .y = UI_PATTERN_EDIT_NOTE_OFFSET * UI_PATTERN_ROW_SPACING-1,
        .w = 41,
        .h = UI_PATTERN_ROW_SPACING
    };
    if (cursor_pos > 0) {
        rect.w = 9;
    };
    SDL_RenderFillRect(ui->renderer, &rect);
}

void ui_track_render(UiTrack *ui, Track *track, int pos, int cursor_pos, bool edit_mode, int x, int y,
    int first_selection, int last_selection) {
    if (last_selection < first_selection) {
        int tmp = first_selection;
        first_selection = last_selection;
        last_selection = tmp;
    }
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    _ui_track_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_BLEND);
    _ui_track_draw_background(ui, pos);
    _ui_track_draw_notes(ui, track, pos, first_selection, last_selection);
    if (edit_mode && cursor_pos > -1) {
        SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_ADD);
        _ui_track_draw_cursor(ui, cursor_pos, edit_mode);
        SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderTarget(ui->renderer, NULL);
    ui->target_rect.x = x;
    ui->target_rect.y = y;
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, &ui->target_rect);
}

