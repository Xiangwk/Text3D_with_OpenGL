#pragma once

#include <glad\glad.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <cmath>

enum CameraMovement{ FORWARD, BACKWARD, LEFT, RIGHT };

//the euler angle and camera option's initialize value
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

//this is an abstract base class
class BaseCamera
{
public:
	glm::vec3 position;   //camera's position
	glm::vec3 front;      //camera's front vector
	glm::vec3 up;         //camera's up vector
	glm::vec3 right;      //camera's right vector
	glm::vec3 worldUp;    //world's up vector, it's [0, 1, 0] commonly

	GLfloat pitch;
	GLfloat yaw;

	GLfloat moveSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;

	BaseCamera(const glm::vec3 &pos = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 &u = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat p = PITCH, GLfloat y = YAW);
	//get view matrix, it is the essential of an opengl camera
	glm::mat4 getViewMatrix();
	//camera movement
	virtual void processKeyboard(CameraMovement direction, GLfloat deltaTime) = 0;
	//camera rotation
	virtual void processMouseMovement(GLfloat x, GLfloat y);
	//camera' zoom
	virtual void processMouseScroll(GLfloat z);

private:
	//according to the eular angles to construct the camera's coordinate
	void updateCamera();
};

BaseCamera::BaseCamera(const glm::vec3 &pos, const glm::vec3 &u, GLfloat p, GLfloat y) :
	position(pos), worldUp(u), pitch(p), yaw(y), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	updateCamera();
}

void BaseCamera::updateCamera()
{
	front.x = std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));
	front.y = std::sin(glm::radians(pitch));
	front.z = std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

glm::mat4 BaseCamera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void BaseCamera::processKeyboard(CameraMovement direction, GLfloat deltaTime)
{
	GLfloat v = moveSpeed * deltaTime;
	switch (direction)
	{
	case FORWARD:
		position += front * v;
		break;
	case BACKWARD:
		position -= front * v;
		break;
	case LEFT:
		position -= right * v;
		break;
	case RIGHT:
		position += right * v;
		break;
	default:
		break;
	}
}

void BaseCamera::processMouseMovement(GLfloat deltaX, GLfloat deltaY)
{
	//the x-oriented position's difference in one frame
	deltaX *= mouseSensitivity;
	//the y-oriented position's difference in one frame
	deltaY *= mouseSensitivity;

	yaw += deltaX;
	pitch += deltaY;

	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;
	//because we changed eular angle, we update the camera's coordinate
	updateCamera();
}

void BaseCamera::processMouseScroll(GLfloat z)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= z;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

//this is a free camera that can move everywhere in the space
class FreeCamera : public BaseCamera
{
public:
	FreeCamera(const glm::vec3 &pos = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 &u = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat p = PITCH, GLfloat y = YAW);
	void processKeyboard(CameraMovement direction, GLfloat deltaTime) override;
};

FreeCamera::FreeCamera(const glm::vec3 &pos, const glm::vec3 &u, GLfloat p, GLfloat y) : BaseCamera(pos, u, p, y){}

void FreeCamera::processKeyboard(CameraMovement direction, GLfloat deltaTime)
{
	BaseCamera::processKeyboard(direction, deltaTime);
}

//this is a fps camera that can move only on the ground(the plane which y = 0)
class FPSCamera : public BaseCamera
{
public:
	FPSCamera::FPSCamera(const glm::vec3 &pos = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 &u = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat p = PITCH, GLfloat y = YAW);
	void processKeyboard(CameraMovement direction, GLfloat deltaTime) override;
};

FPSCamera::FPSCamera(const glm::vec3 &pos, const glm::vec3 &u, GLfloat p, GLfloat y) : BaseCamera(pos, u, p, y){}

void FPSCamera::processKeyboard(CameraMovement direction, GLfloat deltaTime)
{
	BaseCamera::processKeyboard(direction, deltaTime);
	//we cannot leave the ground
	position.y = 0;
}