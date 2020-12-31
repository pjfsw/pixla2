#include <stdbool.h>

#include "parameter_functions.h"
#include "ui_mixer.h"
#include "ui_colors.h"
#include "rack.h"
#include "font.h"

bool _ui_mixer_create_components(UiMixer *ui) {
    UiComponentGroup *instr_group = ui_cmgr_component_group(ui->cmgr, "Instruments", 320);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "0", pf_mixer_level_instr1);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "1", pf_mixer_level_instr2);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "2", pf_mixer_level_instr3);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "3", pf_mixer_level_instr4);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "4", pf_mixer_level_instr5);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "5", pf_mixer_level_instr6);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "6", pf_mixer_level_instr7);
    ui_cmgr_add_parameter(ui->cmgr, instr_group, "7", pf_mixer_level_instr8);
    UiComponentGroup *master_group = ui_cmgr_component_group(ui->cmgr, "Master", 64);
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
    if (ui->cmgr != NULL) {
        ui_cmgr_destroy(ui->cmgr);
    }
    if (ui->texture != NULL) {
        SDL_DestroyTexture(ui->texture);
    }
    free(ui);
}

void ui_mixer_render(UiMixer *ui, Mixer *mixer, int x, int y) {
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    ui_colors_set(ui->renderer, ui_colors_synth_bg());
    SDL_RenderClear(ui->renderer);
    ui_cmgr_render(ui->cmgr, mixer);
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
    ui_cmgr_click(ui->cmgr, mixer, x,y);
}

