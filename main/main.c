#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <cglm/cglm.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "types.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "scene.h"
#include "model.h"

void framebuffer_size_callback(GLFWwindow *, int, int);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *windows, camera *cam);
void joystick_input(camera *cam);
void render(Model *model, ShaderProgram objectShader, u32 lampVAO, ShaderProgram lampShader, camera *cam);

static i32 screenWidth, screenHeight;
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static double lastX = 0.0;
static double lastY = 0.0;
static camera cam = {
	.position = { 0.0f, 0.0f, 3.0f },
	.up = { 0.0f, 1.0f, 0.0f },
	.pitch = 0.0f,
	.yaw = -90.0f,
	.fov = 45.0f,
	.movementSpeed = 2.5f,
	.mouseSensitivity = 0.05f,
	.lockMovementPlane = false
};


int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return 1;
	}

	screenWidth = 800;
	screenHeight = 600;

	lastX = screenWidth / 2;
	lastY = screenHeight / 2;

	glViewport(0, 0, screenWidth, screenHeight);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	ShaderProgram objectShader = createShader("transform.vert", "object.frag");
	if (!objectShader) {
		return 1;
	}

	ShaderProgram lampShader = createShader("transform.vert", "light.frag");
	if (!lampShader) {
		return 1;
	}

	Model model = loadModel("nanosuit/nanosuit.obj");

	glEnable(GL_DEPTH_TEST);

	u32 VBO;
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	u32 lampVAO;
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	lastFrame = (float) glfwGetTime();
	while (!glfwWindowShouldClose(window)) 
	{
		GLenum error = glGetError();
		if (error) {
			printf("error: %i\n", error);
		}

		float currentFrame = (float) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window, &cam);
		joystick_input(&cam);
		render(&model, objectShader, lampVAO, lampShader, &cam);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	UNREFERENCED_PARAMETER(window);
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, camera *myCam)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_processKeyboard(myCam, FORWARD, 1.0f, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_processKeyboard(myCam, BACKWARD, 1.0f, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_processKeyboard(myCam, LEFT, 1.0f, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_processKeyboard(myCam, RIGHT, 1.0f, deltaTime);
	}
}

static bool firstMouse = true;
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	UNREFERENCED_PARAMETER(window);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // y goes top to bottom, so reverse
	lastX = xpos;
	lastY = ypos;
	
	camera_processMouseMovement(&cam, (float) xoffset, (float) yoffset);
}

void joystick_input(camera *myCam)
{
	if (!glfwJoystickPresent(GLFW_JOYSTICK_1)) {
		return;
	}

	i32 count;
	// *axes array is managed by glfw, do not free
	const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

	if (count < 2) {
		return;
	}

	if (axes[0] > 0.0f) {
		camera_processKeyboard(myCam, RIGHT, axes[0], deltaTime);
	}
	else if (axes[0] < 0.0f) {
		camera_processKeyboard(myCam, LEFT, -1.0f * axes[0], deltaTime);
	}

	if (axes[1] > 0.0f) {
		camera_processKeyboard(myCam, FORWARD, axes[1], deltaTime);
	}
	else if (axes[1] < 0.0f) {
		camera_processKeyboard(myCam, BACKWARD, -1.0f * axes[1], deltaTime);
	}

	if (count < 4) {
		return;
	}

	camera_processMouseMovement(myCam, axes[2] * 2.0f, axes[3] * 2.0f);

	if (count < 6) {
		return;
	}

	camera_processMouseScroll(myCam, axes[4]);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(window);
	UNREFERENCED_PARAMETER(xoffset);

	camera_processMouseScroll(&cam, yoffset);
}

