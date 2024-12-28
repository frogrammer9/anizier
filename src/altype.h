#ifndef ALTYPE_H
#define ALTYPE_H

#include <stdint.h>
#include <stdbool.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef const char* cstr;

typedef struct {
	f32 x, y;
} vec2;

typedef struct {
	f32 x, y, z;
} vec3;

typedef struct {
	f32 x, y, z, w;
} vec4;

typedef struct {
	i32 x, y;
} ivec2;

typedef struct {
	i32 x, y, z;
} ivec3;

typedef struct {
	i32 x, y, z, w;
} ivec4;

typedef struct {
	u32 x, y;
} uvec2;

typedef struct {
	u32 x, y, z;
} uvec3;

typedef struct {
	u32 x, y, z, w;
} uvec4;

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)
#define CLAMP(v, l, h) MIN(h, MAX(l, v))

#endif
