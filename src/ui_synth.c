#include <stdio.h>
#include <stdbool.h>

#include "ui_synth.h"
#include "parameter_functions.h"
#include "font.h"

#define UI_SLIDER_W 32
#define UI_SLIDER_WP 48
#define UI_SLIDER_H 100
#define UI_SLIDER_TH 128


void _ui_synth_set_frame_color(UiSynth *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 8,120,120,255);
}

void _ui_synth_set_main_color(UiSynth *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 16,240,240,255);
}

void _ui_synth_set_bg_color(UiSynth *ui) {
    SDL_SetRenderDrawColor(ui->renderer, 3,45,45,255);
}

SDL_Texture *_ui_synth_init_slider_texture(UiSynth *ui, char *text) {
    SDL_Texture *texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_SLIDER_W, UI_SLIDER_TH);
    if (texture == NULL) {
        fprintf(stderr, "Failed to create slider_texture %s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetRenderTarget(ui->renderer, texture);
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
    font_write(ui->renderer, text, 0, UI_SLIDER_H+1);
    SDL_SetRenderTarget(ui->renderer, NULL);
    return texture;
}

bool _ui_synth_init_parameter_controller(
    UiSynth *ui,
    int index,
    int x,
    int y,
    ParameterFunc parameter_func,
    char *text
) {
    ParameterController *parameter_controller = &ui->parameter_controllers[index];

    parameter_controller->x = x;
    parameter_controller->y = y;
    parameter_controller->w = UI_SLIDER_W;
    parameter_controller->h = UI_SLIDER_H;
    parameter_controller->parameter_func = parameter_func;
    parameter_controller->texture = _ui_synth_init_slider_texture(ui, text);
    return (parameter_controller->texture != NULL);
}

bool _ui_synth_init_parameter_controllers(UiSynth *ui) {
    ui->number_of_parameter_controllers = 7;
    ui->parameter_controllers = calloc(ui->number_of_parameter_controllers, sizeof(ParameterController));

    int pc = 0;
    int row1 = 16;
    bool ok = true;
    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        UI_SLIDER_WP,
        row1,
        pf_synth_voice_attack,
        "A"
    );
    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        2*UI_SLIDER_WP,
        row1,
        pf_synth_voice_decay,
        "D"
    );
    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        3*UI_SLIDER_WP,
        row1,
        pf_synth_voice_sustain,
        "S"
    );
    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        4*UI_SLIDER_WP,
        row1,
        pf_synth_voice_release,
        "R"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        6*UI_SLIDER_WP,
        row1,
        pf_synth_comb_detune,
        "Detune"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        7*UI_SLIDER_WP,
        row1,
        pf_synth_comb_oscillator_scale,
        "Scale"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        8*UI_SLIDER_WP,
        row1,
        pf_synth_comb_strength,
        "Strength"
    );

    return ok;
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

    if (!_ui_synth_init_parameter_controllers(ui)) {
        ui_synth_destroy(ui);
        return NULL;
    }

    return ui;
}

void ui_synth_destroy(UiSynth *ui) {
    if (ui != NULL) {
        if (ui->texture != NULL) {
            SDL_DestroyTexture(ui->texture);
        }
        if (ui->parameter_controllers != NULL) {
            for (int i = 0; i < ui->number_of_parameter_controllers; i++) {
                ParameterController *pc = &ui->parameter_controllers[i];
                if (pc->texture != NULL) {
                    SDL_DestroyTexture(pc->texture);
                }
            }
            free(ui->parameter_controllers);
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

void _ui_synth_draw_slider(UiSynth *ui, Synth *synth, ParameterController *pc, bool active) {
    int x = pc->x;
    int y = pc->y;
    double *value = pc->parameter_func(synth);
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = UI_SLIDER_W,
        .h = UI_SLIDER_TH
    };
    double v = *value;
    v = fmin(v, 1);
    v = fmax(v, 0);

    SDL_RenderCopy(ui->renderer, pc->texture, NULL, &rect);

    int h = UI_SLIDER_H-1;
    int yPos = h - v * (double)h;
    rect.h = h;

    if (active) {
        _ui_synth_set_main_color(ui);
        SDL_RenderDrawRect(ui->renderer, &rect);
    } else {
        _ui_synth_set_frame_color(ui);
    }
    rect.x=x+1;
    rect.y=y+yPos;
    rect.w=UI_SLIDER_W-2;
    rect.h= h-yPos;
    SDL_RenderFillRect(ui->renderer, &rect);
}

void _ui_synth_draw_sliders(UiSynth *ui, Synth *synth) {
    for (int i = 0; i < ui->number_of_parameter_controllers; i++) {
        ParameterController *pc = &ui->parameter_controllers[i];
        _ui_synth_draw_slider(ui, synth, pc, i == ui->current_parameter);
    }
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

void ui_synth_click(UiSynth *ui, Synth *synth, int x, int y) {
    for (int i = 0 ; i < ui->number_of_parameter_controllers; i++) {
        ParameterController *pc = &ui->parameter_controllers[i];
        if (x >= pc->x && x <= pc->x + pc->w &&
            y >= pc->y && y <= pc->y + pc->h) {
            ui->current_parameter = i;
            double v = (double)(pc->h - y + pc->y) * 0.01;
            *pc->parameter_func(synth) = v;
        }

    }
}

void ui_synth_alter_parameter(UiSynth *ui, Synth *synth, double delta) {
    ParameterController *pc = &ui->parameter_controllers[ui->current_parameter];
    double *v = pc->parameter_func(synth);
    *v = fmin(fmax(*v + delta,0), 1);
}

void ui_synth_next_parameter(UiSynth *ui) {
    ui->current_parameter = (ui->current_parameter + 1) % ui->number_of_parameter_controllers;
}

void ui_synth_prev_parameter(UiSynth *ui) {
    int n = ui->current_parameter - 1;
    if (n < 0) {
        n = ui->number_of_parameter_controllers - 1;
    }
    ui->current_parameter = n;
}

