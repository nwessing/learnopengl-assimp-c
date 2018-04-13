#pragma once
#include <cglm/cglm.h>

typedef struct camera {
	vec3 position;
	vec3 up;
	float pitch;
	float yaw;
	float fov;
	float movementSpeed;
	float mouseSensitivity;
	bool lockMovementPlane;
} camera;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

void camera_front(camera *cam, vec3 dest);
void camera_right(camera *cam, vec3 front, vec3 dest);
void camera_view_matrix(camera *cam, mat4 dest);
void camera_processKeyboard(camera *cam, enum Camera_Movement movement, float intensity, float deltaTime);
void camera_processMouseMovement(camera *cam, float xoffset, float yoffset);
void camera_processMouseScroll(camera *cam, double yoffset);
