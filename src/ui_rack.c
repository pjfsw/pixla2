#include <stdio.h>
#include <stdbool.h>

#include "ui_rack.h"
#include "font.h"
#include "ui_colors.h"

#define RACK_INSTR_H 24
#define RACK_ITEM_OFFSET_X RACK_INSTR_W
#define RACK_ITEM_OFFSET_Y 0
#define RACK_INSTRUMENT_SPACING RACK_INSTR_H

SDL_Texture *_ui_rack_create_tab_texture(UiRack *ui, char *text) {
    SDL_Texture *texture = SDL_CreateTexture(
        ui->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        RACK_INSTR_W,
        RACK_INSTR_H);
    if (texture == NULL) {
        fprintf(stderr, "Failed to create texture %s\n", SDL_GetError());
        return NULL;
    }
    SDL_SetRenderTarget(ui->renderer, texture);
    ui_colors_set(ui->renderer, ui_colors_synth_bg());
    SDL_RenderClear(ui->renderer);
    ui_colors_set(ui->renderer, ui_colors_synth_frame());
    SDL_Rect rect = {
        .x = 0,
        .y = 0,
        .w = RACK_INSTR_W,
        .h = RACK_INSTR_H
    };
    SDL_RenderDrawRect(ui->renderer, &rect);
    ui_colors_set(ui->renderer, ui_colors_synth_main());
    font_write_scale(ui->renderer, text, 2, 4, 2);
    SDL_SetRenderTarget(ui->renderer, NULL);

    return texture;

}

SDL_Texture *_ui_rack_create_instrument_texture(UiRack *ui, int i, char *text) {
    char s[10];
    sprintf(s, "%s %d", text, i);
    return _ui_rack_create_tab_texture(ui, s);
}

UiRack *ui_rack_create(SDL_Renderer *renderer) {
    UiRack *ui = calloc(1, sizeof(UiRack));
    ui->renderer = renderer;
    ui->ui_instrument = ui_instrument_create(renderer);
    if (ui->ui_instrument == NULL) {
        ui_rack_destroy(ui);
        return NULL;
    }
    ui->ui_mixer = ui_mixer_create(renderer);
    if (ui->ui_mixer == NULL) {
        ui_rack_destroy(ui);
        return NULL;
    }
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        ui->synth_textures[i] = _ui_rack_create_instrument_texture(ui, i, "Synth");
        if (ui->synth_textures[i] == NULL) {
            ui_rack_destroy(ui);
            return NULL;
        }
        ui->sampler_textures[i] = _ui_rack_create_instrument_texture(ui, i, "Sampl");
        if (ui->sampler_textures[i] == NULL) {
            ui_rack_destroy(ui);
            return NULL;
        }
    }
    ui->mixer_texture = _ui_rack_create_tab_texture(ui, "Mixer");
    if (ui->mixer_texture == NULL) {
        ui_rack_destroy(ui);
        return NULL;
    }

    return ui;
}

void ui_rack_destroy(UiRack *ui) {
    if (ui == NULL) {
        return;
    }

    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        if (ui->synth_textures[i] != NULL) {
            SDL_DestroyTexture(ui->synth_textures[i]);
        }
        if (ui->sampler_textures[i] != NULL) {
            SDL_DestroyTexture(ui->sampler_textures[i]);
        }
    }
    if (ui->mixer_texture != NULL) {
        SDL_DestroyTexture(ui->mixer_texture);
    }
    if (ui->ui_instrument != NULL) {
        ui_instrument_destroy(ui->ui_instrument);
    }
    if (ui->ui_mixer != NULL) {
        ui_mixer_destroy(ui->ui_mixer);
    }

    free(ui);
}

void ui_rack_set_mode(UiRack *ui, UiRackMode mode) {
    ui->mode = mode;
}

