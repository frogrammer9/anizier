#ifndef SHADER_H
#define SHADER_H

#include "altype.h"

typedef u32 shaderID;

shaderID shader_init(cstr src);
void shader_terminate(shaderID* id);
void shader_bind(shaderID id);

#endif
