#include <stdio.h>
#include <stdbool.h>

#include "ui_synth.h"
#include "parameter_functions.h"
#include "selection_functions.h"
#include "font.h"

#define UI_SLIDER_W 32
#define UI_SLIDER_WP 48
#define UI_SLIDER_H 100
#define UI_SLIDER_TH (UI_SLIDER_H+9)
#define UI_PANEL_H 130

#define UI_SELECTION_GROUP_W 100
#define UI_SELECTION_GROUP_LINE_SPACING 10

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
    ui->number_of_parameter_controllers = 20;
    ui->parameter_controllers = calloc(ui->number_of_parameter_controllers, sizeof(ParameterController));

    int pc = 0;
    int row1 = 16;
    int row2 = 16+UI_PANEL_H;

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
        pf_synth_comb_strength,
        "Bal"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        9*UI_SLIDER_WP,
        row1,
        pf_synth_comb_attack,
        "A"
    );


    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        10*UI_SLIDER_WP,
        row1,
        pf_synth_comb_decay,
        "D"
    );


    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        11*UI_SLIDER_WP,
        row1,
        pf_synth_comb_sustain,
        "S"
    );


    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        12*UI_SLIDER_WP,
        row1,
        pf_synth_comb_release,
        "R"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        UI_SLIDER_WP,
        row2,
        pf_synth_filter_f,
        "Freq"
    );


    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        2*UI_SLIDER_WP,
        row2,
        pf_synth_filter_q,
        "Q"
    );


    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        3*UI_SLIDER_WP,
        row2,
        pf_synth_filter_attack,
        "A"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        4*UI_SLIDER_WP,
        row2,
        pf_synth_filter_decay,
        "D"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        5*UI_SLIDER_WP,
        row2,
        pf_synth_filter_sustain,
        "S"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        6*UI_SLIDER_WP,
        row2,
        pf_synth_filter_release,
        "R"
    );


    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        8*UI_SLIDER_WP,
        row2,
        pf_synth_lfo1_frequency,
        "Freq"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        9*UI_SLIDER_WP,
        row2,
        pf_synth_lfo1_amount,
        "Amnt"
    );


    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        10*UI_SLIDER_WP,
        row2,
        pf_synth_lfo1_delay,
        "Dlay"
    );

    ok &= _ui_synth_init_parameter_controller(
        ui,
        pc++,
        12*UI_SLIDER_WP,
        row2,
        pf_synth_master_level,
        "Levl"
    );

    return ok;
}

SDL_Texture *_ui_synth_init_selection_group_texture(UiSynth *ui, int count, char *group_name, char *options[]) {
    SDL_Texture *texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        UI_SELECTION_GROUP_W, (count + 1) * UI_SELECTION_GROUP_LINE_SPACING);
    if (texture == NULL) {
        fprintf(stderr, "Failed to create selection group texture %s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetRenderTarget(ui->renderer, texture);
    _ui_synth_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);
    _ui_synth_set_frame_color(ui);
    for (int i = 0; i < count; i++) {
        font_write(ui->renderer, "( )", 0, i * UI_SELECTION_GROUP_LINE_SPACING);
        font_write(ui->renderer, options[i], 32, i * UI_SELECTION_GROUP_LINE_SPACING);
    }
    font_write(ui->renderer, group_name, 0, count * UI_SELECTION_GROUP_LINE_SPACING);
    SDL_SetRenderTarget(ui->renderer, NULL);
    return texture;
}


bool _ui_synth_init_selection_group(
    UiSynth *ui,
    int index,
    int x,
    int y,
    int count,
    SelectionFunc selection_func,
    char *group_name,
    char *options[]
) {
    SelectionGroup *selection_group = &ui->selection_groups[index];

    selection_group->x = x;
    selection_group->y = y;
    selection_group->w = UI_SELECTION_GROUP_W;
    selection_group->h = (count+1)*UI_SELECTION_GROUP_LINE_SPACING;
    selection_group->count = count;
    selection_group->selection_func = selection_func;
    selection_group->texture = _ui_synth_init_selection_group_texture(ui, count, group_name, options);
    return (selection_group->texture != NULL);
}

