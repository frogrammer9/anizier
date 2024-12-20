#include "app.h"
#include "gui.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"
#include "editor.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int shaderWinSizeUniLoc = 0;

int app_init(application_hndl* app) {
	if(window_init(&app->window, 640, 480, "Anizier")) return 1;
	if(gui_init(&app->gui, &app->window)) return 1;
	app->shader = shader_init("basic.glsl"); //If this fails it calls exit(1)
	shaderWinSizeUniLoc = glGetUniformLocation(app->shader, "uWinSize");
	rnBuffer_init(&app->buffer, true);
	return 0;
}

int app_run(application_hndl* app) {
	int WinSw, WinSh;
	bool runEditor = false;

	while(!glfwWindowShouldClose(app->window.win)) {

		shader_bind(app->shader);
		glfwGetWindowSize(app->window.win, &WinSw, &WinSh);
		glUniform2f(shaderWinSizeUniLoc, WinSw, WinSh);
		shader_bind(0);

		GUI_NEW_FRAME(app->gui);
		if(nk_begin(app->gui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New animation")) {
				runEditor = true;
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Edit animation")) {
				runEditor = true;
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

		if(runEditor) {
			runEditor = false;
			editor_run(app);
		}

		window_FEP(&app->window);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	return 0;
}

void app_terminate(application_hndl* app) {
	rnBuffer_terminate(&app->buffer);
	shader_terminate(&app->shader);
	gui_terminate(&app->gui);
	window_terminate(&app->window);
}

vec2 getMousePosGL(application_hndl* app) {
	vec2 pos;
	int ww = 0, wh = 0;
	glfwGetWindowSize(app->window.win, &ww, &wh);
	pos.x = 2 * (app->gui.ctx->input.mouse.pos.x - ww / 2.f);
	pos.y = 2 * (-app->gui.ctx->input.mouse.pos.y + wh / 2.f);
	return pos;
}

vec2 getMousePos(application_hndl* app) {
	vec2 pos;
	pos.x = app->gui.ctx->input.mouse.pos.x;
	pos.y = app->gui.ctx->input.mouse.pos.y;
	return pos;
}

