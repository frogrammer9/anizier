#include "app.h"
#include "gui.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int app_init(application_hndl* app) {
	if(window_init(&app->window, 640, 480, "Anizier")) return 1;
	if(gui_init(&app->gui, &app->window)) return 1;
	return 0;
}

int app_run(application_hndl* app) {
	while(!glfwWindowShouldClose(app->window.win)) {


		GUI_NEW_FRAME(app->gui);
		if(nk_begin(app->gui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New animation")) {
				printf("Button pressed\n");
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Edit animation")) {
				printf("Button pressed\n");
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Run animation")) {
				printf("Button pressed\n");
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Exit")) {
				glfwSetWindowShouldClose(app->window.win, true);
			}

		}
		nk_end(app->gui.ctx);
		GUI_RENDER(app->gui);


		window_FEP(&app->window);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	return 0;
}

void app_terminate(application_hndl* app) {
	gui_terminate(&app->gui);
	window_terminate(&app->window);
}
