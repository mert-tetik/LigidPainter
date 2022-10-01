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

#include "MSHPApp.h"

using namespace std;
GLFWwindow* MSHPApp::MHSPInit() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 800, "Mesh Painter", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	return window;
}
std::string MSHPApp::READ_FILE(const char* path) {
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
unsigned int MSHPApp::SetShadersAndProgram() {
	std::string fratext = READ_FILE("fragmentShaderSource.txt");
	fratext.pop_back();
	const char* fragmentShaderSource = fratext.c_str();
	std::string vertext = READ_FILE("vertexShaderSource.txt");
	vertext.pop_back();
	const char* vertexShaderSource = vertext.c_str();

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//Shader, Const Char Pointer Count, Const Char Pointer
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
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
#pragma region program_link_check
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
#pragma endregion

	return shaderProgram;
}
void MSHPApp::SetLocations(unsigned int program, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 cameraDirection) {
	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	int modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projectionLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void MSHPApp::drawLigtObject(unsigned int program, glm::vec3 lightPos) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	int objectColorLoc = glGetUniformLocation(program, "objectColor");
	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	int lightColorLoc = glGetUniformLocation(program, "lightColor");
	glUniform3f(lightColorLoc, 10.0f, 10.0f, 10.0f);
	int isLightSourceLoc = glGetUniformLocation(program, "isLightSource");
	glUniform1i(isLightSourceLoc, 1);
	int modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
unsigned int MSHPApp::getTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data != NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return textureID;
}
