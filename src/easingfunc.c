#include "easingfunc.h"
#include <math.h>

f32 easing_linear(f32 v) {
	return v;
}
f32 easing_sqare(f32 v) {
	return v * v;
}
f32 easing_root(f32 v) {
	return sqrt(v);
}
f32 easing_s(f32 v) {
	return 3 * v * v - 2 * v * v * v;
}
