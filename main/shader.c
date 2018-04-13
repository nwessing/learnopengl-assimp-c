#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cglm/call.h>

#include "types.h"
#include "file.h"
#include "shader.h"

static u32 compileShader(i32 shaderType, const i8 *shaderSource);
static i32 checkShaderCompileErrors(u32 shader);
static i32 checkProgramLinkErrors(u32 program);

ShaderProgram createShader(const i8 *vertexShaderFileName, const i8 *fragmentShaderFileName)
{
	i8 *vertexSource = readFile(vertexShaderFileName);
	if (vertexSource == NULL) {
		return 0;
	}
	
	i8 *fragmentSource = readFile(fragmentShaderFileName);
	if (fragmentSource == NULL) {
		return 0;
	}

	u32 vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
	free(vertexSource);
	if (!vertexShader) {
		return 0;
	}

	u32 fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
	free(fragmentSource);
	if (!fragmentShader) {
		return 0;
	}

	u32 shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	if (!checkProgramLinkErrors(shaderProgram)) {
		return 0;
	}

	return shaderProgram;
}

void shaderProgramSetBool(ShaderProgram program, const i8 *name, bool value)
{
	glUniform1i(glGetUniformLocation(program, name), (i32)value);
}

void shaderProgramSetInt(ShaderProgram program, const i8 *name, i32 value)
{
	glUniform1i(glGetUniformLocation(program, name), value);
}

void shaderProgramSetFloat(ShaderProgram program, const i8 *name, float value)
{
	glUniform1f(glGetUniformLocation(program, name), value);
}

void shaderProgramSetVec3(ShaderProgram program, const i8 *name, vec3 vec)
{
	glUniform3fv(glGetUniformLocation(program, name), 1, vec);
}

void shaderProgramSetMat4(ShaderProgram program, const i8 *name, const mat4 mat)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, mat);
}

static u32 compileShader(i32 shaderType, const i8 *shaderSource)
{
	u32 shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	if (!checkShaderCompileErrors(shader)) {
		return 0;
	}

	return shader;
}

static i32 checkShaderCompileErrors(u32 shader)
{
	i32 success;
	i8 infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::COMPILATION_FAILURE\n%s\n", infoLog);
	}

	return success;
}

static i32 checkProgramLinkErrors(u32 program)
{
	i32 success;
	i8 infoLog[512];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("ERROR:PROGRAM::LINK_FAILURE\n%s\n", infoLog);
	}

	return success;
}
