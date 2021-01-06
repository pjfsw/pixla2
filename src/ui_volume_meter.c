#include "ui_volume_meter.h"

#define _UI_VOLUME_METER_BASE_DB 32

UiVolumeMeter *ui_volume_meter_create(SDL_Renderer *renderer) {
    UiVolumeMeter *ui = calloc(1, sizeof(UiVolumeMeter));
    ui->renderer = renderer;
    return ui;
}

void ui_volume_meter_destroy(UiVolumeMeter *ui) {
    if (ui != NULL) {
        free(ui);
    }
}

void ui_volume_meter_render(UiVolumeMeter *ui, double loudness, int *history, int x, int y) {
    SDL_Rect rect = {
        .x = x,
        .y = y
    };
    rect.w = 10;
    rect.h = 3;
    int bar_spacing = rect.h+1;
    int currentloud = loudness > 0 ? 0.5 + 20 * log10(loudness) : -1000;
    currentloud+=_UI_VOLUME_METER_BASE_DB;

    if (currentloud > *history) {
        *history = currentloud;
    } else if (*history > -1) {
        (*history)--;
    }

    int y_ofs = (_UI_VOLUME_METER_BASE_DB-1) * bar_spacing;
    int opacity = 255;
    int r = 0;
    int g = 255;
    for (int i = 0; i < _UI_VOLUME_METER_BASE_DB; i++) {
        if (i > *history) {
            opacity = 32;
        }
        if (i >= _UI_VOLUME_METER_BASE_DB-3) {
            r = 255;
            g = 0;
        }
        else if (i >= _UI_VOLUME_METER_BASE_DB-6) {
            r = 255;
            g = 255;
        }
        rect.y=y + y_ofs;
        y_ofs-=bar_spacing;

        SDL_SetRenderDrawColor(ui->renderer,r,g,0,opacity);
        SDL_RenderFillRect(ui->renderer, &rect);
    }
}
