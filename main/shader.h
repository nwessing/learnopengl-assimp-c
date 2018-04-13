#pragma once
#include "types.h"
#include <stdbool.h>
#include <cglm/call.h>

typedef u32 ShaderProgram;

ShaderProgram createShader(const i8 *vertexShaderFileName, const i8 *fragmentShaderFileName);
void shaderProgramSetBool(ShaderProgram program, const i8 *name, bool value);
void shaderProgramSetInt(ShaderProgram program, const i8 *name, i32 value);
void shaderProgramSetFloat(ShaderProgram program, const i8 *name, float value);
void shaderProgramSetVec3(ShaderProgram program, const i8 *name, vec3 vec);
void shaderProgramSetMat4(ShaderProgram program, const i8 *name, const mat4 mat);
