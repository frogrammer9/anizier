#include "app.h"

int main() {
	application_hndl app;
	if(app_init(&app)) return 1;
	int retC = app_run(&app);
	app_terminate(&app);
	return retC;
}
