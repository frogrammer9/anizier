#ifndef APP_H
#define APP_H

#include "gui.h"
#include "window.h"

typedef struct {
	gui_hndl gui;
	window_hndl window;
} application_hndl;

int app_init(application_hndl* app);
int app_run(application_hndl* app);
void app_terminate(application_hndl* app);

#endif
