#ifndef SRC_SETTINGS_FUNC_H_
#define SRC_SETTINGS_FUNC_H_

#include <SDL2/SDL.h>

typedef Uint8* (*UiParameterFunc)(void *user_data);

typedef int* (*UiSelectionFunc)(void *user_data);

#endif /* SRC_SETTINGS_FUNC_H_ */
