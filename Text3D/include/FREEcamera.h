//这是一个Free相机的类
//Free相机可以在空间中自由移动
//镜头俯仰角为-90度到90度，偏航角为0到360度，没有翻转动作
//相机中的数学操作使用GLM库完成

#ifndef FREECAMERA_H_
#define FREECAMERA_H_
//需要使用三角函数
#include <cmath>

#include <gl/glew.h>

//opengl数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//表示相机移动的动作，我们使用一个自定义的枚举来屏蔽不同窗口系统之间方向表示的差异
enum CameraMovement { FORWARD /*前进*/, BACKWARD /*后退*/, LEFT /*左平移*/, RIGHT /*右平移*/ };

//相机中的默认参数
const GLfloat YAW = -90.0f;         //偏航角初始值
const GLfloat PITCH = 0.0f;         //俯仰角初始值
const GLfloat SPEED = 2.0f;         //这个参数是为了平衡帧率
const GLfloat SENSITIVITY = 0.10f;  //鼠标灵敏度
const GLfloat ZOOM = 45.0f;         //视锥体视场角

class Free_Camera
{
public:
	//相机属性
	glm::vec3 position;         //相机位置
	glm::vec3 front;            //相机朝向
	glm::vec3 up;               //相机的上方向
	glm::vec3 right;            //相机的右方向
	glm::vec3 world_up;         //世界上方向，一般就是（0.0, 1.0, 0.0）
	//欧拉角
	GLfloat yaw;        
	GLfloat pitch;
	//相机的设置参数
	GLfloat move_speed;
	GLfloat mouse_sensitivity;
	GLfloat zoom;

public:
	//构造函数
	Free_Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 Up = glm::vec3(0.0, 1.0, 0.0))
		: position(Position), front(Front), up(Up), world_up(Up)
	{
		yaw = YAW;
		pitch = PITCH;
		move_speed = SPEED;
		mouse_sensitivity = SENSITIVITY;
		zoom = ZOOM;
		UpdataCameraVectors();
	}

	//返回视点矩阵，此视点矩阵将被传入着色器中使用
	//这也是相机的本质
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	//处理键盘操作：W前进，S后退，A左平移，D右平移
	void ProcessKeyboard(CameraMovement direction, GLfloat delta_time)
	{
		GLfloat velocity = move_speed * delta_time;
		if (direction == FORWARD)
			position += velocity * glm::normalize(glm::vec3(front.x, front.y, front.z));
		if (direction == BACKWARD)
			position -= velocity * glm::normalize(glm::vec3(front.x, front.y, front.z));
		if (direction == LEFT)
			position -= velocity * right;
		if (direction == RIGHT)
			position += velocity * right;
	}

	//处理鼠标操作，默认限制俯仰角的角度不超过-90度和90度
	void ProcessMouseMovement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch = true)
	{
		//使用灵敏度
		x_offset *= mouse_sensitivity;
		y_offset *= mouse_sensitivity;

		pitch += y_offset;
		yaw += x_offset;

		if (constrain_pitch)
		{
			if (pitch > 89.0)
				pitch = 89.0;
			if (pitch < -89.0)
				pitch = -89.0;
		}
		UpdataCameraVectors();
	}

	//处理滚轮操作，滚轮向上滑动
	void ProcessMouseScroll(GLfloat y_offset)
	{
		if (zoom >= 1.0 && zoom <= 45.0)
			zoom -= y_offset;
		if (zoom <= 1.0)
			zoom = 1.0;
		if (zoom >= 45.0)
			zoom = 45.0;
	}

private:
	
	//更新相机的朝向
	void UpdataCameraVectors()
	{
		//根据当前的欧拉角计算相机的朝向
		glm::vec3 current_front;
		current_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		current_front.y = sin(glm::radians(pitch));
		current_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(current_front);

		//更新相机坐标系
		right = glm::normalize(glm::cross(front, world_up));
		up = glm::normalize(glm::cross(right, front));
	}

};

#endif