#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "MSHPApp.h"
#include "stb_image_write.h"

using namespace std;


GLFWwindow* MSHPApp::init() {
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Mesh Painter", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	return window;
}
std::string MSHPApp::readFile(const char* path) {
	string fullText;
	fstream my_file;
	my_file.open(path, ios::in);
	if (my_file.is_open()) {
		char mychar;
		while (my_file) {
			mychar = my_file.get();
			fullText += mychar;
		}
	}
	my_file.close();
	return fullText;
}
void MSHPApp::getProgram() {
	commonData cmnd;
	std::string fshader = readFile("fragmentShaderSource.txt");
	fshader.pop_back();
	const char* fragmentShaderSource = fshader.c_str();
	std::string vshader = readFile("vertexShaderSource.txt");
	vshader.pop_back();
	const char* vertexShaderSource = vshader.c_str();

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

#pragma region vertex_compilation_check
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
#pragma endregion

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

#pragma region fragment_compilation_check
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
#pragma endregion

	//Program
	cmnd.program = glCreateProgram();

	glAttachShader(cmnd.program, vertexShader);
	glAttachShader(cmnd.program, fragmentShader);
	glLinkProgram(cmnd.program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
#pragma region program_link_check
	glGetProgramiv(cmnd.program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(cmnd.program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
#pragma endregion
}

projectionData MSHPApp::transformObject( glm::vec3 cameraPos , glm::vec3 originPos) {
	commonData cmnd;
	glm::mat4 view;
	view = glm::lookAt(cameraPos, originPos, glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	int modelLoc = glGetUniformLocation(cmnd.program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(cmnd.program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projectionLoc = glGetUniformLocation(cmnd.program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	projectionData pd;
	pd.modelMat = model;
	pd.projMat = projection;
	pd.viewMat = view;
	return pd;
}
glm::vec3 MSHPApp::getUnprojection(glm::vec3 vPos,glm::vec3 cameraPos, glm::vec3 originPos) {
	MSHPApp mshp;
	commonData cmnd;
	projectionData pd = mshp.transformObject(cameraPos, originPos);
	return glm::project(vPos, pd.modelMat * pd.viewMat, pd.projMat, glm::vec4(0, 0, 1920, 1080));
}
void MSHPApp::drawLightObject( glm::vec3 lightPos) {
	commonData cmnd;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	int lightColorLoc = glGetUniformLocation(cmnd.program, "lightColor");
	glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 1);
	int modelLoc = glGetUniformLocation(cmnd.program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void MSHPApp::renderModel(std::vector<float>& vertices, float panelLoc){
	glSet gls;
	vector<float>axisPointer{
		0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		-100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
		0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
	};
	MSHP_userInterface ui;
	commonData cmnd;
	colorData colorD;

	gls.meshDataToShaders();
	gls.drawArrays(vertices,false);

	gls.uiDataToShaders(glm::vec3(colorD.buttonColor.x, colorD.buttonColor.y, colorD.buttonColor.z));
	ui.button(0.1f, panelLoc/2, -0.8f);
	gls.uiDataToShaders(glm::vec3(colorD.panelColor.x, colorD.panelColor.y, colorD.panelColor.z));
	ui.panel(panelLoc);
	gls.axisPointerDataToShaders();
	gls.drawArrays(axisPointer, true);
}