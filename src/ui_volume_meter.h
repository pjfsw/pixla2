#ifndef SRC_UI_VOLUME_METER_H_
#define SRC_UI_VOLUME_METER_H_

#include <SDL2/SDL.h>

typedef struct {
    SDL_Renderer *renderer;
} UiVolumeMeter;

UiVolumeMeter *ui_volume_meter_create(SDL_Renderer *renderer);

void ui_volume_meter_destroy(UiVolumeMeter *ui);

void ui_volume_meter_render(UiVolumeMeter *ui, double loudness, int *history, int x, int y);

#endif /* SRC_UI_VOLUME_METER_H_ */
