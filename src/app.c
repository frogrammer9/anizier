#include "app.h"
#include "gui.h"
#include "shader.h"
#include "window.h"
#include "rendering.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int shaderWinSizeUniLoc = 0;

static f32 randf(f32 min, f32 max) {
	return min + (f32)rand() / (f32)RAND_MAX * (max - min);
}

int app_init(application_hndl* app) {
	if(window_init(&app->window, 640, 480, "Anizier")) return 1;
	if(gui_init(&app->gui, &app->window)) return 1;
	app->shader = shader_init("basic.glsl"); //If this fails it calls exit(1)
	shaderWinSizeUniLoc = glGetUniformLocation(app->shader, "uWinSize");
	return 0;
}

int app_run(application_hndl* app) {

	rnBuffer buff;
	rnBuffer_init(&buff, true);

	#define amount 20
	
	sample l1s[amount];
	controlPoint l1[amount];
	sample s;
	controlPoint cp;
	s.col = 0xff0000ff;
	for(u32 i = 0; i < amount; ++i) {
		f32 fx = randf(-400.f, 400.f);
		f32 fy = randf(-800.f, 800.f);
		f32 fw = randf(0.f, 2.f);
		s.pos.x = fx;
		s.pos.y = fy;
		cp.point.x = fx;
		cp.point.y = fy;
		cp.weight = fw;
		l1s[i] = s;
		l1[i] = cp;
	}

	sample curve[500];

	generate_bezier_samples(l1, amount, 100, curve);
	rnBuffer_add_curve(&buff, curve, 100);
	u32 frame = rnBuffer_new_frame(&buff);

	for(u32 i = 0; i < amount; ++i) {
		f32 fx = randf(-400.f, 400.f);
		f32 fy = randf(-800.f, 800.f);
		f32 fw = randf(0.f, 2.f);
		s.pos.x = fx;
		s.pos.y = fy;
		cp.point.x = fx;
		cp.point.y = fy;
		cp.weight = fw;
		l1s[i] = s;
		l1[i] = cp;
	}

	generate_bezier_samples(l1, amount, 100, curve);
	rnBuffer_add_curve(&buff, curve, 100);
	rnBuffer_new_frame(&buff);

	for(u32 i = 0; i < amount; ++i) {
		f32 fx = randf(-400.f, 400.f);
		f32 fy = randf(-800.f, 800.f);
		f32 fw = randf(0.f, 2.f);
		s.pos.x = fx;
		s.pos.y = fy;
		cp.point.x = fx;
		cp.point.y = fy;
		cp.weight = fw;
		l1s[i] = s;
		l1[i] = cp;
	}

	generate_bezier_samples(l1, amount, 100, curve);
	rnBuffer_add_curve(&buff, curve, 100);
	rnBuffer_new_frame(&buff);

	int WinSw, WinSh;

	while(!glfwWindowShouldClose(app->window.win)) {

		shader_bind(app->shader);
		glfwGetWindowSize(app->window.win, &WinSw, &WinSh);
		glUniform2f(shaderWinSizeUniLoc, WinSw, WinSh);
		shader_bind(0);

		vec2 pos = getMousePosGL(app);

		if(app->gui.ctx->input.keyboard.text[app->gui.ctx->input.keyboard.text_len - 1] == '+') frame++;
		if(app->gui.ctx->input.keyboard.text[app->gui.ctx->input.keyboard.text_len - 1] == '-') frame--;

		rnBuffer_render(&buff, app->shader, frame, 30);

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

