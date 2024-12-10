#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <GLFW/glfw3.h>


typedef struct {
	bool open;
	GLFWwindow* win;
	float mouseX;
	float mouseY;
	uint64_t keyboardMask;
	uint64_t keyboardClearMask;
} window_hndl;

int window_init(window_hndl* win, int width, int height, const char* name);
void window_terminate(window_hndl* win);
void window_FEP(window_hndl* win);

bool window_getKeyState(window_hndl* win, int key);
void window_getMousePos(window_hndl* win, float* mouseXout, float* mouseYout);

#endif

