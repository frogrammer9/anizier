#ifndef GUI_H
#define GUI_H

#include <GL/glew.h>
#include "window.h"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_MAX_VERTEX_BUFFER	512 * 1024
#define NK_MAX_ELEMENT_BUFFER	128 * 1024
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

typedef struct {
	struct nk_context* ctx;
	struct nk_glfw glfw;
	struct nk_font_atlas* fatlas;
	GLFWwindow* win;
} gui_hndl;

#define GUI_NEW_FRAME(gui) nk_glfw3_new_frame(&gui.glfw)
#define GUI_RENDER(gui) nk_glfw3_render(&gui.glfw, NK_ANTI_ALIASING_ON, NK_MAX_VERTEX_BUFFER, NK_MAX_ELEMENT_BUFFER)

int gui_init(gui_hndl* gui, window_hndl* win);
void gui_terminate(gui_hndl* gui);
void gui_render(gui_hndl* gui);

#endif
