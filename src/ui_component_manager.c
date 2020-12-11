#include "ui_component_manager.h"
#include "ui_colors.h"
#include "font.h"

#define _UI_SELECTION_GROUP_W 88
#define _UI_SELECTION_GROUP_LINE_SPACING 10

#define _UI_SLIDER_W 32
#define _UI_SLIDER_WP 48
#define _UI_SLIDER_H 100
#define _UI_SLIDER_TH (_UI_SLIDER_H+9)

UiComponentManager *ui_cmgr_create(SDL_Renderer *renderer) {
    UiComponentManager *cmgr = calloc(1, sizeof(UiComponentManager));
    cmgr->renderer = renderer;
    cmgr->components = calloc(UI_CMGR_MAX_COMPONENTS, sizeof(UiComponent));
    cmgr->groups = calloc(UI_CMGR_MAX_GROUPS, sizeof(UiComponentGroup));
    return cmgr;
}

void ui_cmgr_destroy(UiComponentManager *cmgr) {
    if (cmgr == NULL) {
        return;
    }
    if (cmgr->components != NULL) {
        for (int i = 0; i < cmgr->number_of_components; i++) {
            if (cmgr->components[i].texture != NULL) {
                SDL_DestroyTexture(cmgr->components[i].texture);
            }
        }
        free(cmgr->components);
    }
    if (cmgr->groups != NULL) {
        for (int i = 0; i < cmgr->number_of_groups; i++) {
            if (cmgr->groups[i].texture != NULL) {
                SDL_DestroyTexture(cmgr->groups[i].texture);
            }
        }
        free(cmgr->groups);
    }
    free(cmgr);
}

void _ui_cmgr_draw_frame(SDL_Renderer *renderer, int x, int y, int w, int h) {
    ui_colors_set(renderer, ui_colors_synth_frame());
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    SDL_RenderDrawRect(renderer, &rect);
}

UiComponent *_ui_cmgr_create_component(UiComponentManager *cmgr, UiComponentGroup *group, int width, int height) {
    if (cmgr->number_of_components == UI_CMGR_MAX_COMPONENTS) {
        return NULL;
    }
    cmgr->number_of_components++;
    UiComponent *component = &cmgr->components[cmgr->number_of_components-1];
    component->x = group->next_x;
    component->y = 0;
    component->w = width;
    component->h = height;
    component->cg = group;
    group->next_x += width + 4;
    return component;
}

UiComponentGroup *ui_cmgr_component_group(
    UiComponentManager *cmgr,
    char *title,
    int x, int y,
    int w, int h
) {
    if (cmgr == NULL || cmgr->error || title == NULL  || cmgr->number_of_groups >= UI_CMGR_MAX_GROUPS) {
        if (cmgr != NULL) {
            cmgr->error |= true;
        }
        return NULL;
    }

    cmgr->number_of_groups++;
    UiComponentGroup *group = &cmgr->groups[cmgr->number_of_groups-1];
    group->x = x;
    group->y = y;
    group->w = w;
    group->h = h;

    group->texture = SDL_CreateTexture(
        cmgr->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h);
    if (group->texture == NULL) {
        fprintf(stderr, "Failed to create slider_texture %s\n", SDL_GetError());
        cmgr->error |= true;
        return NULL;
    }

    SDL_SetRenderTarget(cmgr->renderer, group->texture);
    ui_colors_set(cmgr->renderer, ui_colors_synth_bg());
    SDL_RenderClear(cmgr->renderer);

    _ui_cmgr_draw_frame(cmgr->renderer,0,4,w,h-4);
    ui_colors_set(cmgr->renderer, ui_colors_synth_bg());
    SDL_Rect rect = {
        .x = 3,
        .y = 0,
        .w = strlen(title)*8+1,
        .h = 8
    };
    SDL_RenderFillRect(cmgr->renderer, &rect);
    ui_colors_set(cmgr->renderer, ui_colors_synth_frame());
    font_write(cmgr->renderer, title, 4, 0);
    SDL_SetRenderTarget(cmgr->renderer, NULL);
    return group;
}

