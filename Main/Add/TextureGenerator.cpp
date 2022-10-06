#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>

#include "MSHPApp.h"

#include <vector>

using namespace std;

vector<glm::vec2> unprojectedVertices;
glm::vec3 unprojected;
glm::vec2 holdDistance = glm::vec2(0, 0);

double mouseXpos, mouseYpos;
double ax;
double ay;
double bx;
double by;
double cx;
double cy;
double w1;
double w2;

double originalX_p1;
double originalY_p1;
double originalZ_p1;

double originalX_p2;
double originalY_p2;
double originalZ_p2;

double originalX_p3;
double originalY_p3;
double originalZ_p3;

double cameraFaceDistance;

double imgX;
double imgY;

void MSHP_Texture_Generator::paintTexture(GLFWwindow* window, std::vector<float> vertices,unsigned int shaderProgram,glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 originPos) {
	MSHPApp MSHP;
	unprojectedVertices.clear();
	glm::vec2 holdDistance = glm::vec2(0, 0);
	for (size_t i = 0; i < vertices.size(); i++)
	{
		unprojected = MSHP.getUnprojection(glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]), shaderProgram, cameraPos, cameraFront, cameraUp, originPos);
		unprojected.y = 400 - (unprojected.y - 400);
		unprojectedVertices.push_back(glm::vec2(unprojected.x, unprojected.y));

		i += 7;
	}
	for (size_t i = 0; i < unprojectedVertices.size(); i++)
	{

		glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
		ax = unprojectedVertices[i].x;
		ay = unprojectedVertices[i].y;
		bx = unprojectedVertices[i + 1].x;
		by = unprojectedVertices[i + 1].y;
		cx = unprojectedVertices[i + 2].x;
		cy = unprojectedVertices[i + 2].y;
		w1 = (ax * (cy - ay) + (mouseYpos - ay) * (cx - ax) - mouseXpos * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
		w2 = (mouseYpos - ay - w1 * (by - ay)) / (cy - ay);
		if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {

			originalX_p1 = vertices[i * 8];
			originalY_p1 = vertices[i * 8 + 1];
			originalZ_p1 = vertices[i * 8 + 2];

			originalX_p2 = vertices[(i + 1) * 8];
			originalY_p2 = vertices[(i + 1) * 8 + 1];
			originalZ_p2 = vertices[(i + 1) * 8 + 2];

			originalX_p3 = vertices[(i + 2) * 8];
			originalY_p3 = vertices[(i + 2) * 8 + 1];
			originalZ_p3 = vertices[(i + 2) * 8 + 2];

				cameraFaceDistance = sqrt(pow(cameraPos.x - originalX_p1, 2) + pow(cameraPos.y - originalY_p1, 2) + pow(cameraPos.z - originalZ_p1, 2)) + sqrt(pow(cameraPos.x - originalX_p2, 2) + pow(cameraPos.y - originalY_p2, 2) + pow(cameraPos.z - originalZ_p2, 2)) + sqrt(pow(cameraPos.x - originalX_p3, 2) + pow(cameraPos.y - originalY_p3, 2) + pow(cameraPos.z - originalZ_p3, 2));				if (holdDistance.x == 0 && holdDistance.y == 0) {
				holdDistance.x = cameraFaceDistance;
				holdDistance.y = i;
			}
			else {
				if (holdDistance.x > cameraFaceDistance) {
					holdDistance.x = cameraFaceDistance;
					holdDistance.y = i;
				}
			}
		}
		i += 2;
	}
	std::cout << glm::to_string(unprojectedVertices[holdDistance.y]) << glm::to_string(unprojectedVertices[holdDistance.y + 1]) << glm::to_string(unprojectedVertices[holdDistance.y + 2]) << '\n';

	ax = unprojectedVertices[holdDistance.y].x;
	ay = unprojectedVertices[holdDistance.y].y;
	bx = unprojectedVertices[holdDistance.y + 1].x;
	by = unprojectedVertices[holdDistance.y + 1].y;
	cx = unprojectedVertices[holdDistance.y + 2].x;
	cy = unprojectedVertices[holdDistance.y + 2].y;

	w1 = (ax * (cy - ay) + (mouseYpos - ay) * (cx - ax) - mouseXpos * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
	w2 = (mouseYpos - ay - w1 * (by - ay)) / (cy - ay);

	originalX_p1 = vertices[holdDistance.y * 8 + 3];
	originalY_p1 = vertices[holdDistance.y * 8 + 4];

	originalX_p2 = vertices[(holdDistance.y + 1) * 8 + 3];
	originalY_p2 = vertices[(holdDistance.y + 1) * 8 + 4];

	originalX_p3 = vertices[(holdDistance.y + 2) * 8 + 3];
	originalY_p3 = vertices[(holdDistance.y + 2) * 8 + 4];

	mouseXpos = originalX_p1 + w1 * (originalX_p2 - originalX_p1) + w2 * (originalX_p3 - originalX_p1);
	mouseYpos = originalY_p1 + w1 * (originalY_p2 - originalY_p1) + w2 * (originalY_p3 - originalY_p1);

	cout << '\n' << mouseXpos << ' ' << mouseYpos << '\n';

	imgX = mouseXpos * 100.0;
	imgY = mouseYpos * 100.0;

	glActiveTexture(GL_TEXTURE0);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, MSHP.getTexture("container.png", imgX, imgY));
}