void render(Model *model, ShaderProgram objectShader, u32 lampVAO, ShaderProgram lampShader, camera *myCam)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view;
	camera_view_matrix(myCam, view);

	mat4 projection = GLM_MAT4_IDENTITY_INIT;
	glm_perspective(glm_rad(myCam->fov), screenWidth / (float)screenHeight, 0.1f, 100.0f, projection);

	glUseProgram(objectShader);
	shaderProgramSetMat4(objectShader, "view", view);
	shaderProgramSetMat4(objectShader, "projection", projection);
	shaderProgramSetMat4(objectShader, "model", GLM_MAT4_IDENTITY);

	shaderProgramSetFloat(objectShader, "material.shininess", 32);

	shaderProgramSetVec3(objectShader, "dirLight.direction", (vec3) { -0.2f, -1.0f, -0.3f });
	shaderProgramSetVec3(objectShader, "dirLight.ambient", (vec3) { 0.05f, 0.05f, 0.05f });
	shaderProgramSetVec3(objectShader, "dirLight.diffuse", (vec3) { 0.4f, 0.4f, 0.4f });
	shaderProgramSetVec3(objectShader, "dirLight.specular", (vec3) { 0.5f, 0.5f, 0.5f });

	vec3 cameraFront;
	camera_front(myCam, cameraFront);
	shaderProgramSetVec3(objectShader, "viewPos", myCam->position);
	shaderProgramSetVec3(objectShader, "spotLight.position", myCam->position);
	shaderProgramSetVec3(objectShader, "spotLight.direction", cameraFront);
	shaderProgramSetFloat(objectShader, "spotLight.cutOff", (float) cos(glm_rad(12.5f)));
	shaderProgramSetFloat(objectShader, "spotLight.outerCutOff", (float) cos(glm_rad(15.0f)));

	shaderProgramSetVec3(objectShader, "spotLight.ambient", (vec3) { 0.0f, 0.0f, 0.0f });
	shaderProgramSetVec3(objectShader, "spotLight.diffuse", (vec3) { 1.0f, 1.0f, 1.0f });
	shaderProgramSetVec3(objectShader, "spotLight.specular", (vec3) { 1.0f, 1.0f, 1.0f });
	shaderProgramSetFloat(objectShader, "spotLight.constant", 1.0f);
	shaderProgramSetFloat(objectShader, "spotLight.linear", 0.09f);
	shaderProgramSetFloat(objectShader, "spotLight.quadratic", 0.032f);

	shaderProgramSetInt(objectShader, "usedPointLights", 4);

	vec3 pointLightPositions[] = {
		{ 0.7f,  20.0f,  2.0f },
		{ 2.3f, 15.0f, -4.0f },
		{ -4.0f,  10.0f, -12.0f },
		{ 0.0f,  0.0f, -3.0f }
	};

	vec3 pointLightColors[] = {
		{ 0.0f, 1.0f, 1.0f, },
		{ 1.0f, 1.0f, 1.0f, },
		{ 1.0f, 1.0f, 1.0f, },
		{ 1.0f, 0.0f, 0.0f, }
	};

	char uniformNameBuf[100];
	for (int i = 0; i < 4; i++) {
		sprintf(uniformNameBuf, "pointLights[%i].position", i);
		shaderProgramSetVec3(objectShader, uniformNameBuf, pointLightPositions[i]);
		sprintf(uniformNameBuf, "pointLights[%i].constant", i);
		shaderProgramSetFloat(objectShader, uniformNameBuf, 1.0f);
		sprintf(uniformNameBuf, "pointLights[%i].linear", i);
		shaderProgramSetFloat(objectShader, uniformNameBuf, 0.09f);
		sprintf(uniformNameBuf, "pointLights[%i].quadratic", i);
		shaderProgramSetFloat(objectShader, uniformNameBuf, 0.032f);

		sprintf(uniformNameBuf, "pointLights[%i].ambient", i);
		vec3 ambient;
		glm_vec_scale(pointLightColors[i], 0.2f, ambient);
		shaderProgramSetVec3(objectShader, uniformNameBuf, ambient);

		sprintf(uniformNameBuf, "pointLights[%i].diffuse", i);
		vec3 diffuse;
		glm_vec_scale(pointLightColors[i], 0.5f, diffuse);
		shaderProgramSetVec3(objectShader, uniformNameBuf, diffuse);
		
		sprintf(uniformNameBuf, "pointLights[%i].specular", i);
		shaderProgramSetVec3(objectShader, uniformNameBuf, pointLightColors[i]);
	}

	assert(glGetError() == GL_NO_ERROR);
	model_draw(model, objectShader);
	assert(glGetError() == GL_NO_ERROR);

	glUseProgram(lampShader);
	shaderProgramSetMat4(lampShader, "view", view);
	shaderProgramSetMat4(lampShader, "projection", projection);
	for (int i = 0; i < 4; i++) {
		mat4 lampModel = GLM_MAT4_IDENTITY_INIT;
		glm_translate(lampModel, pointLightPositions[i]);
		glm_scale(lampModel, (vec3) { 0.2f, 0.2f, 0.2f });
		shaderProgramSetMat4(lampShader, "model", lampModel);
		shaderProgramSetVec3(lampShader, "lightColor", pointLightColors[i]);
		glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}