bool _ui_synth_init_selection_groups(UiSynth *ui) {
    ui->number_of_selection_groups = 7;
    ui->selection_groups = calloc(ui->number_of_selection_groups, sizeof(SelectionGroup));

    int sg = 0;
    int row1 = 16+2*UI_PANEL_H;
    int row2 = row1 + 6 * UI_SELECTION_GROUP_LINE_SPACING;
    int col = 16;

    bool ok = true;
    char *waveform_options[] = {"Square", "Saw", "Sine", "Noise"};
    ok &= _ui_synth_init_selection_group(
        ui,
        sg++,
        col,
        row1,
        sizeof(waveform_options)/sizeof(char*),
        sf_synth_oscillator1_waveform,
        "Osc 1",
        waveform_options
    );
    ok &= _ui_synth_init_selection_group(
        ui,
        sg++,
        col+UI_SELECTION_GROUP_W,
        row1,
        sizeof(waveform_options)/sizeof(char*),
        sf_synth_oscillator2_waveform,
        "Osc 2",
        waveform_options
    );

    char *combiner_mode_options[] = {"+", "*", "><"};
    ok &= _ui_synth_init_selection_group(
        ui,
        sg++,
        col,
        row2,
        sizeof(combiner_mode_options)/sizeof(char*),
        sf_synth_combiner_mode,
        "Comb mode",
        combiner_mode_options
    );

    char *combiner_strength_mode_options[] = {"Manual", "VCA"};
    ok &= _ui_synth_init_selection_group(
        ui,
        sg++,
        col+UI_SELECTION_GROUP_W,
        row2,
        sizeof(combiner_strength_mode_options)/sizeof(char*),
        sf_synth_combiner_oscillator2_strength_mode,
        "Comb bal",
        combiner_strength_mode_options
    );

    char *vca_settings[] = {"Normal", "Inverse"};
    ok &= _ui_synth_init_selection_group(
        ui,
        sg++,
        col+2*UI_SELECTION_GROUP_W,
        row2,
        sizeof(vca_settings)/sizeof(char*),
        sf_synth_filter_vca_inverse,
        "FltrVCA",
        vca_settings
    );

    ok &= _ui_synth_init_selection_group(
        ui,
        sg++,
        col+3*UI_SELECTION_GROUP_W,
        row2,
        sizeof(vca_settings)/sizeof(char*),
        sf_synth_combiner_vca_inverse,
        "CombVCA",
        vca_settings
    );

    char *off_on[] = {"Off", "On"};
    ok &= _ui_synth_init_selection_group(
        ui,
        sg++,
        col+4*UI_SELECTION_GROUP_W,
        row2,
        sizeof(off_on)/sizeof(char*),
        sf_synth_echo,
        "Echo",
        off_on
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

    if (!_ui_synth_init_selection_groups(ui)) {
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
        if (ui->selection_groups != NULL) {
            for (int i = 0; i < ui->number_of_selection_groups; i++) {
                SelectionGroup *sg = &ui->selection_groups[i];
                if (sg->texture != NULL) {
                    SDL_DestroyTexture(sg->texture);
                }
            }
            free(ui->selection_groups);
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

void _ui_synth_draw_panel(UiSynth *ui, char *title, int x, int y, int w, int h) {
    _ui_synth_draw_frame(ui,x,y+4,w,h-4);
    _ui_synth_set_bg_color(ui);
    SDL_Rect rect = {
        .x = x+3,
        .y = y,
        .w = strlen(title)*8+1,
        .h = 8
    };
    SDL_RenderFillRect(ui->renderer, &rect);
    _ui_synth_set_frame_color(ui);
    font_write(ui->renderer, title, x+4, y);
}

void _ui_synth_draw_background(UiSynth *ui) {
    _ui_synth_draw_frame(ui, 0,0,UI_SYNTH_W, UI_SYNTH_H);
    _ui_synth_draw_panel(ui, "Voice", 0,0,8*UI_SLIDER_W,UI_PANEL_H);
    _ui_synth_draw_panel(ui, "Combiner", 8*UI_SLIDER_W,0,12*UI_SLIDER_W,UI_PANEL_H);
    _ui_synth_draw_panel(ui, "Filter", 0,UI_PANEL_H,11*UI_SLIDER_W,UI_PANEL_H);
    _ui_synth_draw_panel(ui, "Modulation", 11*UI_SLIDER_W,UI_PANEL_H,6*UI_SLIDER_W,UI_PANEL_H);

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

void _ui_synth_draw_selection_group(UiSynth *ui, Synth *synth, SelectionGroup *sg, bool active) {
    int x = sg->x;
    int y = sg->y;
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = sg->w,
        .h = sg->h,
    };
    SDL_RenderCopy(ui->renderer, sg->texture, NULL, &rect);
    int *value = sg->selection_func(synth);
    rect.x = x+8;
    rect.y = *value * UI_SELECTION_GROUP_LINE_SPACING + y;
    rect.w = 6;
    rect.h = 7;
    if (active) {
        _ui_synth_set_main_color(ui);
    } else {
        _ui_synth_set_frame_color(ui);
    }
    SDL_RenderFillRect(ui->renderer, &rect);
}

void _ui_synth_draw_sliders(UiSynth *ui, Synth *synth) {
    for (int i = 0; i < ui->number_of_parameter_controllers; i++) {
        ParameterController *pc = &ui->parameter_controllers[i];
        _ui_synth_draw_slider(ui, synth, pc, i == ui->current_parameter);
    }
}

void _ui_synth_draw_selection_groups(UiSynth *ui, Synth *synth) {
    for (int i = 0; i < ui->number_of_selection_groups; i++) {
        SelectionGroup *sg = &ui->selection_groups[i];
        _ui_synth_draw_selection_group(ui, synth, sg, i == ui->current_parameter-ui->number_of_parameter_controllers);
    }
}

void ui_synth_render(UiSynth *ui, Synth *synth, int x, int y) {
    SDL_SetRenderTarget(ui->renderer, ui->texture);
    _ui_synth_set_bg_color(ui);
    SDL_RenderClear(ui->renderer);
    _ui_synth_draw_background(ui);
    _ui_synth_draw_sliders(ui, synth);
    _ui_synth_draw_selection_groups(ui,synth);
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
            return;
        }
    }
    for (int i = 0; i < ui->number_of_selection_groups; i++) {
        SelectionGroup *sg =&ui->selection_groups[i];
        if (x >= sg->x && x <= sg->x + sg->w &&
            y >= sg->y && y < sg->y + sg->h - UI_SELECTION_GROUP_LINE_SPACING) {
            ui->current_parameter = i + ui->number_of_parameter_controllers;
            int v = (y-sg->y) / UI_SELECTION_GROUP_LINE_SPACING;
            *sg->selection_func(synth) = v;
        }
    }
}

void ui_synth_alter_parameter(UiSynth *ui, Synth *synth, double delta) {
    if (ui->current_parameter >= ui->number_of_parameter_controllers) {
        int step = 1;
        if (delta > 0) {
            step = -1;
        }
        SelectionGroup *sg = &ui->selection_groups[ui->current_parameter-ui->number_of_parameter_controllers];
        int *v = sg->selection_func(synth);
        int n = *v + step;
        if (n < 0) {
            n = sg->count-1;
        }
        if (n >= sg->count) {
            n = 0;
        }
        *v = n;
    } else {
        ParameterController *pc = &ui->parameter_controllers[ui->current_parameter];
        double *v = pc->parameter_func(synth);
        *v = fmin(fmax(*v + delta,0), 1);
    }
}

void ui_synth_next_parameter(UiSynth *ui) {
    ui->current_parameter = (ui->current_parameter + 1) % (ui->number_of_parameter_controllers + ui->number_of_selection_groups);
}

void ui_synth_prev_parameter(UiSynth *ui) {
    int n = ui->current_parameter - 1;
    if (n < 0) {
        n = ui->number_of_parameter_controllers + ui->number_of_selection_groups - 1;
    }
    ui->current_parameter = n;
}

