#include <stdio.h>
#include <stdbool.h>

#include "ui_synth.h"
#include "parameter_functions.h"
#include "selection_functions.h"
#include "font.h"
#include "ui_colors.h"
#include "ui_boundary.h"
#include "oscilloscope.h"

#define UI_PANEL_H 130

#define UI_SYNTH_H UI_INSTRUMENT_H
#define UI_SYNTH_W UI_INSTRUMENT_W

bool _ui_synth_create_components(UiSynth *ui) {

    char *waveforms[] = {"Sine", "Saw", "Square", "Tria"};
    int waveforms_count = sizeof(waveforms)/sizeof(char*);
    char *lfo_targets[] = {"Osc1+2", "Filter", "Phase", "O2 Mix"};
    int lfo_targets_count = sizeof(lfo_targets)/sizeof(char*);
    char *vca_targets[] = {"Off", "Phase", "O2 Mix", "RingF", "RingA", "RingF+A"};
    int vca_targets_count = sizeof(vca_targets)/sizeof(char*);
    char *phase_modes[] = {"Off(0)", "Sub(-)", "Mul(*)"};
    int phase_modes_count = sizeof(phase_modes)/sizeof(char*);
    char *combiner_mode_options[] = {"Add", "FM"};
    int comb_mode_count = sizeof(combiner_mode_options)/sizeof(char*);
    char *vca_settings[] = {"Normal", "Inverse"};
    int vca_settings_count = sizeof(vca_settings)/sizeof(char*);
    /*char *off_on[] = {"Off", "On"};
    int off_on_count = sizeof(off_on)/sizeof(char*);
    */
    char *transpose[] = {"None", "-Oct", "-2Oct", "-3Oct", "+Oct","+2Oct","+3Oct"};
    int transpose_count = sizeof(transpose)/sizeof(char*);
    char *waveform = "Waveform";
    char *echo_mode[] = {"None", "1 step", "2 step", "3 step", "4 step", "5 step", "6 step", "7 step", "8 step"};
    int echo_mode_count = sizeof(echo_mode)/sizeof(char*);

    UiComponentGroup *osc1_group = ui_cmgr_component_group(ui->cmgr, "Oscillator 1", 352);
    ui_cmgr_add_selection(ui->cmgr, osc1_group, waveform, waveforms, waveforms_count, sf_synth_oscillator1_waveform);
    ui_cmgr_add_selection(ui->cmgr, osc1_group, "Phase", phase_modes, phase_modes_count, sf_synth_oscillator1_phase_mode);
    ui_cmgr_add_parameter(ui->cmgr, osc1_group, "Phase", pf_synth_oscillator1_phase);
    ui_cmgr_add_selection(ui->cmgr, osc1_group, "Transp", transpose, transpose_count, sf_synth_oscillator1_transpose);

    UiComponentGroup *filter_group = ui_cmgr_component_group(ui->cmgr, "Filter", 96);
    ui_cmgr_add_parameter(ui->cmgr, filter_group, "Freq", pf_synth_filter_f);
    ui_cmgr_add_parameter(ui->cmgr, filter_group, "Q", pf_synth_filter_q);

    UiComponentGroup *filter_vca_group = ui_cmgr_component_group(ui->cmgr, "Filter VCA", 248);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "A", pf_synth_filter_attack);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "D", pf_synth_filter_decay);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "S", pf_synth_filter_sustain);
    ui_cmgr_add_parameter(ui->cmgr, filter_vca_group, "R", pf_synth_filter_release);
    ui_cmgr_add_selection(ui->cmgr, filter_vca_group, "Mode", vca_settings, vca_settings_count, sf_synth_filter_vca_inverse);

    UiComponentGroup *vca_group = ui_cmgr_component_group(ui->cmgr, "Amp", 160);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "A", pf_synth_voice_attack);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "D", pf_synth_voice_decay);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "S", pf_synth_voice_sustain);
    ui_cmgr_add_parameter(ui->cmgr, vca_group, "R", pf_synth_voice_release);


    ui_cmgr_add_new_line(ui->cmgr);

    UiComponentGroup *osc2_group = ui_cmgr_component_group(ui->cmgr, "Oscillator 2", 352);
    ui_cmgr_add_selection(ui->cmgr, osc2_group, waveform, waveforms, waveforms_count, sf_synth_oscillator2_waveform);
    ui_cmgr_add_selection(ui->cmgr, osc2_group, "Phase", phase_modes, phase_modes_count, sf_synth_oscillator2_phase_mode);
    ui_cmgr_add_parameter(ui->cmgr, osc2_group, "Phase", pf_synth_oscillator2_phase);
    ui_cmgr_add_selection(ui->cmgr, osc2_group, "Transp", transpose, transpose_count, sf_synth_oscillator2_transpose);

    UiComponentGroup *combiner_group = ui_cmgr_component_group(ui->cmgr, "Mix", 172);
    ui_cmgr_add_selection(ui->cmgr, combiner_group, "Comb mode", combiner_mode_options,comb_mode_count,  sf_synth_combiner_mode);
    ui_cmgr_add_parameter(ui->cmgr, combiner_group, "Detune", pf_synth_comb_detune);
    ui_cmgr_add_parameter(ui->cmgr, combiner_group, "Osc2", pf_synth_comb_strength);

    UiComponentGroup *ringmod_group = ui_cmgr_component_group(ui->cmgr, "Ring mod", 96);
    ui_cmgr_add_parameter(ui->cmgr, ringmod_group, "RFreq", pf_synth_comb_ring_freq);
    ui_cmgr_add_parameter(ui->cmgr, ringmod_group, "RAmnt", pf_synth_comb_ring_amount);

    UiComponentGroup *echo_group = ui_cmgr_component_group(ui->cmgr, "Echo", 224);
    ui_cmgr_add_selection(ui->cmgr, echo_group, "Sync mode", echo_mode, echo_mode_count, sf_synth_echo);
    ui_cmgr_add_parameter(ui->cmgr, echo_group, "Dlay", pf_synth_echo_delay);
    ui_cmgr_add_parameter(ui->cmgr, echo_group, "Wet", pf_synth_echo_wetness);
    ui_cmgr_add_parameter(ui->cmgr, echo_group, "Fbk", pf_synth_echo_feedback);

    ui_cmgr_add_new_line(ui->cmgr);

    UiComponentGroup *mod1_group = ui_cmgr_component_group(ui->cmgr, "Modulation 1", 224);
    ui_cmgr_add_selection(ui->cmgr, mod1_group, waveform, waveforms, waveforms_count, sf_synth_lfo1_waveform);
    ui_cmgr_add_selection(ui->cmgr, mod1_group, "Target", lfo_targets, lfo_targets_count, sf_synth_lfo1_target);
    ui_cmgr_add_parameter(ui->cmgr, mod1_group, "Freq", pf_synth_lfo1_frequency);
    ui_cmgr_add_parameter(ui->cmgr, mod1_group, "Amnt", pf_synth_lfo1_amount);
    ui_cmgr_add_parameter(ui->cmgr, mod1_group, "Dlay", pf_synth_lfo1_delay);


    UiComponentGroup *mod2_group = ui_cmgr_component_group(ui->cmgr, "Modulation 2", 224);
    ui_cmgr_add_selection(ui->cmgr, mod2_group, waveform, waveforms, waveforms_count, sf_synth_lfo2_waveform);
    ui_cmgr_add_selection(ui->cmgr, mod2_group, "Target", lfo_targets, lfo_targets_count, sf_synth_lfo2_target);
    ui_cmgr_add_parameter(ui->cmgr, mod2_group, "Freq", pf_synth_lfo2_frequency);
    ui_cmgr_add_parameter(ui->cmgr, mod2_group, "Amnt", pf_synth_lfo2_amount);
    ui_cmgr_add_parameter(ui->cmgr, mod2_group, "Dlay", pf_synth_lfo2_delay);

    UiComponentGroup *mod_vca_group = ui_cmgr_component_group(ui->cmgr, "Modulation VCA", 280);
    ui_cmgr_add_parameter(ui->cmgr, mod_vca_group, "A", pf_synth_mod_attack);
    ui_cmgr_add_parameter(ui->cmgr, mod_vca_group, "D", pf_synth_mod_decay);
    ui_cmgr_add_parameter(ui->cmgr, mod_vca_group, "S", pf_synth_mod_sustain);
    ui_cmgr_add_parameter(ui->cmgr, mod_vca_group, "R", pf_synth_mod_release);
    ui_cmgr_add_parameter(ui->cmgr, mod_vca_group, "Amnt", pf_synth_mod_vca_strength);
    ui_cmgr_add_selection(ui->cmgr, mod_vca_group, "Target", vca_targets, vca_targets_count, sf_synth_mod_vca_target);
    ui_cmgr_add_selection(ui->cmgr, mod_vca_group, "Mode", vca_settings, vca_settings_count, sf_synth_mod_vca_inverse);

    UiComponentGroup *master_group = ui_cmgr_component_group(ui->cmgr, "Master", 64);
    ui_cmgr_add_parameter(ui->cmgr, master_group, "Levl", pf_synth_master_level);


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

