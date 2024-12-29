#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "gui.h"
#include <GLFW/glfw3.h>

int gui_init(gui_hndl* gui, window_hndl* win) {
	gui->glfw.text_len = 0;
	gui->ctx = nk_glfw3_init(&gui->glfw, win->win, NK_GLFW3_INSTALL_CALLBACKS);
	nk_glfw3_font_stash_begin(&gui->glfw, &gui->fatlas);
	nk_glfw3_font_stash_end(&gui->glfw);
	return 0;
}

void gui_terminate(gui_hndl* gui) {
	nk_glfw3_shutdown(&gui->glfw);
}

void gui_text(gui_hndl* gui, cstr text) {
    struct nk_command_buffer *canvas = nk_window_get_canvas(gui->ctx);
    struct nk_rect rect = nk_rect(50, 50, 200, 30); 
    struct nk_color text_color = nk_rgb(255, 255, 255); 
    struct nk_color bg_color = nk_rgba(0, 0, 0, 0); 
    const struct nk_user_font *font = gui->ctx->style.font;
    nk_draw_text(canvas, rect, text, strlen(text), font, bg_color, text_color);
}
