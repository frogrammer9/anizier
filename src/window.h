#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <GLFW/glfw3.h>


typedef struct {
	GLFWwindow* win;
} window_hndl;

int window_init(window_hndl* win, int width, int height, const char* name);
void window_terminate(window_hndl* win);
void window_FEP(window_hndl* win);

#endif