void _ui_render_tab(UiRack *ui, Rack *rack, int x, int y, SDL_Texture *texture, int tab, bool highlight) {
    SDL_Rect rect = {
        .x = 0,
        .y = RACK_ITEM_OFFSET_Y + tab * RACK_INSTR_H,
        .w = RACK_INSTR_W,
        .h = RACK_INSTR_H
    };

    SDL_RenderCopy(ui->renderer, texture, NULL, &rect);
    if (highlight) {
        SDL_SetRenderDrawColor(ui->renderer, 255,255,255,255);
        SDL_RenderDrawRect(ui->renderer, &rect);
    }
}

void _ui_rack_render_instruments(UiRack *ui, Rack *rack, int x, int y) {
    Instrument *instrument = &rack->instruments[ui->current_instrument];
    ui_instrument_render(ui->ui_instrument, instrument, x+RACK_ITEM_OFFSET_X, y+RACK_ITEM_OFFSET_Y);

    char s[2];
    for (int i = 0; i < NUMBER_OF_INSTRUMENTS; i++) {
        s[0] = i + 48;
        s[1] = 0;
        SDL_Texture *texture = rack->instruments[i].type == INSTR_SYNTH
            ? ui->synth_textures[i] : ui->sampler_textures[i];
        _ui_render_tab(ui, rack, x,y, texture, i, i == ui->current_instrument);
    }
}

void _ui_rack_render_effects(UiRack *ui, Rack *rack, int x, int y) {
    ui_mixer_render(ui->ui_mixer, rack->mixer, x+RACK_ITEM_OFFSET_X, y+RACK_ITEM_OFFSET_Y);
    _ui_render_tab(ui, rack, x,y, ui->mixer_texture, 0, true);
}

void ui_rack_render(UiRack *ui, Rack *rack, int x, int y) {
    if (ui->mode == UI_RACK_INSTRUMENT) {
        _ui_rack_render_instruments(ui, rack, x, y);
    } else {
        _ui_rack_render_effects(ui, rack, x, y);
    }
}

void ui_rack_click(UiRack *ui, Rack *rack, int x, int y) {
    if (x > RACK_ITEM_OFFSET_X && y > RACK_ITEM_OFFSET_Y) {
        x -= RACK_ITEM_OFFSET_X;
        y -= RACK_ITEM_OFFSET_Y;
        if (ui->mode == UI_RACK_INSTRUMENT) {
            Instrument *instrument = &rack->instruments[ui->current_instrument];
            ui_instrument_click(ui->ui_instrument, instrument, x, y);
        } else if (ui->mode == UI_RACK_MIXER) {
            ui_mixer_click(ui->ui_mixer, rack->mixer, x, y);
        }
    } else if (x < RACK_ITEM_OFFSET_X && y > RACK_ITEM_OFFSET_Y) {
        ui->current_instrument = (y-RACK_ITEM_OFFSET_Y)/RACK_INSTRUMENT_SPACING;
        if (ui->current_instrument > NUMBER_OF_INSTRUMENTS-1) {
            ui->current_instrument = NUMBER_OF_INSTRUMENTS-1;
        }
    }
}

void ui_rack_alter_parameter(UiRack *ui, Rack *rack, double delta) {
    Instrument *instrument = &rack->instruments[ui->current_instrument];
    ui_instrument_alter_parameter(ui->ui_instrument, instrument, delta);
}

void ui_rack_next_parameter(UiRack *ui) {
    ui_instrument_next_parameter(ui->ui_instrument);
}

void ui_rack_prev_parameter(UiRack *ui) {
    ui_instrument_prev_parameter(ui->ui_instrument);
}

void ui_rack_prev_instrument(UiRack *ui) {
    if (ui->current_instrument > 0) {
        ui->current_instrument--;
    }
}

void ui_rack_next_instrument(UiRack *ui) {
    if (ui->current_instrument < NUMBER_OF_INSTRUMENTS-1) {
        ui->current_instrument++;
    }
}




