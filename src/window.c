#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "window.h"
#include "gui.h"

int window_init(window_hndl* win, int width, int height, const char* name) {
	if(!glfwInit()) {
		printf("error: glew failed to initialize\n");
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	win->win = glfwCreateWindow(width, height, name, NULL, NULL);
	if(!win->win) {
		glfwTerminate();
		printf("error: failed to initialize window\n");
		return 1;
	}
	glfwMakeContextCurrent(win->win);
	
	if(glewInit() != GLEW_OK) {
		printf("glew failed to initialize\n");
		return 1;
	}

	return 0;
}

void window_terminate([[maybe_unused]] window_hndl* win) {
	glfwTerminate();
}

void window_FEP(window_hndl* win) {
	glfwSwapBuffers(win->win);
	glfwPollEvents();
}
