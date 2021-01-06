#include <stdbool.h>

#include "parameter_functions.h"
#include "ui_mixer.h"
#include "ui_colors.h"
#include "rack.h"
#include "font.h"

#define _UI_MIXER_SPACING 72

bool _ui_mixer_create_components(UiMixer *ui) {
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        char str[10];
        sprintf(str, "Instr %d", i);
        UiComponentGroup *instr_group = ui_cmgr_component_group(ui->cmgr, str, _UI_MIXER_SPACING-4);
        ui_cmgr_add_parameter(ui->cmgr, instr_group, "Vol", pf_mixer_instr_level(i));
    }
    UiComponentGroup *master_group = ui_cmgr_component_group(ui->cmgr, "Master", _UI_MIXER_SPACING-4);
    ui_cmgr_add_parameter(ui->cmgr, master_group, "Vol", pf_mixer_master_level);
    return !ui_cmgr_is_error(ui->cmgr);
}


UiMixer *ui_mixer_create(SDL_Renderer *renderer) {
    UiMixer *ui = calloc(1, sizeof(UiMixer));
    ui->renderer = renderer;
    ui->texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_MIXER_W,
        UI_MIXER_H);
    if (ui->texture == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        ui_mixer_destroy(ui);
        return NULL;
    }
    if ((ui->cmgr = ui_cmgr_create(renderer)) == NULL) {
        ui_mixer_destroy(ui);
        return NULL;
    }
    ui->vu_ui = ui_volume_meter_create(renderer);
    if (!_ui_mixer_create_components(ui)) {
        ui_mixer_destroy(ui);
        return NULL;
    }

    return ui;

}

void ui_mixer_destroy(UiMixer *ui) {
    if (ui == NULL) {
        return;
    }
    if (ui->vu_ui != NULL) {
        ui_volume_meter_destroy(ui->vu_ui);
    }
    if (ui->cmgr != NULL) {
        ui_cmgr_destroy(ui->cmgr);
    }
    if (ui->texture != NULL) {
        SDL_DestroyTexture(ui->texture);
    }
    free(ui);
}

void _ui_mixer_render_vus(UiMixer *ui, Mixer *mixer, int x, int y) {
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i <  NUMBER_OF_INSTRUMENTS; i++) {
        ui_volume_meter_render(ui->vu_ui, mixer->instrument_loudness[i].loudness,
            &ui->loudness[i], x+i*_UI_MIXER_SPACING, y);
    }
    ui_volume_meter_render(ui->vu_ui, mixer->master_loudness.loudness,
        &ui->master_loudness, x+NUMBER_OF_INSTRUMENTS*_UI_MIXER_SPACING, y);

    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_NONE);

}

void ui_mixer_render(UiMixer *ui, Mixer *mixer, int x, int y) {
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    ui_colors_set(ui->renderer, ui_colors_synth_bg());
    SDL_RenderClear(ui->renderer);
    ui_cmgr_render(ui->cmgr, mixer->settings);
    _ui_mixer_render_vus(ui, mixer, 52, 12);
    ui_colors_set(ui->renderer, ui_colors_synth_frame());
    font_write_scale(ui->renderer, "MIXER",(UI_MIXER_W-80)/2,UI_MIXER_H-16,2);

    //_ui_synth_draw_vca(ui,synth);
    SDL_SetRenderTarget(ui->renderer, NULL);
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = UI_MIXER_W,
        .h = UI_MIXER_H
    };
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, &rect);
}

void ui_mixer_click(UiMixer *ui, Mixer *mixer, int x, int y) {
    ui_cmgr_click(ui->cmgr, mixer->settings, x,y);
}

