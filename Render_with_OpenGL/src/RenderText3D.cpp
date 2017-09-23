#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <stb_image.h>

#include <Shader.h>
#include <Camera.h>
#include <Light.h>
#include <Texture2D.h>
#include <Mesh.h>

#include <Glyph3D.h>
#include <FreeTypeFont.h>
#include <Tessellator.h>

#include <iostream>
#include <cmath>
#include <vector>

//window's width and height
GLuint WIDTH = 800;
GLuint HEIGHT = 600;

enum Uniform_IDs{ lights, VPmatrix, NumUniforms };
enum Attrib_Ids{ vPostion, vNormal, vTexCoord };

GLuint Uniforms[NumUniforms];

glm::vec3 cameraPos(0.0f, 1.0f, 5.0f);
FreeCamera camera(cameraPos);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

bool firstMouse = true;
//the cursor's position in last frame
GLfloat lastX = GLfloat(WIDTH) / 2;
GLfloat lastY = GLfloat(HEIGHT) / 2;

const GLint PointLightNum = 1;
const GLint SpotLightNum = 1;

const glm::vec3 pointLightPositions[PointLightNum] =
{
	glm::vec3(0.2f, 0.6f, 3.0f),
};

const glm::vec3 spotLightPositions[SpotLightNum] =
{
	glm::vec3(0.0f, 4.5f, 0.0f)
};

//initialize GLFW window option
void initWindowOption();
//process input
void processInput(GLFWwindow *window);
//frame buffer size callback
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow *window, double x, double y);
void scrollCallback(GLFWwindow *window, double x, double y);

int main()
{
	std::cout << "OpenGL 3.3 GO! Let's make some fun!" << std::endl;

	initWindowOption();
	//create a glfw window object
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Window", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window!";
		glfwTerminate();
		std::abort();
	}
	glfwMakeContextCurrent(window);

	//initilize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to init glad!";
		std::abort();
	}

	//create viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	//set background color
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//set callback
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/////////////////////////////////////////Objects//////////////////////////////////////////////////
	//输入要显示的文字及字体
	std::vector<GLuint> advances;
	std::vector<Mesh> string3D;

	std::wstring text = L"北邮·IPOC OSG";

	for (const auto &c : text)
	{
		//获取字形信息
		if (c == ' ')
		{
			advances.back() *= 2;
			continue;
		}
		FreeTypeFont temp_char(c, "../fonts/stxinwei.ttf");
		//添加每个字形的步进宽度
		advances.push_back(temp_char.advanceX());
		//将字形信息存入绘制单元
		Glyph3D glyph = temp_char.getGlyph3D();
		//生成3D文字模型
		computeGlyphGeometry(glyph, 3.0);

		std::vector<glm::vec3> normals = glyph.getNormalArray();
		ElementArray indices = glyph.getIndices();
		std::vector<Vertex> verts(indices.size());

		for (size_t i = 0; i < indices.size(); ++i)
			verts.push_back({ glyph._vertices[indices[i]], normals[i], glm::vec2() });

		string3D.emplace_back(verts);
		//调试用输出
		//glyph.output();
	}

	////////////////////////////////////////Shaders/////////////////////////////////////////////////////
	Shader textShader("shader/text3D.vert", "shader/text3D.frag");


	//create lights ubo
	glGenBuffers(NumUniforms, Uniforms);
	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[lights]);
	glBufferData(GL_UNIFORM_BUFFER, 64 + PointLightNum * 80 + SpotLightNum * 112, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, Uniforms[lights]);

	glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[VPmatrix]);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Uniforms[VPmatrix]);

	//////////////////////////////////////////////Lights////////////////////////////////////////////////////
	//directional light
	DirLight dLight
	{
		glm::vec3(-0.2f, -1.0f, -0.3f),
		glm::vec3(0.01f, 0.01f, 0.01f),
		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.5f, 0.5f, 0.5f)
	};
	//point lights
	std::vector<PointLight> pLights;
	for (size_t i = 0; i < PointLightNum; ++i)
	{
		PointLight pLight
		{
			pointLightPositions[i],
			glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(0.8f, 0.8f, 0.8f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			{ 1.0f, 0.09f, 0.032f }
		};
		pLights.push_back(pLight);
	}
	//spot lights
	std::vector<SpotLight> sLights;
	for (size_t i = 0; i < SpotLightNum; ++i)
	{
		SpotLight torch
		{
			camera.position,
			glm::vec3(0.0f, -1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			{ 1.0f, 0.09f, 0.032f },
			{ glm::cos(glm::radians(8.0f)), glm::cos(glm::radians(15.0f)) }
		};
		sLights.push_back(torch);
	}

	//set the lights uniform
	dLight.setUniform(Uniforms[lights], 0);
	for (size_t i = 0; i < PointLightNum; ++i)
	{
		GLuint baseOffset = 64 + i * 80;
		pLights[i].setUniform(Uniforms[lights], baseOffset);
	}
	
	//////////////////////////////////////////////Materials////////////////////////////////////////////////////
	textShader.use();
	textShader.setUniformVec3("material.ambient", glm::vec3(1.0f, 1.0f, 0.0f));
	textShader.setUniformVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 0.0f));
	textShader.setUniformVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	textShader.setUniformFloat("material.shininess", 500.0f);

	glEnable(GL_DEPTH_TEST);

	///////////////////////////////////////////////////Game Loop////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currTime = glfwGetTime();
		deltaTime = currTime - lastTime;
		lastTime = currTime;

		//clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//process input
		processInput(window);
		//check events
		glfwPollEvents();
		//view matrix and projection matrix
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 proj = glm::perspective(glm::radians(camera.zoom), (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);

		glBindBuffer(GL_UNIFORM_BUFFER, Uniforms[VPmatrix]);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		
		sLights[0].position = camera.position;
		sLights[0].direction = camera.front;
		GLuint baseOffset = 64 + PointLightNum * 80;
		sLights[0].setUniform(Uniforms[lights], baseOffset);

		textShader.use();
		textShader.setUniformVec3("viewPos", camera.position);
		for (size_t i = 0, advance = 0; i < string3D.size(); advance += advances[i], ++i)
		{
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(-0.9 + advance * 0.05f, 0.6f, 0.0f));
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
			textShader.setUniformMat4("model", model);

			string3D[i].draw(textShader);
		}

		//swap frame buffer
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	std::cout << "Done!" << std::endl;

	return 0;
}

void initWindowOption()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//we have no need to use the compatibility profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void processInput(GLFWwindow *window)
{
	//if we press the key_esc, the window will be closed!
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow *window, double x, double y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	GLfloat xoffset = x - lastX;
	GLfloat yoffset = lastY - y;

	lastX = x;
	lastY = y;

	camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow *window, double x, double y)
{
	camera.processMouseScroll(y);
}