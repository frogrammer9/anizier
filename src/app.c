#include "app.h"
#include "gui.h"
#include "window.h"
#include <stdio.h>

int app_init(application_hndl* app) {
	if(window_init(&app->window, 640, 480, "Bezier editor")) return 1;
	if(gui_init(&app->gui, &app->window)) return 1;
	return 0;
}

int app_run(application_hndl* app) {
	while(app->window.open) {


		gui_render(&app->gui);
		window_FEP(&app->window);
	}
	return 0;
}

void app_terminate(application_hndl* app) {
	gui_terminate(&app->gui);
	window_terminate(&app->window);
}
