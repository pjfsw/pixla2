#include <stdio.h>
#include <stdbool.h>

#include "ui_synth.h"
#include "parameter_functions.h"
#include "selection_functions.h"
#include "font.h"
#include "ui_colors.h"


#define UI_PANEL_H 130

bool _ui_synth_create_components(UiSynth *ui) {

    char *waveforms[] = {"Square", "Saw", "Tria", "Sine"};
    int waveforms_count = sizeof(waveforms)/sizeof(char*);
    char *combiner_mode_options[] = {"Add", "FM"};
    int comb_mode_count = sizeof(combiner_mode_options)/sizeof(char*);
    char *combiner_strength_mode_options[] = {"Manual", "VCA"};
    int comb_strenth_count = sizeof(combiner_strength_mode_options)/sizeof(char*);
    char *vca_settings[] = {"Normal", "Inverse"};
    int vca_settings_count = sizeof(vca_settings)/sizeof(char*);
    char *off_on[] = {"Off", "On"};
    int off_on_count = sizeof(off_on)/sizeof(char*);
    char *transpose[] = {"None", "-Oct", "-2Oct", "-3Oct", "+Oct","+2Oct","+3Oct"};
    int transpose_count = sizeof(transpose)/sizeof(char*);

    UiComponentGroup *voice_group = ui_cmgr_component_group(ui->cmgr, "Oscillators", 272);
    ui_cmgr_add_selection(ui->cmgr, voice_group, "Osc 1", waveforms, waveforms_count, sf_synth_oscillator1_waveform);
    ui_cmgr_add_selection(ui->cmgr, voice_group, "Osc 2", waveforms, waveforms_count, sf_synth_oscillator2_waveform);
    ui_cmgr_add_selection(ui->cmgr, voice_group, "Trans", transpose, transpose_count, sf_synth_oscillator2_transpose);

    UiComponentGroup *vca_group = ui_cmgr_component_group(ui->cmgr, "Amp", 160);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "A", pf_synth_voice_attack);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "D", pf_synth_voice_decay);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "S", pf_synth_voice_sustain);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "R", pf_synth_voice_release);

    UiComponentGroup *mod_group = ui_cmgr_component_group(ui->cmgr, "Modulation", 216);
    ui_cmgr_add_selection(ui->cmgr, mod_group, "Wave", waveforms, waveforms_count, sf_synth_lfo1_waveform);
    ui_cmgr_add_parameter(ui->cmgr, mod_group, "Freq", pf_synth_lfo1_frequency);
    ui_cmgr_add_parameter(ui->cmgr, mod_group, "Amnt", pf_synth_lfo1_amount);
    ui_cmgr_add_parameter(ui->cmgr, mod_group, "Dlay", pf_synth_lfo1_delay);
    ui_cmgr_add_parameter(ui->cmgr, mod_group, "Osc1", pf_synth_mod_osc1);
    ui_cmgr_add_parameter(ui->cmgr, mod_group, "Osc2", pf_synth_mod_osc2);
    ui_cmgr_add_parameter(ui->cmgr, mod_group, "Filtr", pf_synth_mod_filter);


    ui_cmgr_add_new_line(ui->cmgr);
    UiComponentGroup *combiner_group = ui_cmgr_component_group(ui->cmgr, "Oscillator combiner", 272);
    ui_cmgr_add_selection(ui->cmgr, combiner_group, "Comb mode", combiner_mode_options,comb_mode_count,  sf_synth_combiner_mode);
    ui_cmgr_add_selection(ui->cmgr, combiner_group, "Comb bal", combiner_strength_mode_options,  comb_strenth_count, sf_synth_combiner_oscillator2_strength_mode);
    ui_cmgr_add_parameter(ui->cmgr, combiner_group, "Detune", pf_synth_comb_detune);
    ui_cmgr_add_parameter(ui->cmgr, combiner_group, "Osc2", pf_synth_comb_strength);

    UiComponentGroup *combiner_vca_group = ui_cmgr_component_group(ui->cmgr, "Combiner VCA", 296);
    ui_cmgr_add_parameter(ui->cmgr, combiner_vca_group, "A", pf_synth_comb_attack);
    ui_cmgr_add_parameter(ui->cmgr, combiner_vca_group, "D", pf_synth_comb_decay);
    ui_cmgr_add_parameter(ui->cmgr, combiner_vca_group, "S", pf_synth_comb_sustain);
    ui_cmgr_add_parameter(ui->cmgr, combiner_vca_group, "R", pf_synth_comb_release);
    ui_cmgr_add_selection(ui->cmgr, combiner_vca_group, "Mode", vca_settings, vca_settings_count, sf_synth_combiner_vca_inverse);

    UiComponentGroup *master_group = ui_cmgr_component_group(ui->cmgr, "Master", 64);
    ui_cmgr_add_parameter(ui->cmgr, master_group, "Levl", pf_synth_master_level);

    ui_cmgr_add_new_line(ui->cmgr);

    UiComponentGroup *filter_group = ui_cmgr_component_group(ui->cmgr, "Filter", 96);
    ui_cmgr_add_parameter(ui->cmgr, filter_group, "Freq", pf_synth_filter_f);
    ui_cmgr_add_parameter(ui->cmgr, filter_group, "Q", pf_synth_filter_q);
    UiComponentGroup *filter_vca_group = ui_cmgr_component_group(ui->cmgr, "Filter VCA", 256);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "A", pf_synth_filter_attack);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "D", pf_synth_filter_decay);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "S", pf_synth_filter_sustain);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "R", pf_synth_filter_release);
    ui_cmgr_add_selection(ui->cmgr, filter_vca_group, "Mode", vca_settings, vca_settings_count, sf_synth_filter_vca_inverse);

    UiComponentGroup *echo_group = ui_cmgr_component_group(ui->cmgr, "Echo", 240);
    ui_cmgr_add_selection(ui->cmgr, echo_group, "Use", off_on, off_on_count, sf_synth_echo);
    ui_cmgr_add_parameter(ui->cmgr, echo_group, "Dlay", pf_synth_echo_delay);
    ui_cmgr_add_parameter(ui->cmgr, echo_group, "Wet", pf_synth_echo_wetness);
    ui_cmgr_add_parameter(ui->cmgr, echo_group, "Fbk", pf_synth_echo_feedback);


    return !ui_cmgr_is_error(ui->cmgr);
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

    if ((ui->cmgr = ui_cmgr_create(renderer)) == NULL) {
        ui_synth_destroy(ui);
        return NULL;
    }

    if (!_ui_synth_create_components(ui)) {
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
        if (ui->cmgr != NULL) {
            ui_cmgr_destroy(ui->cmgr);
        }
        free(ui);
    }
}

