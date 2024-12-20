#ifndef EDITOR_H
#define EDITOR_H

#include "app.h"

typedef struct {
	bool close;
} editor_params;

void editor_run(application_hndl* app);
void editor_render_gui(application_hndl* app, editor_params* ediprm);

#endif
