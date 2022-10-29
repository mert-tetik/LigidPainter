#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "RigidPainter.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

using namespace std;

GLubyte* resizedPixels = new GLubyte[50 * 50 * 3];
TextureData textureData;

void Texture::drawTexture(GLFWwindow* window, string path,bool brushTextureChanged,unsigned int screenPaintingTextureId) {
	int distanceX = 50;
	int distanceY = 50;

	GlSet glset;
	Texture txtr;
	if (brushTextureChanged) {
		glset.activeTexture(GL_TEXTURE1);
		textureData = txtr.getTextureData(path.c_str());
		resizedPixels = getTextureArray(path,0,0,true);
	}
	GLubyte* resultSquare = new GLubyte[50 * 50 * 3];
	GLfloat* screenTextureSquare = new GLfloat[50 * 50 * 3];

	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(window, &mouseXpos, &mouseYpos);

	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);



	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenPaintingTextureId, 0);
	if (screenTextureSquare) {
		glReadPixels(mouseXpos - distanceX / 2, 1080 - mouseYpos - distanceY / 2, 50, 50, GL_RGB, GL_FLOAT, screenTextureSquare);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for (size_t i = 0; i < 50*50*3; i++)
	{
		resultSquare[i] = min(resizedPixels[i] + (int)(screenTextureSquare[i]*255),255);
	}

	glActiveTexture(GL_TEXTURE4);
	glTexSubImage2D(GL_TEXTURE_2D, 0 , mouseXpos - distanceX/2,1080- mouseYpos - distanceY/2, distanceX, distanceY, GL_RGB, GL_UNSIGNED_BYTE, resultSquare);
	glGenerateMipmap(GL_TEXTURE_2D);

	/*delete(resultSquare);
	delete(screenTextureSquare);*/
}

unsigned int Texture::getTexture(std::string path, double imgX, double imgY,bool resize) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 3);
	GLubyte* resizedPixels = new GLubyte[50 * 50 * 3];


	if (resize) {
		stbir_resize_uint8(data, width, height, 0, resizedPixels, 50, 50, 0, 3);
	}

	if (data != NULL)
	{
		if (resize) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 50, 50, 0, GL_RGB, GL_UNSIGNED_BYTE, resizedPixels);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	stbi_image_free(resizedPixels);
	return textureID;
}
GLubyte* Texture::getTextureArray(std::string path, double imgX, double imgY, bool resize) {
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 3);
	GLubyte* resizedPixels = new GLubyte[50 * 50 * 3];
	if (resize) {
		stbir_resize_uint8(data, width, height, 0, resizedPixels, 50, 50, 0, 3);
	}

	if (data != NULL)
	{
		stbi_image_free(data);
		return resizedPixels;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return NULL;
	}

}
//void Texture::drawTexture(const char* path, int width, int height, GLubyte* pixels, int channels) {
//	stbi_flip_vertically_on_write(true);
//	stbi_write_jpg(path, width, height, channels, pixels, width*channels);
//}
GLubyte* Texture::getTextureFromProgram(int texture, int width, int height,int channels) {
	GLubyte* pixels = new GLubyte[width * height * channels * sizeof(GLubyte)*2];
	glActiveTexture(texture);
	if (channels == 3) {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}
	else if(channels == 4){
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	}
	else if (channels == 1) {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	}
	return pixels;
}
TextureData Texture::getTextureData(const char* path) {
	TextureData td;
	int width, height, channelS;
	stbi_info(path, &width, &height, &channelS);
	td.width = width;
	td.height = height;
	td.channels = channelS;
	return td;
}
unsigned int Texture::createScreenPaintTexture(GLubyte* &screenTexture) {
	std::fill_n(screenTexture, 1920 * 1080 * 3, 0);
	glActiveTexture(GL_TEXTURE4);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, screenTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}