#include "editor.h"
#include "altype.h"
#include "app.h"
#include "gui.h"
#include "rendering.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "easingfunc.h"

typedef f32 (*easing_func_ptr)(f32);

i64 current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (i64)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void editor_run(application_hndl* app) {
	bool running = true;
	animation anim = {.frames = NULL, .size = 0, .maxsize = 0};
	new_frame(&anim);
	u32 curveId = -1;
	u32 frameId = 0;
	controlPoint* editedPoint = NULL;
	enum state  {
		none,
		adding_line_f,
		adding_line_s,
		moving_point,
		adding_point,
		interpolating,
		playing_animation,
	};
	enum state current_state = none;
	enum state inter_state = none;
	u32 inter_framesAmount = 1;
	char cbuffer[128] = "1";
	cstr easing_func[] = {"Linear", "Square", "Root", "S-shaped"};
	easing_func_ptr easing_funcs[] = {easing_linear, easing_sqare, easing_root, easing_s};
	int easing_func_inx = 0;
	i64 last_time;
	char file_name[256] = "";
	gui_hndl localgui;

	
	gui_init(&localgui, &app->window);

	while(running && !glfwWindowShouldClose(app->window.win)) {
		switch(current_state) {
			case none:
				if(nk_input_is_mouse_pressed(&localgui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(localgui.ctx)) {
					editedPoint = find_point_if_any(&anim.frames[frameId], getMousePosGL(app, &localgui), &curveId);
					if(editedPoint) current_state = moving_point;
				}
			break;
			case adding_line_f: {
				if(nk_input_is_mouse_pressed(&localgui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(localgui.ctx)) {
					vec2 mousePos = getMousePosGL(app, &localgui);
					editedPoint = add_point(&anim.frames[frameId].curves[curveId], mousePos.x, mousePos.y, 1);
					current_state = adding_line_s;
				}
			}
			break;
			case adding_line_s: {
				if(nk_input_is_mouse_pressed(&localgui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(localgui.ctx)) {
					vec2 mousePos = getMousePosGL(app, &localgui);
					editedPoint = add_point(&anim.frames[frameId].curves[curveId], mousePos.x, mousePos.y, 1);
					current_state = none;
				}
			}
			break;
			case moving_point: {
				if(nk_input_is_mouse_released(&localgui.ctx->input, NK_BUTTON_LEFT)) current_state = none;
				vec2 mousePos = getMousePosGL(app, &localgui);
				editedPoint->point.x = mousePos.x;
				editedPoint->point.y = mousePos.y;
			}
			break;
			case adding_point:
				if(nk_input_is_mouse_pressed(&localgui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(localgui.ctx)) {
					vec2 mousePos = getMousePosGL(app, &localgui);
					add_point(&anim.frames[frameId].curves[curveId], mousePos.x, mousePos.y, 1);
					current_state = none;
					bezierTemplate* curve = &anim.frames[frameId].curves[curveId];
					controlPoint s = curve->points[curve->size-1];
					curve->points[curve->size - 1] = curve->points[curve->size - 2];
					curve->points[curve->size - 2] = s;
					editedPoint = &curve->points[curve->size - 2];
				}
			break;
			case playing_animation:
				if(labs(last_time - current_time_ms()) > 41) {
					last_time = current_time_ms();
					++frameId;
					if(frameId == anim.size) {
						frameId = 0;
						current_state = none;
					}
				}
			break;
			default: printf("Wydarzyło sie coś nieoczekiwanego :c\n"); break;
		}
		if(current_state != playing_animation) {
			if(localgui.ctx->input.keyboard.text[localgui.ctx->input.keyboard.text_len - 1] == '+') ++frameId;
			if(localgui.ctx->input.keyboard.text[localgui.ctx->input.keyboard.text_len - 1] == '-') --frameId;
			if(frameId == -1) frameId = 0;
			frameId = MIN(frameId, anim.size - 1);
		}

		if(current_state != playing_animation) {
		GUI_NEW_FRAME(localgui);
		if(nk_begin(localgui.ctx, "Menu", nk_rect(50, 50, 230, 750), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(localgui.ctx, 30, 200, 1);
			nk_labelf(localgui.ctx, NK_TEXT_LEFT, "Frame: %u/%u (+/-)", frameId + 1, anim.size);
			if(inter_state == interpolating) {
				nk_layout_row_static(localgui.ctx, 20, 200, 1);
				nk_label(localgui.ctx, "Amount of frames:", NK_TEXT_LEFT);
				if(nk_edit_string_zero_terminated(localgui.ctx,NK_EDIT_FIELD, cbuffer, sizeof(cbuffer), nk_filter_decimal)) {
					sscanf(cbuffer, "%u", &inter_framesAmount);
				}
				if(editedPoint) {
					nk_layout_row_static(localgui.ctx, 30, 200, 1);
					nk_label(localgui.ctx, "Weight of the point:", NK_TEXT_LEFT);
					nk_slider_float(localgui.ctx, .01f, &editedPoint->weight, 10.f, .01f);
				}
				nk_layout_row_static(localgui.ctx, 20, 200, 1);
				easing_func_inx = nk_combo(localgui.ctx, easing_func, sizeof(easing_func) / sizeof(char*), easing_func_inx, 30, nk_vec2(200, 200));
				nk_layout_row_static(localgui.ctx, 60, 200, 1);
				if(nk_button_label(localgui.ctx, "Interpolate") && current_state == none) {
					inter_state = none;
					for(u32 i = 0; i < inter_framesAmount; ++i) {
						new_frame(&anim);
					}
					frame tframe = anim.frames[frameId];
					interpolate_frames(&anim.frames[frameId - 1], &tframe, easing_funcs[easing_func_inx], inter_framesAmount, &anim.frames[frameId]);
					frameId += inter_framesAmount;
					anim.frames[frameId] = tframe;
				}
			}
			else {
				nk_layout_row_static(localgui.ctx, 60, 200, 1);
				if(nk_button_label(localgui.ctx, "New line") && current_state == none) {
					editedPoint = NULL;
					new_line(&anim.frames[frameId]);
					curveId = anim.frames[frameId].size - 1;
					current_state = adding_line_f;
				}
				if(curveId != -1) {
					nk_layout_row_static(localgui.ctx, 60, 200, 1);
					if(nk_button_label(localgui.ctx, "Add point to selected line") && current_state == none) {
						current_state = adding_point;
					}
				}
				if(editedPoint) {
					nk_layout_row_static(localgui.ctx, 30, 200, 1);
					nk_label(localgui.ctx, "Weight of the point:", NK_TEXT_LEFT);
					nk_slider_float(localgui.ctx, .01f, &editedPoint->weight, 10.f, .01f);
				}
				nk_layout_row_static(localgui.ctx, 60, 200, 1);
				if(nk_button_label(localgui.ctx, "New frame") && current_state == none) {
					new_frame(&anim);
					curveId = -1;
					++frameId;
				}
				nk_layout_row_static(localgui.ctx, 60, 200, 1);
				if(nk_button_label(localgui.ctx, "Create an interpolation") && current_state == none) {
					if(anim.size - 1 != frameId) {
						printf("Can create an interpolation only of the last frame\n");
					}
					else {
						memset(cbuffer, 0, sizeof(cbuffer));
						cbuffer[0] = '1';
						new_frame(&anim);
						curveId = -1;
						++frameId;
						inter_state = interpolating;
						anim.frames[frameId].size = anim.frames[frameId - 1].size;
						anim.frames[frameId].maxsize = anim.frames[frameId - 1].maxsize;
						anim.frames[frameId].curves = malloc(anim.frames[frameId].maxsize * sizeof(bezierTemplate));
						for(u32 i = 0; i < anim.frames[frameId].size; ++i) {
							anim.frames[frameId].curves[i].size = anim.frames[frameId - 1].curves[i].size;
							anim.frames[frameId].curves[i].maxsize = anim.frames[frameId - 1].curves[i].maxsize;
							anim.frames[frameId].curves[i].points = malloc(anim.frames[frameId].curves[i].maxsize * sizeof(controlPoint));
							memcpy(	anim.frames[frameId].curves[i].points, 
									anim.frames[frameId - 1].curves[i].points, 
									anim.frames[frameId - 1].curves[i].size * sizeof(controlPoint));
						}
					}
				}
				nk_layout_row_static(localgui.ctx, 60, 200, 1);
				if(nk_button_label(localgui.ctx, "Run animation") && current_state == none) {
					current_state = playing_animation;
					last_time = current_time_ms();
					frameId = 0;
				}
				nk_layout_row_static(localgui.ctx, 20, 200, 1);
				nk_label(localgui.ctx, "Filename:", NK_TEXT_LEFT);
				nk_edit_string_zero_terminated(localgui.ctx,NK_EDIT_FIELD, file_name, sizeof(file_name), nk_filter_ascii);
				nk_layout_row_static(localgui.ctx, 60, 200, 1);
				if(nk_button_label(localgui.ctx, "Save animation") && current_state == none) {
					save_animation(&anim, file_name);
				}
				if(nk_button_label(localgui.ctx, "Load animation") && current_state == none) {
					editedPoint = NULL;
					load_animation(&anim, file_name);
				}
			}
			nk_layout_row_static(localgui.ctx, 60, 200, 1);
			if(nk_button_label(localgui.ctx, "Back")) {
				running = false;
			}
		}
		nk_end(localgui.ctx);
		GUI_RENDER(localgui);
		}

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

		if(current_state != playing_animation)
			render_cpoints(&anim.frames[frameId], 0x19e023ff, app->pointShader, curveId, 0xcc1dc3ff, 0xabcdefff, editedPoint);

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

void new_frame(animation* anim) {
	if(anim->maxsize == 0) {
		anim->frames = malloc(2 * sizeof(frame));
		anim->size = 0;
		anim->maxsize = 2;
	}
	if(++anim->size > anim->maxsize) {
		anim->frames = realloc(anim->frames, anim->maxsize * 2 * sizeof(frame));
		anim->maxsize *= 2;
	}
	anim->frames[anim->size - 1].curves = NULL;
	anim->frames[anim->size - 1].size = 0;
	anim->frames[anim->size - 1].maxsize = 0;
}

void new_line(frame* f) {
	if(f->maxsize == 0) {
		f->curves = malloc(2 * sizeof(bezierTemplate));
		f->size = 0;
		f->maxsize = 2;
	}
	if(++f->size > f->maxsize) {
		f->curves = realloc(f->curves, f->maxsize * 2 * sizeof(bezierTemplate));
		f->maxsize *= 2;
	}
	f->curves[f->size - 1].size = 0;
	f->curves[f->size - 1].maxsize = 0;
	f->curves[f->size - 1].points = NULL;
}

controlPoint* add_point(bezierTemplate* curve, f32 xPos, f32 yPos, f32 w) {
	if(curve->maxsize == 0) {
		curve->points = malloc(2 * sizeof(controlPoint));
		curve->maxsize = 2;
	}
	if(++curve->size > curve->maxsize) {
		curve->points = realloc(curve->points, curve->maxsize * 2 * sizeof(controlPoint));
		curve->maxsize *= 2;
	}
	curve->points[curve->size - 1].point.x = xPos;
	curve->points[curve->size - 1].point.y = yPos;
	curve->points[curve->size - 1].weight = w;
	return &curve->points[curve->size - 1];
}

static u32 len(controlPoint cp, vec2 mp) {
	i64 X = cp.point.x - mp.x;
	i64 Y = cp.point.y - mp.y;
	return sqrt(X * X + Y * Y);
}

controlPoint* find_point_if_any(frame* f, vec2 mPos, u32* curveId) {
	for(u32 i = 0; i < f->size; ++i) {
		for(u32 j = 0; j < f->curves[i].size; ++j) {
			if(len(f->curves[i].points[j], mPos) < 20.f) {
				*curveId = i;
				return &f->curves[i].points[j];
			}
		}
	}
	return NULL;
}

void render_cpoints(frame* f, u32 color, shaderID shader, u32 ccurve, u32 colorSelect, u32 lineSelect, controlPoint* cpoint) {
	u32 size = 0;
	for(u32 i = 0; i < f->size; ++i) size += f->curves[i].size;
	sample* samples = malloc(size * sizeof(sample));
	u32 inx = 0;
	for(u32 i = 0; i < f->size; ++i) {
		for(u32 j = 0; j < f->curves[i].size; ++j) {
			samples[inx].pos.x = f->curves[i].points[j].point.x;
			samples[inx].pos.y = f->curves[i].points[j].point.y;
			if(&f->curves[i].points[j] == cpoint) samples[inx++].col = colorSelect;
			else if(i == ccurve) samples[inx++].col = lineSelect;
			else samples[inx++].col = color;
		}
	}
	render_points(samples, size, shader);
}

void interpolate_frames(frame* f1, frame* f2, f32 (*easingFunc)(f32), u32 Nframes, frame* framesOUT) {
	if(f1->size != f2->size) {
		printf("ERROR Tried to interpolate between two frames with different amount of control points\n"); return;
	}
	for(u32 i = 0; i < Nframes; ++i) {
		framesOUT[i].size = f1->size;
		framesOUT[i].maxsize = f1->maxsize;
		framesOUT[i].curves = malloc(f1->maxsize * sizeof(bezierTemplate));
		for(u32 j = 0; j < f1->size; ++j) {
			framesOUT[i].curves[j].size = f1->curves[j].size;
			framesOUT[i].curves[j].maxsize = f1->curves[j].maxsize;
			framesOUT[i].curves[j].points = malloc(f1->curves[j].maxsize * sizeof(controlPoint));
		}
	}
	for(u32 fId = 0; fId < Nframes; ++fId) {
		f32 easingFactor = easingFunc((f32)(fId + 1) / (Nframes + 1));
		easingFactor = CLAMP(easingFactor, 0.f, 1.f);
		for(u32 i = 0; i < f1->size; ++i) {
			for(u32 j = 0; j < f1->curves[i].size; ++j) {
				controlPoint p0 = f1->curves[i].points[j];
				controlPoint p1 = f2->curves[i].points[j];
				framesOUT[fId].curves[i].points[j].point.x = p0.point.x + (p1.point.x - p0.point.x) * easingFactor;
				framesOUT[fId].curves[i].points[j].point.y = p0.point.y + (p1.point.y - p0.point.y) * easingFactor;
				framesOUT[fId].curves[i].points[j].weight = p0.weight + (p1.weight - p0.weight) * easingFactor;
			}
		}
	}
}

void save_animation(animation* anim, cstr path) {
	FILE *file = fopen(path, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }
    fprintf(file, "[\n");
    for (u32 d = 0; d < anim->size; d++) {
        fprintf(file, "  [\n");
        for (u32 r = 0; r < anim->frames[d].size; r++) {
            fprintf(file, "    [");
            for (u32 c = 0; c < anim->frames[d].curves[r].size; c++) {
				controlPoint cp = anim->frames[d].curves[r].points[c];
                fprintf(file, "{%f, %f, %f}", cp.point.x, cp.point.y, cp.weight);
                if (c < anim->frames[d].curves[r].size - 1) fprintf(file, ", ");
            }
            fprintf(file, "]");
            if (r < anim->frames[d].size - 1) fprintf(file, ",");
            fprintf(file, "\n");
        }
        fprintf(file, "  ]");
        if (d < anim->size - 1) fprintf(file, ",");
        fprintf(file, "\n");
    }
    fprintf(file, "]\n");

    fclose(file);
}

void load_animation(animation* anim, cstr path) {
	FILE* file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }
	for(u32 i = 0; i < anim->size; ++i) {
		for(u32 j = 0; j < anim->frames[i].size; ++j) {
			free(anim->frames[i].curves[j].points);
		}
		free(anim->frames[i].curves);
	}
	anim->frames = NULL;
	anim->size = 0;
	anim->maxsize = 0;
	free(anim->frames);
   char line[1024]; 
	while(fgets(line, sizeof(line), file)) {
		if(strstr(line, "    [")) { new_line(&anim->frames[anim->size - 1]); }
		else if(strstr(line, "  [")) { new_frame(anim); } 
		char *start = strchr(line, '{'); 
		while(start) {
			float x, y, z;
			if (sscanf(start, "{%f, %f, %f}", &x, &y, &z) == 3) {
				add_point(&anim->frames[anim->size - 1].curves[anim->frames[anim->size - 1].size - 1], x, y, z);
			}
			start = strchr(start + 1, '{'); 
        }
    }
    fclose(file);
	printf("Loaded file: %s\n", path);
}
