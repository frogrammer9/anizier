#include <GL/glew.h>
#include <stdio.h>
#include "window.h"

static void close_CB(GLFWwindow* win) {
	window_hndl* winHndl = glfwGetWindowUserPointer(win);
	winHndl->open = false;
}

static void mouse_CB(GLFWwindow* win, double xpos, double ypos)  {
	window_hndl* winHndl = glfwGetWindowUserPointer(win);
	winHndl->mouseX = xpos;
	winHndl->mouseY = ypos;
}

static void keyboard_CB(GLFWwindow* win, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
	window_hndl* winHndl = glfwGetWindowUserPointer(win);
	if(action == GLFW_REPEAT) return;
	if(key < 48 || key > 90 || (key > 57 && key < 65)) return; //Ignore anything other then A-Z and 0-9
	int keyMask = 0;
	if(key < 65) keyMask = key - 48;
	else keyMask = key - 65 + 10;
	if(action == GLFW_PRESS) winHndl->keyboardMask |= (1 << keyMask);
}

int window_init(window_hndl* win, int width, int height, const char* name) {
	win->open = false;
	win->mouseX = 0.f;
	win->mouseY = 0.f;
	win->keyboardMask = 0;

	if(!glfwInit()) {
		perror("glew failed to initialize\n");
		return 1;
	}
	win->win = glfwCreateWindow(width, height, name, NULL, NULL);
	if(!win->win) {
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(win->win);
	
	glfwSetWindowUserPointer(win->win, win);
	glfwSetWindowCloseCallback(win->win, close_CB);
	glfwSetCursorPosCallback(win->win, mouse_CB);
	glfwSetKeyCallback(win->win, keyboard_CB);

	win->open = true;

	if(glewInit() != GLEW_OK) {
		perror("glew failed to initialize\n");
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
	win->keyboardMask &= ~win->keyboardClearMask;
	win->keyboardClearMask = win->keyboardMask;
}

bool window_getKeyState(window_hndl* win, int key) {
	if(key < 48 || key > 90 || (key > 57 && key < 65)) return false; //Ignore anything other then A-Z and 0-9
	int keyMask = 0;
	if(key < 65) keyMask = key - 48;
	else keyMask = key - 65 + 10;

	bool state = win->keyboardMask & (1 << keyMask);
	win->keyboardMask &= ~(1 << keyMask);
	return state;
}

void window_getMousePos(window_hndl* win, float* mouseXout, float* mouseYout) {
	*mouseXout = win->mouseX;
	*mouseYout = win->mouseY;
}
