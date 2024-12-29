#include "runner.h"
#include "gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void runner_run(application_hndl* app) {
	bool running = true;
	animation anim = {.frames = NULL, .size = 0, .maxsize = 0};
	u32 frameId = 0;
	i64 last_time;
	char file_name[256] = "";
	char fps_select[64] = "24";
	bool animation_running = false;
	u32 fps_var = 0;
	gui_hndl localgui;
	gui_init(&localgui, &app->window);

	while(running && !glfwWindowShouldClose(app->window.win)) {
		if(labs(last_time - current_time_ms()) > (1.f / fps_var) * 1000 && animation_running) {
			last_time = current_time_ms();
			++frameId;
			if(frameId == anim.size) {
				frameId = 0;
				animation_running = false;
			}
		}

		GUI_NEW_FRAME(localgui);
		if(nk_begin(localgui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(localgui.ctx, 60, 200, 1);
			if(nk_button_label(localgui.ctx, "Run animation")) {
				if(anim.size == 0) {
					printf("No animation is loaded\n");
				}
				else {
					sscanf(fps_select, "%u", &fps_var);
					animation_running = true;
					last_time = current_time_ms();
					frameId = 0;
				}
			}
			nk_layout_row_static(localgui.ctx, 20, 200, 1);
			nk_label(localgui.ctx, "FPS:", NK_TEXT_LEFT);
			nk_edit_string_zero_terminated(localgui.ctx,NK_EDIT_FIELD, fps_select, sizeof(fps_select), nk_filter_decimal);
			nk_layout_row_static(localgui.ctx, 20, 200, 1);
			nk_label(localgui.ctx, "Filename:", NK_TEXT_LEFT);
			nk_edit_string_zero_terminated(localgui.ctx,NK_EDIT_FIELD, file_name, sizeof(file_name), nk_filter_ascii);
			nk_layout_row_static(localgui.ctx, 60, 200, 1);
			if(nk_button_label(localgui.ctx, "Load animation")) {
				load_animation(&anim, file_name);
			}
			nk_layout_row_static(localgui.ctx, 60, 200, 1);
			if(nk_button_label(localgui.ctx, "Back")) {
				running = false;
			}
			nk_end(localgui.ctx);
			GUI_RENDER(localgui);
		}
		if(animation_running) {
			rnBuffer lineBuffer;
			rnBuffer_init(&lineBuffer, false);
			for(u32 i = 0; i < anim.frames[frameId].size; ++i) {
				sample buffer[100];
				generate_bezier_samples(anim.frames[frameId].curves[i].points, anim.frames[frameId].curves[i].size, 50, buffer);
				rnBuffer_add_curve(&lineBuffer, buffer, 50);
			}
			u32 ftr = rnBuffer_new_frame(&lineBuffer);
			rnBuffer_render(&lineBuffer, app->lineShader, ftr, 0);
			rnBuffer_terminate(&lineBuffer);
		}
		window_FEP(&app->window);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	for(u32 i = 0; i < anim.size; ++i) {
		for(u32 j = 0; j < anim.frames[i].size; ++j) {
			free(anim.frames[i].curves[j].points);
		}
		free(anim.frames[i].curves);
	}
	free(anim.frames);
	gui_terminate(&localgui);
}

