#include "app.h"
#include "gui.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"
#include <stdio.h>

int app_init(application_hndl* app) {
	if(window_init(&app->window, 640, 480, "Anizier")) return 1;
	if(gui_init(&app->gui, &app->window)) return 1;
	app->shader = shader_init("basic.glsl"); //If this fails it calls exit(1)
	return 0;
}

int app_run(application_hndl* app) {

	rnBuffer buff;
	rnBuffer_init(&buff, true);
	sample l1[3] = {
		{.pos = {.x = 0.f, .y = 0.f}, .col = 0xaabbccff},
		{.pos = {.x = 10.f, .y = 10.f}, .col = 0xaabbccff},
		{.pos = {.x = 30.f, .y = 30.f}, .col = 0xaabbccff}
	};
	sample l2[3] = {
		{.pos = {.x = 21.f, .y = 37.f}, .col = 0xaabbccff},
		{.pos = {.x = 11.f, .y = 22.f}, .col = 0xaabbccff},
	};
	rnBuffer_add_curve(&buff, l1, 3);
	rnBuffer_add_curve(&buff, l2, 2);
	rnBuffer_add_curve(&buff, l1, 3);
	rnBuffer_new_frame(&buff);

	while(!glfwWindowShouldClose(app->window.win)) {

		rnBuffer_render(&buff, app->shader, 1, 30);

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
	rnBuffer_terminate(&buff);
	return 0;
}

void app_terminate(application_hndl* app) {
	shader_terminate(&app->shader);
	gui_terminate(&app->gui);
	window_terminate(&app->window);
}
