#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "MSHPApp.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"

void glSet::drawArrays(std::vector<float> &vertices,bool isLine) {
	if (!isLine) {
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
	}
	else {
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINES, 0, vertices.size() / 8);
	}
}
void glSet::uiDataToShaders(glm::vec3 color) {
	commonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 1);
	int uiColorLoc = glGetUniformLocation(cmnd.program, "uiColor");
	glUniform3f(uiColorLoc, color.x, color.y, color.z);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 1);
}
void glSet::meshDataToShaders() {
	commonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 0);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 0);
}
void glSet::axisPointerDataToShaders() {
	commonData cmnd;
	int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
	glUniform1i(isAxisPointerLoc, 1);
	int isTwoDimensionalLoc = glGetUniformLocation(cmnd.program, "isTwoDimensional");
	glUniform1i(isTwoDimensionalLoc, 0);
	int is2DLoc = glGetUniformLocation(cmnd.program, "is2D");
	glUniform1i(is2DLoc, 0);
	int isLightSourceLoc = glGetUniformLocation(cmnd.program, "isLightSource");
	glUniform1i(isLightSourceLoc, 0);
}
void glSet::bindFrameBuffer() {
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
}
