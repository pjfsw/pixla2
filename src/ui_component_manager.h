#ifndef SRC_UI_COMPONENT_MANAGER_H_
#define SRC_UI_COMPONENT_MANAGER_H_

#define UI_CMGR_MAX_COMPONENTS 100
#define UI_CMGR_MAX_GROUPS 10

#include <stdlib.h>
#include <SDL2/SDL.h>

#include "synth.h"

typedef double* (*UiParameterFunc)(Synth *synth);

typedef int* (*UiSelectionFunc)(Synth *synth);

typedef struct {
    UiParameterFunc parameter_func;
} UiParameterController;

typedef struct {
    int count;
    UiSelectionFunc selection_func;
} UiSelectionGroup;

typedef struct {
    int number_of_components;
    SDL_Texture *texture;
    int x;
    int y;
    int w;
    int h;
    int next_x;
} UiComponentGroup;

typedef enum {
    UI_COMP_PARAMETER_CONTROLLER,
    UI_COMP_SELECTION_GROUP
} UiComponentType;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    SDL_Texture *texture;
    UiComponentType type;
    UiParameterController pc;
    UiSelectionGroup sg;
    UiComponentGroup *cg;
} UiComponent;

typedef struct {
    int number_of_groups;
    int number_of_components;
    int current_component;
    UiComponent *components;
    UiComponentGroup *groups;
    SDL_Renderer *renderer;
    bool error;
} UiComponentManager;

UiComponentManager *ui_cmgr_create(SDL_Renderer *renderer);

void ui_cmgr_destroy(UiComponentManager *cmgr);

void ui_cmgr_render(UiComponentManager *cmgr, void *user_data);

UiComponentGroup *ui_cmgr_component_group(
    UiComponentManager *cmgr,
    char *title,
    int x, int y,
    int w, int h
);

void ui_cmgr_add_parameter(
    UiComponentManager *cmgr,
    UiComponentGroup *group,
    char *title,
    UiParameterFunc parameter_func
);

void ui_cmgr_add_selection(
    UiComponentManager *cmgr,
    UiComponentGroup *group,
    char *title,
    char *options[],
    int count,
    UiSelectionFunc select_func
);

bool ui_cmgr_is_error(UiComponentManager *cmgr);

void ui_cmgr_click(UiComponentManager *cmgr, void *user_data, int x, int y);

#endif /* SRC_UI_COMPONENT_MANAGER_H_ */
