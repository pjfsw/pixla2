#include <stdio.h>
#include <stdbool.h>

#include "ui_synth.h"

#define UI_SLIDER_W 16
#define UI_SLIDER_H 100

void _ui_synth_set_frame_color(UiSynth *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 12,180,180,255);
}

void _ui_synth_set_main_color(UiSynth *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 16,240,240,255);
}

void _ui_synth_set_bg_color(UiSynth *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 3,45,45,255);
}

void _ui_synth_init_slider_texture(UiSynth *ui) {
    SDL_SetRenderTarget(ui->renderer, ui->slider_texture);
    _ui_synth_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);
    _ui_synth_set_frame_color(ui);
    SDL_Rect rect = {
        .x = 0,
        .y = 0,
        .w = UI_SLIDER_W,
        .h = UI_SLIDER_H
    };
    SDL_RenderDrawRect(ui->renderer, &rect);
    for (int i = 0; i < 10; i++) {
        int w = UI_SLIDER_W/3;
        if (i == 5) {
            w = UI_SLIDER_W/2;
        }
        int x = (UI_SLIDER_W-w)/2;
        SDL_RenderDrawLine(ui->renderer, x,i*10,x+w,i*10);
    }

    SDL_SetRenderTarget(ui->renderer, NULL);
}

UiSynth *ui_synth_create(SDL_Renderer *renderer) {
    UiSynth *ui = calloc(1, sizeof(UiSynth));

    ui->renderer = renderer;
    ui->texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_SYNTH_W,
        UI_SYNTH_H);
    if (ui->texture == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        ui_synth_destroy(ui);
        return NULL;
    }
    ui->target_rect.w = UI_SYNTH_W;
    ui->target_rect.h = UI_SYNTH_H;

    ui->slider_texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_SLIDER_W, UI_SLIDER_H);
    if (ui->slider_texture == NULL) {
        fprintf(stderr, "Failed to create slider_texture %s\n", SDL_GetError());
        ui_synth_destroy(ui);
        return NULL;
    }
    _ui_synth_init_slider_texture(ui);

    return ui;
}

void ui_synth_destroy(UiSynth *ui) {
    if (ui != NULL) {
        if (ui->slider_texture != NULL) {
            SDL_DestroyTexture(ui->slider_texture);
        }
        if (ui->texture != NULL) {
            SDL_DestroyTexture(ui->texture);
        }
        free(ui);
    }
}

void _ui_synth_draw_frame(UiSynth *ui, int x, int y, int w, int h) {
    _ui_synth_set_frame_color(ui);
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    SDL_RenderDrawRect(ui->renderer, &rect);
}

void _ui_synth_draw_slider(UiSynth *ui, double *value, int x, int y) {
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = UI_SLIDER_W,
        .h = UI_SLIDER_H
    };
    double v = *value;
    v = fmin(v, 1);
    v = fmax(v, 0);
    int yPos = UI_SLIDER_H- v * (double)UI_SLIDER_H;

    SDL_RenderCopy(ui->renderer, ui->slider_texture, NULL, &rect);
    _ui_synth_set_main_color(ui);
    rect.x=x+1;
    rect.y=y+yPos;
    rect.w=UI_SLIDER_W-2;
    rect.h= UI_SLIDER_H-yPos;
    SDL_RenderFillRect(ui->renderer, &rect);
}

void _ui_synth_draw_sliders(UiSynth *ui, Synth *synth) {
    _ui_synth_draw_slider(ui, &synth->voice_vca_settings.attack, UI_SLIDER_W, 16);
    _ui_synth_draw_slider(ui, &synth->voice_vca_settings.decay, 3*UI_SLIDER_W, 16);
    _ui_synth_draw_slider(ui, &synth->voice_vca_settings.sustain, 5*UI_SLIDER_W, 16);
    _ui_synth_draw_slider(ui, &synth->voice_vca_settings.release, 7*UI_SLIDER_W, 16);
}

void ui_synth_render(UiSynth *ui, Synth *synth, int x, int y) {
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    _ui_synth_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);
    _ui_synth_draw_frame(ui, 0,0,UI_SYNTH_W, UI_SYNTH_H);
    _ui_synth_draw_sliders(ui, synth);
    SDL_SetRenderTarget(ui->renderer, NULL);
    ui->target_rect.x = x;
    ui->target_rect.y = y;
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, &ui->target_rect);
}
