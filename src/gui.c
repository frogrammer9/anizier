#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "gui.h"

int gui_init(gui_hndl* gui, window_hndl* win) {
	gui->ctx = nk_glfw3_init(&gui->glfw, win->win, NK_GLFW3_INSTALL_CALLBACKS);
	nk_glfw3_font_stash_begin(&gui->glfw, &gui->fatlas);
	nk_glfw3_font_stash_end(&gui->glfw);
	return 0;
}

void gui_terminate(gui_hndl* gui) {
	nk_glfw3_shutdown(&gui->glfw);
}