void ui_cmgr_add_parameter(
    UiComponentManager *cmgr,
    UiComponentGroup *group,
    char *title,
    UiParameterFunc parameter_func
) {
    if (cmgr == NULL || cmgr->error || group == NULL || title == NULL || parameter_func == NULL) {
        if (cmgr != NULL) {
            cmgr->error |= true;
        }
        return;
    }

    UiComponent *component = _ui_cmgr_create_component(cmgr, group, _UI_SLIDER_W, _UI_SLIDER_TH);
    component->type = UI_COMP_PARAMETER_CONTROLLER;
    component->pc.parameter_func = parameter_func;

    component->texture = SDL_CreateTexture(
        cmgr->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        _UI_SLIDER_W, _UI_SLIDER_TH);
    if (component->texture == NULL) {
        fprintf(stderr, "Failed to create slider_texture %s\n", SDL_GetError());
        cmgr->error |= true;
        return;
    }

    SDL_SetRenderTarget(cmgr->renderer, component->texture);
    ui_colors_set(cmgr->renderer, ui_colors_synth_bg());
    SDL_RenderClear(cmgr->renderer);
    ui_colors_set(cmgr->renderer, ui_colors_synth_frame());

    SDL_Rect rect = {
        .x = 0,
        .y = 0,
        .w = _UI_SLIDER_W,
        .h = _UI_SLIDER_H
    };
    SDL_RenderDrawRect(cmgr->renderer, &rect);
    for (int i = 0; i < 10; i++) {
        int w = _UI_SLIDER_W/3;
        if (i == 5) {
            w = _UI_SLIDER_W/2;
        }
        int x = (_UI_SLIDER_W-w)/2;
        SDL_RenderDrawLine(cmgr->renderer, x,i*10,x+w,i*10);
    }
    font_write(cmgr->renderer, title, 0, _UI_SLIDER_H+1);
    SDL_SetRenderTarget(cmgr->renderer, NULL);
}

void ui_cmgr_add_selection(
    UiComponentManager *cmgr,
    UiComponentGroup *group,
    char *title,
    char *options[],
    int count,
    UiSelectionFunc select_func
) {
    if (cmgr == NULL || cmgr->error || group == NULL || title == NULL || options == NULL || select_func == NULL) {
        if (cmgr != NULL) {
            cmgr->error |= true;
        }
        return;
    }

    int w = _UI_SELECTION_GROUP_W;
    int h = (count + 1) * _UI_SELECTION_GROUP_LINE_SPACING;

    UiComponent *component = _ui_cmgr_create_component(cmgr, group, w, h);
    component->type = UI_COMP_SELECTION_GROUP;
    component->sg.count = count;
    component->sg.selection_func = select_func;

    component->texture =  SDL_CreateTexture(
        cmgr->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w,h);

    if (component->texture == NULL) {
        fprintf(stderr, "Failed to create selection group texture %s\n", SDL_GetError());
        cmgr->error |= true;
        return;
    }

    SDL_SetRenderTarget(cmgr->renderer, component->texture);
    ui_colors_set(cmgr->renderer, ui_colors_synth_bg());
    SDL_RenderClear(cmgr->renderer);
    ui_colors_set(cmgr->renderer, ui_colors_synth_frame());
    for (int i = 0; i < count; i++) {
        font_write(cmgr->renderer, "( )", 0, i * _UI_SELECTION_GROUP_LINE_SPACING);
        font_write(cmgr->renderer, options[i], 32, i * _UI_SELECTION_GROUP_LINE_SPACING);
    }
    font_write(cmgr->renderer, title, 0, count * _UI_SELECTION_GROUP_LINE_SPACING);
    SDL_SetRenderTarget(cmgr->renderer, NULL);
    return;
}

/**
 *  Returns true if an error occurred during component creation
 */
bool ui_cmgr_is_error(UiComponentManager *cmgr) {
    return cmgr->error;

}

int _ui_cmgr_get_component_render_y(UiComponent *component) {
    return component->cg->y + component->y + 12;
}

int _ui_cmgr_get_component_render_x(UiComponent *component) {
    return component->cg->x + component->x + 12;
}

