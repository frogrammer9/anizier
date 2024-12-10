#ifndef GUI_H
#define GUI_H

#include "window.h"

typedef struct {
	GLFWwindow* win;
} gui_hndl;

int gui_init(gui_hndl* gui, window_hndl* win);
void gui_terminate(gui_hndl* gui);
void gui_render(gui_hndl* gui);

#endif
