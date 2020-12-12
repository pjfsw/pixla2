#include "ui_rack.h"
#include "font.h"

UiRack *ui_rack_create(SDL_Renderer *renderer) {
    UiRack *ui = calloc(1, sizeof(UiRack));
    ui->renderer = renderer;
    ui->ui_instrument = ui_instrument_create(renderer);
    if (ui->ui_instrument == NULL) {
        ui_rack_destroy(ui);
        return NULL;
    }
    return ui;
}

void ui_rack_destroy(UiRack *ui) {
    if (ui == NULL) {
        return;
    }

    if (ui->ui_instrument != NULL) {
        ui_instrument_destroy(ui->ui_instrument);
    }

    free(ui);
}

void ui_rack_render(UiRack *ui, Rack *rack, int x, int y) {
    Instrument *instrument = &rack->instruments[ui->current_instrument];
    ui_instrument_render(ui->ui_instrument, instrument, x, y);
    char s[2];
    s[0] = ui->current_instrument + 49;
    s[1] = 0;
    font_write(ui->renderer, s, x,y);
}

void ui_rack_click(UiRack *ui, Rack *rack, int x, int y) {
    Instrument *instrument = &rack->instruments[ui->current_instrument];
    ui_instrument_click(ui->ui_instrument, instrument, x, y);
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
    ui->current_instrument--;
    if (ui->current_instrument < 0) {
        ui->current_instrument = 0;
    }
}

void ui_rack_next_instrument(UiRack *ui) {
    ui->current_instrument++;
    if (ui->current_instrument > NUMBER_OF_INSTRUMENTS-1) {
        ui->current_instrument = NUMBER_OF_INSTRUMENTS-1;
    }
}



