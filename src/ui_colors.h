#ifndef SRC_UI_COLORS_H_
#define SRC_UI_COLORS_H_

#include <SDL2/SDL.h>

SDL_Color *ui_colors_synth_frame();

SDL_Color *ui_colors_synth_main();

SDL_Color *ui_colors_synth_bg();

void ui_colors_set(SDL_Renderer *renderer, SDL_Color *color);

#endif /* SRC_UI_COLORS_H_ */