void _ui_synth_draw_vca(UiSynth *ui, Synth *synth) {
    ui->visual_vca.settings = &synth->settings.voice_vca_settings;
    vca_trigger(&ui->visual_vca, 440);
    double dt = 1/48000.0;
    int scale = 40;
    ui_colors_set(ui->renderer, ui_colors_synth_main());
    int n = 24000;
    for (int i = 0; i < n; i++) {
        double amp = vca_transform(&ui->visual_vca, 1, dt);
        if (i % scale == 0) {
            SDL_RenderDrawPoint(ui->renderer, i/scale, (1-amp)*50.0 + 400 );
        }
        if ( i > n/2) {
            vca_off(&ui->visual_vca);
        }
    }
    //vca_off(&ui->visual_vca);
}
void ui_synth_render(UiSynth *ui, Synth *synth, int x, int y) {
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    ui_colors_set(ui->renderer, ui_colors_synth_bg());
    SDL_RenderClear(ui->renderer);
    ui_cmgr_render(ui->cmgr, synth);
    _ui_synth_draw_vca(ui,synth);
    SDL_SetRenderTarget(ui->renderer, NULL);
    ui->target_rect.x = x;
    ui->target_rect.y = y;
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, &ui->target_rect);
}


void ui_synth_click(UiSynth *ui, Synth *synth, int x, int y) {
    ui_cmgr_click(ui->cmgr, synth, x,y);
}

void ui_synth_alter_parameter(UiSynth *ui, Synth *synth, double delta) {
    ui_cmgr_alter_component(ui->cmgr, synth, delta);
}

void ui_synth_next_parameter(UiSynth *ui) {
    ui_cmgr_next_component(ui->cmgr);
}

void ui_synth_prev_parameter(UiSynth *ui) {
    ui_cmgr_previous_component(ui->cmgr);
}

