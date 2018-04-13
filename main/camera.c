#include <cglm/vec3.h>
#include "camera.h"
#include "math.h"

void camera_lookat(vec3 position, vec3 target, vec3 worldUp, mat4 dest);

void camera_front(camera *cam, vec3 dest) {
	float pitchRad = glm_rad(cam->pitch);
	float yawRad = glm_rad(cam->yaw);
	dest[0] = (float) (cos(pitchRad) * cos(yawRad));
	dest[1] = (float) sin(pitchRad);
	dest[2] = (float)(cos(pitchRad) * sin(yawRad));
	glm_normalize(dest);
}

void camera_right(camera *cam, vec3 front, vec3 dest) {
	glm_vec_cross(front, cam->up, dest);
	glm_normalize(dest);
}

void camera_view_matrix(camera *cam, mat4 dest) {
	vec3 front;
	camera_front(cam, front);

	vec3 center;
	glm_vec_add(cam->position, front, center);

	//camera_lookat(cam->position, center, cam->up, dest);
	glm_lookat(cam->position, center, cam->up, dest);
}

void camera_processKeyboard(camera *cam, enum Camera_Movement movement, float intensity, float deltaTime)
{
	if (intensity < 0.0f) {
		intensity = 0.0f;
	}

	if (intensity > 1.0f) {
		intensity = 1.0f;
	}

	float velocity = cam->movementSpeed * deltaTime * intensity;

	vec3 front;
	camera_front(cam, front);
	if (cam->lockMovementPlane) {
		front[1] = 0.0f;
		glm_normalize(front);
	}

	vec3 right;
	camera_right(cam, front, right);

	if (movement == FORWARD) {
		vec3 cameraFrontScaled;
		
		glm_vec_scale(front, velocity, cameraFrontScaled);
		glm_vec_add(cam->position, cameraFrontScaled, cam->position);
	}

	if (movement == BACKWARD) {
		vec3 cameraFrontScaled;
		glm_vec_scale(front, velocity, cameraFrontScaled);
		glm_vec_sub(cam->position, cameraFrontScaled, cam->position);
	}

	if (movement == LEFT) {
		vec3 cameraRightScaled;
		glm_vec_scale(right, velocity, cameraRightScaled);
		glm_vec_sub(cam->position, cameraRightScaled, cam->position);
	}

	if (movement == RIGHT) {
		vec3 cameraRightScaled;
		glm_vec_scale(right, velocity, cameraRightScaled);
		glm_vec_add(cam->position, cameraRightScaled, cam->position);
	}
}

void camera_processMouseMovement(camera *cam, float xoffset, float yoffset)
{
	xoffset *= cam->mouseSensitivity;
	yoffset *= cam->mouseSensitivity;

	cam->yaw += xoffset;
	cam->pitch += yoffset;

	if (cam->pitch > 89.0f) {
		cam->pitch = 89.0f;
	}

	if (cam->pitch < -89.0f) {
		cam->pitch = -89.0f;
	}
}

void camera_processMouseScroll(camera *cam, double yoffset)
{
	if (cam->fov >= 1.0f && cam->fov <= 45.0f) {
		cam->fov -= (float) yoffset;
	}

	if (cam->fov <= 1.0f) {
		cam->fov = 1.0f;
	}

	if (cam->fov >= 45.0f) {
		cam->fov = 45.0f;
	}
}

void camera_lookat(vec3 position, vec3 target, vec3 worldUp, mat4 dest) 
{
	vec3 direction;
	glm_vec_sub(position, target, direction);
	glm_normalize(direction);

	vec3 worldUpNorm;
	glm_normalize_to(worldUp, worldUpNorm);
	vec3 right;
	glm_cross(worldUpNorm, direction, right);
	glm_normalize(right);

	vec3 up;
	glm_cross(direction, right, up);

	mat4 rotation = GLM_MAT4_IDENTITY_INIT;
	rotation[0][0] = right[0];
	rotation[1][0] = right[1];
	rotation[2][0] = right[2];
	rotation[0][1] = up[0];
	rotation[1][1] = up[1];
	rotation[2][1] = up[2];
	rotation[0][2] = direction[0];
	rotation[1][2] = direction[1];
	rotation[2][2] = direction[2];

	mat4 translation = GLM_MAT4_IDENTITY_INIT;
	translation[3][0] = -1.0f * position[0];
	translation[3][1] = -1.0f * position[1];
	translation[3][2] = -1.0f * position[2];

	glm_mat4_mul(rotation, translation, dest);
}
