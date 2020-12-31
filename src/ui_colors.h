#ifndef SRC_UI_COLORS_H_
#define SRC_UI_COLORS_H_

#include <SDL2/SDL.h>

SDL_Color *ui_colors_synth_frame();

SDL_Color *ui_colors_synth_main();

SDL_Color *ui_colors_synth_highlight();

SDL_Color *ui_colors_synth_bg();

SDL_Color *ui_colors_sequencer_status();

SDL_Color *ui_colors_sequencer_highlight();

void ui_colors_set(SDL_Renderer *renderer, SDL_Color *color);

#endif /* SRC_UI_COLORS_H_ */
