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
#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;

#pragma region paintingVariables
double uvProjectionX;
double uvProjectionY;
int paintingSqDistance = 50;
double paintingSqX, paintingSqY;
vector<glm::vec2> unprojectedVertices;
glm::vec3 unprojected;
glm::vec2 holdDistance = glm::vec2(0, 0);
double ax, ay, bx, by, cx, cy;
double w1, w2;
bool outsideAreaCheck;
bool enableDraw;
glm::vec3 paintingColor = glm::vec3(255, 255, 255);
glm::vec3 originalColor;
glm::vec3 smallColorValues;
glm::vec3 highColorValues;
float maskGrayScale;
double originalX_p1, originalY_p1, originalZ_p1;
double originalX_p2, originalY_p2, originalZ_p2;
double originalX_p3, originalY_p3, originalZ_p3;
double cameraFaceDistance;
double imgX;
double imgY;
int pixelX;
int pixelY;

int paintingQuality = 1;
#pragma endregion

void MSHP_Texture_Generator::paintTexture(GLFWwindow* window, std::vector<float> &vertices,glm::vec3 cameraPos, glm::vec3 originPos, double mouseXpos, double mouseYpos, GLubyte* pixelsTxtr, GLubyte* pixelsMask,float panelLoc) {
	MSHPApp MSHP;
	commonData cmnd;
	MSHP_userInterface ui;
	unprojectedVertices.clear();
	glm::vec2 holdDistance = glm::vec2(0, 0);
	for (size_t i = 0; i < vertices.size(); i++)
	{
		unprojected = MSHP.getUnprojection(glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]), cameraPos, originPos);
		unprojected.y = 1080/2 - (unprojected.y - 1080 / 2);
		unprojectedVertices.push_back(glm::vec2(unprojected.x, unprojected.y));
		int isAxisPointerLoc = glGetUniformLocation(cmnd.program, "isAxisPointer");
		glUniform1i(isAxisPointerLoc, 0);
		i += 7;
	}
	for (size_t i = 0; i < unprojectedVertices.size(); i++)
	{
		MSHP.renderModel(vertices,panelLoc);
		ax = unprojectedVertices[i].x;
		ay = unprojectedVertices[i].y;
		bx = unprojectedVertices[i + 1].x;
		by = unprojectedVertices[i + 1].y;
		cx = unprojectedVertices[i + 2].x;
		cy = unprojectedVertices[i + 2].y;
		w1 = (ax * (cy - ay) + (mouseYpos - ay) * (cx - ax) - mouseXpos * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
		w2 = (mouseYpos - ay - w1 * (by - ay)) / (cy - ay);
		if (w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1) {
			outsideAreaCheck = true;
			originalX_p1 = vertices[i * 8];
			originalY_p1 = vertices[i * 8 + 1];
			originalZ_p1 = vertices[i * 8 + 2];

			originalX_p2 = vertices[(i + 1) * 8];
			originalY_p2 = vertices[(i + 1) * 8 + 1];
			originalZ_p2 = vertices[(i + 1) * 8 + 2];

			originalX_p3 = vertices[(i + 2) * 8];
			originalY_p3 = vertices[(i + 2) * 8 + 1];
			originalZ_p3 = vertices[(i + 2) * 8 + 2];

			cameraFaceDistance = sqrt(pow(cameraPos.x - originalX_p1, 2) + pow(cameraPos.y - originalY_p1, 2) + pow(cameraPos.z - originalZ_p1, 2)) + sqrt(pow(cameraPos.x - originalX_p2, 2) + pow(cameraPos.y - originalY_p2, 2) + pow(cameraPos.z - originalZ_p2, 2)) + sqrt(pow(cameraPos.x - originalX_p3, 2) + pow(cameraPos.y - originalY_p3, 2) + pow(cameraPos.z - originalZ_p3, 2));				
			if (holdDistance.x == 0 && holdDistance.y == 0) {
				holdDistance.x = cameraFaceDistance;
				holdDistance.y = i;
			}
			else {
				if (holdDistance.x > cameraFaceDistance) 
				{
					holdDistance.x = cameraFaceDistance;
					holdDistance.y = i;
				}
			}
		}

		i += 2;
	}
	for (size_t y = 1; y < 49 * paintingQuality; y++)
	{
		for (size_t x = 2; x < 49 * paintingQuality; x++)
		{
			if (outsideAreaCheck) {
				paintingSqX =(mouseXpos - 25 + (double)x / paintingQuality);
				paintingSqY = mouseYpos - 25 + (double)y / paintingQuality;

				ax = unprojectedVertices[holdDistance.y].x;
				ay = unprojectedVertices[holdDistance.y].y;
				bx = unprojectedVertices[holdDistance.y + 1].x;
				by = unprojectedVertices[holdDistance.y + 1].y;
				cx = unprojectedVertices[holdDistance.y + 2].x;
				cy = unprojectedVertices[holdDistance.y + 2].y;

				w1 = (ax * (cy - ay) + (paintingSqY - ay) * (cx - ax) - paintingSqX * (cy - ay)) / ((by - ay) * (cx - ax) - (bx - ax) * (cy - ay));
				w2 = (paintingSqY - ay - w1 * (by - ay)) / (cy - ay);

				originalX_p1 = vertices[holdDistance.y * 8 + 3];
				originalY_p1 = vertices[holdDistance.y * 8 + 4];

				originalX_p2 = vertices[(holdDistance.y + 1) * 8 + 3];
				originalY_p2 = vertices[(holdDistance.y + 1) * 8 + 4];

				originalX_p3 = vertices[(holdDistance.y + 2) * 8 + 3];
				originalY_p3 = vertices[(holdDistance.y + 2) * 8 + 4];

				uvProjectionX = originalX_p1 + w1 * (originalX_p2 - originalX_p1) + w2 * (originalX_p3 - originalX_p1);
				uvProjectionY = originalY_p1 + w1 * (originalY_p2 - originalY_p1) + w2 * (originalY_p3 - originalY_p1);
				imgX = uvProjectionX * 100.0;
				imgY = uvProjectionY * 100.0;

				pixelX = (1000 / 100) * imgX;
				pixelY = (1000 / 100) * imgY;

				if(pixelY * 3000 + (pixelX * 3) > 0 && pixelY * 3000 + (pixelX * 3) < 1000*1000*3){
					originalColor.x = pixelsTxtr[pixelY * 3000 + (pixelX * 3)];
					originalColor.y = pixelsTxtr[(pixelY * 3000 + (pixelX * 3)) + 1];
					originalColor.z = pixelsTxtr[(pixelY * 3000 + (pixelX * 3)) + 2];
					if (originalColor.x >= paintingColor.x) {
						smallColorValues.x = paintingColor.x;
						highColorValues.x = originalColor.x;
					}
					else
					{
						smallColorValues.x = originalColor.x;
						highColorValues.x = paintingColor.x;
					}

					if (originalColor.y >= paintingColor.y) {
						smallColorValues.y = paintingColor.y;
						highColorValues.y = originalColor.y;
					}
					else
					{
						smallColorValues.y = originalColor.y;
						highColorValues.y = paintingColor.y;
					}

					if (originalColor.z >= paintingColor.z) {
						smallColorValues.z = paintingColor.z;
						highColorValues.z = originalColor.z;
					}
					else
					{
						smallColorValues.z = originalColor.z;
						highColorValues.z = paintingColor.z;
					}

					maskGrayScale = ((float)pixelsMask[y/ paintingQuality * 150 + (x/ paintingQuality * 3)] / 255 + (float)pixelsMask[(y/ paintingQuality * 150 + (x/ paintingQuality * 3)) - 1] / 255 + (float)pixelsMask[(y/ paintingQuality * 150 + (x/ paintingQuality * 3)) - 2] / 255) / 3;

					if (true) {
						if (originalColor.x - paintingColor.x <= 0) {
							pixelsTxtr[pixelY * 3000 + (pixelX * 3)] = smallColorValues.x + (abs(originalColor.x - paintingColor.x) * maskGrayScale);
						}
						else
						{
							pixelsTxtr[pixelY * 3000 + (pixelX * 3)] = highColorValues.x - (abs(originalColor.x - paintingColor.x) * maskGrayScale);
						}
						if (originalColor.y - paintingColor.y <= 0) {
							pixelsTxtr[(pixelY * 3000 + (pixelX * 3)) + 1] = smallColorValues.y + (abs(originalColor.y - paintingColor.y) * maskGrayScale);
						}
						else {
							pixelsTxtr[(pixelY * 3000 + (pixelX * 3)) + 1] = highColorValues.y - (abs(originalColor.y - paintingColor.y) * maskGrayScale);
						}
						if (originalColor.z - paintingColor.z <= 0) {
							pixelsTxtr[(pixelY * 3000 + (pixelX * 3)) + 2] = smallColorValues.z + (abs(originalColor.z - paintingColor.z) * maskGrayScale);
						}
						else {
							pixelsTxtr[(pixelY * 3000 + (pixelX * 3)) + 2] = highColorValues.z - (abs(originalColor.z - paintingColor.z) * maskGrayScale);
						}
					}
				}
				
			}
		}
	}
	outsideAreaCheck = false;
	glActiveTexture(GL_TEXTURE0);
	glTexSubImage2D(GL_TEXTURE_2D, 0,0, 0, 1000, 1000, GL_RGB, GL_UNSIGNED_BYTE, pixelsTxtr);
	glGenerateMipmap(GL_TEXTURE_2D);
}

unsigned int texture::getTexture(const char* path, double imgX, double imgY) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data != NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	return textureID;
}
void texture::drawTexture(const char* path, int width, int height, GLubyte* pixels, int channels) {
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(path, width, height, channels, pixels, width*channels);
}
GLubyte* texture::getTextureFromProgram(int texture, int width, int height,int channels) {
	GLubyte* pixels = new GLubyte[width * height * channels * sizeof(GLubyte)*2];
	glActiveTexture(texture);
	if (channels == 3) {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}
	else {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}
	return pixels;
}