#ifndef APP_H
#define APP_H

#include "gui.h"
#include "rendering.h"
#include "window.h"
#include "shader.h"
#include "altype.h"

typedef struct {
	gui_hndl gui;
	window_hndl window;
	shaderID lineShader;
	shaderID pointShader;
	rnBuffer buffer;
} application_hndl;

int app_init(application_hndl* app);
int app_run(application_hndl* app);
void app_terminate(application_hndl* app);

vec2 getMousePosGL(application_hndl* app);
vec2 getMousePos(application_hndl* app);

#endif
