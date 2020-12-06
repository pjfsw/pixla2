#include <SDL2/SDL.h>
#include "ui_trackpos.h"
#include "font.h"

#define _UI_TRACKPOS_TH ((UI_PATTERN_VISIBLE_NOTES + NOTES_PER_TRACK) * UI_PATTERN_ROW_SPACING)


void _ui_trackpos_set_main_color(UiTrackPos *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 191,63,127,255);
}

void _ui_trackpos_set_bg_color(UiTrackPos *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 0,0,0,255);
}


void _ui_trackpos_init_texture(UiTrackPos *ui) {
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    _ui_trackpos_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);
    _ui_trackpos_set_main_color(ui);
    char s[4];
    for (int i = 0; i < NOTES_PER_TRACK; i++) {
        sprintf(s, "%02d", i);
        font_write(ui->renderer, s, 0, (i + UI_PATTERN_EDIT_NOTE_OFFSET) * UI_PATTERN_ROW_SPACING);
    }

    SDL_SetRenderTarget(ui->renderer, NULL);
}


UiTrackPos *ui_trackpos_create(SDL_Renderer *renderer) {
    UiTrackPos *ui = calloc(1, sizeof(UiTrackPos));
    ui->renderer = renderer;
    ui->texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_TRACKPOS_W,
        _UI_TRACKPOS_TH);
    if (ui->texture == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        ui_trackpos_destroy(ui);
        return NULL;
    }

    ui->target_rect.w = UI_TRACKPOS_W*2;
    ui->target_rect.h = UI_TRACKPOS_H*2;

    ui->source_rect.w = UI_TRACKPOS_W;
    ui->source_rect.h = UI_TRACKPOS_H;
    ui->source_rect.x = 0;

    _ui_trackpos_init_texture(ui);

    return ui;
}

void ui_trackpos_destroy(UiTrackPos *ui) {
    if (ui != NULL) {
        if (ui->texture != NULL) {
            SDL_DestroyTexture(ui->texture);
        }
        free(ui);
    }
}

void ui_trackpos_render(UiTrackPos *ui, int pos, int x, int y) {
    ui->target_rect.x = x;
    ui->target_rect.y = y;

    ui->source_rect.y = pos * UI_PATTERN_ROW_SPACING;

    SDL_RenderCopy(ui->renderer, ui->texture, &ui->source_rect, &ui->target_rect);
}
