#include "app.h"
#include "gui.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"
#include "editor.h"
#include "runner.h"
#include <GLFW/glfw3.h>

int shaderWinSizeUloc = 0;
int shaderPointSizeUloc = 0;

int app_init(application_hndl* app) {
	if(window_init(&app->window, 640, 480, "Anizier")) return 1;
	if(gui_init(&app->gui, &app->window)) return 1;
	app->lineShader = shader_init("assets/line.glsl"); //If this fails it calls exit(1)
	app->pointShader = shader_init("assets/point.glsl"); //If this fails it calls exit(1)
	shaderWinSizeUloc = glGetUniformLocation(app->lineShader, "uWinSize");
	shaderPointSizeUloc = glGetUniformLocation(app->pointShader, "pointSize");
	rnBuffer_init(&app->buffer, true);
	return 0;
}

int app_run(application_hndl* app) {
	int WinSw, WinSh;
	bool runEditor = false;
	bool runRunner = false;

	controlPoint cps[] = {
		{.point = {.x = -150.f,	.y = -200.f},	.weight = 1.f},
		{.point = {.x = 0.f,	.y = 100.f },	.weight = 1.f},
		{.point = {.x = 150.f,	.y = -200.f},	.weight = 1.f},
	};

	sample sam[] = {
		{.pos = {.x = -150.f,	.y = -200.f},	.col = 0x19e023ff},
		{.pos = {.x = 0.f,		.y = 100.f },	.col = 0x19e023ff},
		{.pos = {.x = 150.f,	.y = -200.f},	.col = 0x19e023ff},
	};

	sample curve[100];
	generate_bezier_samples(cps, 3, 50, curve);
	rnBuffer_add_curve(&app->buffer, curve, 50);
	generate_bezier_samples(cps, 2, 50, curve);
	rnBuffer_add_curve(&app->buffer, curve, 50);
	generate_bezier_samples(&cps[1], 2, 50, curve);
	rnBuffer_add_curve(&app->buffer, curve, 50);
	rnBuffer_new_frame(&app->buffer);
	rnBuffer_add_curve(&app->buffer, sam, 3);
	rnBuffer_new_frame(&app->buffer);

	while(!glfwWindowShouldClose(app->window.win)) {
		GUI_NEW_FRAME(app->gui);

		glfwGetWindowSize(app->window.win, &WinSw, &WinSh);
		shader_bind(app->lineShader);
		glUniform2f(shaderWinSizeUloc, WinSw, WinSh);
		shader_bind(app->pointShader);
		glUniform2f(shaderWinSizeUloc, WinSw, WinSh);
		glUniform1f(shaderPointSizeUloc, .05f);
		shader_bind(0);

		rnBuffer_render(&app->buffer, app->lineShader, 1, 0);
		render_points(sam, 3, app->pointShader);

		if(nk_begin(app->gui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Run editor")) {
				runEditor = true;
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Run animation")) {
				runRunner = true;
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
		if(runRunner) {
			runRunner= false;
			runner_run(app);
		}

		window_FEP(&app->window);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	return 0;
}

void app_terminate(application_hndl* app) {
	rnBuffer_terminate(&app->buffer);
	shader_terminate(&app->lineShader);
	shader_terminate(&app->pointShader);
	gui_terminate(&app->gui);
	window_terminate(&app->window);
}

vec2 getMousePosGL(application_hndl* app, gui_hndl* gui) {
	vec2 pos;
	int ww = 0, wh = 0;
	glfwGetWindowSize(app->window.win, &ww, &wh);
	pos.x = gui->ctx->input.mouse.pos.x - ww / 2.f;
	pos.y = -gui->ctx->input.mouse.pos.y + wh / 2.f;
	return pos;
}

vec2 getMousePos(application_hndl* app) {
	vec2 pos;
	pos.x = app->gui.ctx->input.mouse.pos.x;
	pos.y = app->gui.ctx->input.mouse.pos.y;
	return pos;
}

