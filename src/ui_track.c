#include <stdlib.h>
#include "ui_track.h"
#include "font.h"

#define _UI_TRACK_H (UI_PATTERN_VISIBLE_NOTES * UI_PATTERN_ROW_SPACING)

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
    note[2] = octave + 47;

    SDL_Texture *texture = font_create_texture(ui->renderer, pitch == 0 ? no_note : note, color);
    free(note);
    return texture;
}

SDL_Texture *_ui_track_create_digit_texture(UiTrack *ui, int digit, SDL_Color *color) {
    char *digits[] = {"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"};
    return font_create_texture(ui->renderer, digits[digit], color);
}

UiTrack *ui_track_create(SDL_Renderer *renderer) {
    UiTrack *ui = calloc(1, sizeof(UiTrack));
    ui->renderer = renderer;
    ui->texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_TRACK_W,
        _UI_TRACK_H);
    if (ui->texture == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        ui_track_destroy(ui);
        return NULL;
    }

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
    for (int i = 0; i < 16; i++) {
        ui->digit_texture[i] = _ui_track_create_digit_texture(ui,i, ui_pattern_get_note_color());
        if (ui->digit_texture[i] == NULL) {
            fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
            ui_track_destroy(ui);
            return NULL;
        }
    }
    ui->target_rect.w = UI_TRACK_W*2;
    ui->target_rect.h = _UI_TRACK_H*2;

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
        for (int i = 0; i < 16; i++) {
            if (ui->digit_texture[i] != NULL) {
                SDL_DestroyTexture(ui->digit_texture[i]);
            }
        }
        free(ui);
    }
}

void _ui_track_draw_notes(UiTrack *ui, Track *track, int pos) {
    _ui_track_set_main_color(ui);
    SDL_Rect pitch_rect;
    pitch_rect.w = _UI_TRACK_PITCH_W;
    pitch_rect.h = _UI_TRACK_PITCH_H;
    pitch_rect.x = 8;
    SDL_Rect target_rect;
    target_rect.w = 8;
    target_rect.h = 8;
    for (int i = 0; i < UI_PATTERN_VISIBLE_NOTES; i++) {
        int row = pos + i - UI_PATTERN_EDIT_NOTE_OFFSET;
        if (row < 0 || row >= NOTES_PER_TRACK) {
            continue;
        }
        pitch_rect.y = i * UI_PATTERN_ROW_SPACING;
        target_rect.y = i * UI_PATTERN_ROW_SPACING;
        SDL_RenderCopy(ui->renderer, ui->pitch_texture[track->note[row].pitch], NULL, &pitch_rect);
        target_rect.x = _UI_TRACK_PITCH_W + pitch_rect.x + 8;
        SDL_RenderCopy(ui->renderer, ui->digit_texture[track->note[row].instrument], NULL, &target_rect);
        target_rect.x+=16;
        SDL_RenderCopy(ui->renderer, ui->digit_texture[track->note[row].velocity >> 4], NULL, &target_rect);
        target_rect.x+=8;
        SDL_RenderCopy(ui->renderer, ui->digit_texture[track->note[row].velocity & 15], NULL, &target_rect);

        //font_write(ui->renderer, "C-4 1 01", 0, i * UI_PATTERN_ROW_SPACING);
    }
    SDL_Rect rect = {
        .x = 2,
        .y = UI_PATTERN_ROW_SPACING * UI_PATTERN_EDIT_NOTE_OFFSET,
        .w = UI_TRACK_W-2,
        .h = UI_PATTERN_ROW_SPACING
    };
    SDL_Color *color = ui_pattern_get_active_row_color();
    SDL_SetRenderDrawColor(ui->renderer, color->r, color->g,color->b,color->a );

    SDL_RenderFillRect(ui->renderer, &rect);
    rect.y = 0;
    rect.x = 0;
    rect.w = 2;
    rect.h = _UI_TRACK_H;
    SDL_RenderFillRect(ui->renderer, &rect);
}

void ui_track_render(UiTrack *ui, Track *track, int pos, int x, int y) {
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    _ui_track_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_BLEND);
    _ui_track_draw_notes(ui, track, pos);
    SDL_SetRenderTarget(ui->renderer, NULL);
    ui->target_rect.x = x;
    ui->target_rect.y = y;
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, &ui->target_rect);

}