void _ui_cmgr_draw_parameter_controller(UiComponentManager *cmgr, UiComponent *component, void *user_data, bool active) {
    int x = _ui_cmgr_get_component_render_x(component);
    int y = _ui_cmgr_get_component_render_y(component);
    double *value = component->pc.parameter_func(user_data);
    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = _UI_SLIDER_W,
        .h = _UI_SLIDER_TH
    };
    double v = *value;
    v = fmin(v, 1);
    v = fmax(v, 0);

    SDL_RenderCopy(cmgr->renderer, component->texture, NULL, &rect);

    int h = _UI_SLIDER_H-1;
    int yPos = h - v * (double)h;
    rect.h = h;

    if (active) {
        ui_colors_set(cmgr->renderer, ui_colors_synth_main());
    } else {
        ui_colors_set(cmgr->renderer, ui_colors_synth_frame());
    }

    rect.x=x+1;
    rect.y=y+yPos;
    rect.w=_UI_SLIDER_W-2;
    rect.h= h-yPos;
    SDL_RenderFillRect(cmgr->renderer, &rect);
}

void _ui_cmgr_draw_selection_group(UiComponentManager *cmgr, UiComponent *component, void *user_data, bool active) {
    int x = _ui_cmgr_get_component_render_x(component);
    int y = _ui_cmgr_get_component_render_y(component);

    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = component->w,
        .h = component->h,
    };
    SDL_RenderCopy(cmgr->renderer, component->texture, NULL, &rect);
    int *value = component->sg.selection_func(user_data);
    rect.x = x+8;
    rect.y = *value * _UI_SELECTION_GROUP_LINE_SPACING + y;
    rect.w = 6;
    rect.h = 7;
    if (active) {
        ui_colors_set(cmgr->renderer, ui_colors_synth_main());
    } else {
        ui_colors_set(cmgr->renderer, ui_colors_synth_frame());
    }
    SDL_RenderFillRect(cmgr->renderer, &rect);
}

void _ui_cmgr_draw_components(UiComponentManager *cmgr, void *user_data) {
    for (int i = 0; i < cmgr->number_of_components; i++) {
        bool active = i == cmgr->current_component;
        if (cmgr->components[i].type == UI_COMP_PARAMETER_CONTROLLER) {
            _ui_cmgr_draw_parameter_controller(cmgr, &cmgr->components[i], user_data, active);
        } else if (cmgr->components[i].type == UI_COMP_SELECTION_GROUP) {
            _ui_cmgr_draw_selection_group(cmgr, &cmgr->components[i], user_data, active);
        }
    }
}

void _ui_cmgr_draw_groups(UiComponentManager *cmgr) {
    SDL_Rect rect;
    for (int i = 0; i < cmgr->number_of_groups; i++) {
        UiComponentGroup *group = &cmgr->groups[i];
        rect.x = group->x;
        rect.y = group->y;
        rect.w = group->w;
        rect.h = group->h;
        SDL_RenderCopy(cmgr->renderer, group->texture, NULL, &rect);
    }
}

void ui_cmgr_render(UiComponentManager *cmgr, void *user_data) {
    _ui_cmgr_draw_groups(cmgr);
    _ui_cmgr_draw_components(cmgr, user_data);
}

void ui_cmgr_click(UiComponentManager *cmgr, void *user_data, int x, int y) {
    for (int i = 0; i < cmgr->number_of_components; i++) {
        UiComponent *component = &cmgr->components[i];
        int cx = _ui_cmgr_get_component_render_x(component);
        int cy = _ui_cmgr_get_component_render_y(component);
        int cw = component->w;
        int ch = component->h;
        if (x >= cx && x <= cx + cw &&
            y >= cy && y <= cy + ch) {
            if (component->type == UI_COMP_PARAMETER_CONTROLLER) {
                cmgr->current_component = i;
                double v = (double)(ch - y + cy) * 0.01;
                *component->pc.parameter_func(user_data) = v;
                return;
            } else if (component->type == UI_COMP_SELECTION_GROUP) {
                cmgr->current_component = i;
                int v = (y-cy) / _UI_SELECTION_GROUP_LINE_SPACING;
                if (v < 0) {
                    v = 0;
                }
                if (v > component->sg.count-1) {
                    v = component->sg.count-1;
                }
                *component->sg.selection_func(user_data) = v;
            }
        }
    }

}