/*
void _ui_synth_draw_vca(UiSynth *ui, Synth *synth) {
    ui->visual_vca.settings = &synth->settings.voice_vca_settings;
    vca_trigger(&ui->visual_vca, 440, 255);
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
*/

void _ui_synth_render_oscilloscope(UiSynth *ui,
    Oscilloscope *oscilloscope, Oscilloscope *history,
    bool new_synth,
    int x, int y) {
    int w = 128;
    int h = 128;
    int half_h = h/2;
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = w+1,
        .h = h+3
    };
    x++;
    y++;
    SDL_SetRenderDrawColor(ui->renderer, 0,0,0,255);
    SDL_RenderFillRect(ui->renderer, &rect);
    ui_colors_set(ui->renderer, ui_colors_synth_frame());
    SDL_RenderDrawRect(ui->renderer, &rect);
    SDL_SetRenderDrawColor(ui->renderer, 255,255,255,255);

    if (new_synth) {
        memset(history->buffer, 0, sizeof(double)*OSCILLOSCOPE_SIZE);
    }
    double current_weight = 0.8;
    if (oscilloscope->trigger_time < 0.05) {
        current_weight = 16 * oscilloscope->trigger_time;
    }
    int last_y = 0;
    for (int i = 0; i < w; i++) {
        int index = i * OSCILLOSCOPE_SIZE / w;
        double amp = oscilloscope->buffer[index];
        amp = current_weight * amp + (1-current_weight) * history->buffer[index];
        history->buffer[index] = amp;
        amp+=1.0;
        amp = fmin(fmax(0,amp), 2.0);
        int amp_y = y + h - amp * half_h;
        if (i > 0) {
            SDL_RenderDrawLine(ui->renderer, x+i-1, last_y, x+i, amp_y);
        }
        last_y = amp_y;
    }
}
void _ui_synth_render_oscilloscopes(UiSynth *ui, Synth *synth) {
    Voice *voice = &synth->voices[synth->next_voice];
    bool new_synth = synth != ui->last_synth;
    for (int j = 0; j < 2; j++) {
        _ui_synth_render_oscilloscope(ui,
            oscillator_get_oscilloscope(&voice->oscillators[j]),
            &ui->osc_history[j],
            new_synth,
            216, 12+j*151);
    }
}
void ui_synth_render(UiSynth *ui, Synth *synth, int x, int y) {
    SDL_SetRenderDrawBlendMode(ui->renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    ui_colors_set(ui->renderer, ui_colors_synth_bg());
    SDL_RenderClear(ui->renderer);
    ui_colors_set(ui->renderer, ui_colors_synth_frame());
    font_write_scale(ui->renderer, "SYNTH",(UI_SYNTH_W-80)/2,UI_SYNTH_H-16,2);
    ui_cmgr_render(ui->cmgr, synth->settings);
    _ui_synth_render_oscilloscopes(ui, synth);
    //_ui_synth_draw_vca(ui,synth);
    SDL_SetRenderTarget(ui->renderer, NULL);
    ui->target_rect.x = x;
    ui->target_rect.y = y;
    SDL_RenderCopy(ui->renderer, ui->texture, NULL, &ui->target_rect);
    ui->last_synth = synth;
}


void ui_synth_click(UiSynth *ui, Synth *synth, int x, int y) {
    ui_cmgr_click(ui->cmgr, synth->settings, x,y);
}

void ui_synth_alter_parameter(UiSynth *ui, Synth *synth, double delta) {
    ui_cmgr_alter_component(ui->cmgr, synth->settings, delta);
}

void ui_synth_next_parameter(UiSynth *ui) {
    ui_cmgr_next_component(ui->cmgr);
}

void ui_synth_prev_parameter(UiSynth *ui) {
    ui_cmgr_previous_component(ui->cmgr);
}

