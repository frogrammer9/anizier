#include "editor.h"
#include "gui.h"
#include "window.h"
#include <stdio.h>

void editor_run(application_hndl* app) {
	editor_params ediparm;
	ediparm.close = false;
	while(!ediparm.close) {
		editor_render_gui(app, &ediparm);

		window_FEP(&app->window);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void editor_render_gui(application_hndl* app, editor_params* ediprm) {
		GUI_NEW_FRAME(app->gui);
		if(nk_begin(app->gui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New line")) {
				editor_run(app);
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New frame")) {
				printf("Button pressed\n");
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Run animation")) {
				printf("Button pressed\n");
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Back")) {
				ediprm->close = true;
			}

		}
		nk_end(app->gui.ctx);
		GUI_RENDER(app->gui);
}
