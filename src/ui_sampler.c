#include "ui_sampler.h"
#include "ui_colors.h"
#include "ui_boundary.h"
#include "font.h"

#define UI_SAMPLER_W UI_INSTRUMENT_W
#define UI_SAMPLER_H UI_INSTRUMENT_H

UiSampler *ui_sampler_create(SDL_Renderer *renderer) {
    UiSampler *ui = calloc(1, sizeof(UiSampler));
    ui->renderer = renderer;
    return ui;
}

void ui_sampler_destroy(UiSampler *ui) {
    if (ui == NULL) {
        return;
    }
    free(ui);
}

void ui_sampler_render(UiSampler *ui, Sampler *sampler, int x, int y) {
    ui_colors_set(ui->renderer, ui_colors_synth_bg());
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = UI_SAMPLER_W,
        .h = UI_SAMPLER_H
    };
    SDL_RenderFillRect(ui->renderer, &rect);
    ui_colors_set(ui->renderer, ui_colors_synth_frame());
    font_write_scale(ui->renderer, "SAMPLER",x+(UI_SAMPLER_W-112)/2,y+UI_SAMPLER_H-16,2);
}

