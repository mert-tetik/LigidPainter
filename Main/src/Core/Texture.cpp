#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "RigidPainter.h"
#include "gl.h"
#include "Texture.h"
#include <vector>
#include "stb_image.h"
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

using namespace std;

TextureData textureData;
GLubyte* resizedPixels  = new GLubyte[50 * 50 * 3];;
float lastBrushSizeRangeBarValue = 0.0f;

double lastMouseXPos = 0;
double lastMouseYPos = 0;
void Texture::drawTexture(GLFWwindow* window, string path,bool brushTextureChanged,unsigned int screenPaintingTextureId, float brushSizeRangeBarValue) {
	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);
	if (lastMouseXPos - mouseXpos != 0 || lastMouseYPos - mouseYpos != 0) {
		
		bool brushSizeChanged = false;
		if (lastBrushSizeRangeBarValue != brushSizeRangeBarValue) {
			brushSizeChanged = true;
		}
		lastBrushSizeRangeBarValue = brushSizeRangeBarValue;

		GlSet glset;
		Texture txtr;
		int distanceX = double(brushSizeRangeBarValue + 0.1f) * 500.0 + 10.0;
		int distanceY = distanceX;

		if (brushSizeChanged || brushTextureChanged) {
			resizedPixels = new GLubyte[distanceX * distanceY * 3]; //deleted
			glset.activeTexture(GL_TEXTURE1);
			textureData = txtr.getTextureData(path.c_str());
			GetTextureData getTextureData;

			getTextureData = getTexture(path, 0, 0, true, brushSizeRangeBarValue);
			resizedPixels = getTextureData.imgData;
		}
		GLubyte* resultSquare = new GLubyte[distanceX * distanceY * 3]; //deleted
		GLfloat* screenTextureSquare = new GLfloat[distanceX * distanceY * 3]; //deleted
		
		unsigned int FBO;
		glset.genFramebuffers(FBO);
		glset.bindFramebuffer(FBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenPaintingTextureId, 0);
		if (screenTextureSquare) {
			glReadPixels(mouseXpos + ((1920 - screenSizeX) / 2) - distanceX / 2, 1080 - mouseYpos - ((1080 - screenSizeY) / 2) - distanceY / 2, distanceX, distanceY, GL_RGB, GL_FLOAT, screenTextureSquare);
		}
		glset.bindFramebuffer(0);
		for (size_t i = 0; i < distanceX * distanceY * 3; i++)
		{
			resultSquare[i] = min(resizedPixels[i] + (int)(screenTextureSquare[i] * 255), 255);
		}
		glset.activeTexture(GL_TEXTURE4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, mouseXpos+((1920-screenSizeX)/2) - distanceX / 2, 1080 - mouseYpos - ((1080 - screenSizeY) / 2) - distanceY / 2, distanceX, distanceY, GL_RGB, GL_UNSIGNED_BYTE, resultSquare);
		glset.generateMipmap();

		delete(screenTextureSquare);
		delete(resultSquare);
		glset.deleteFramebuffers(FBO);
	}
	lastMouseXPos = mouseXpos;
	lastMouseYPos = mouseYpos;
	
}

GetTextureData Texture::getTexture(std::string path, double imgX, double imgY,bool resize,float brushSizeRangeBarValue) {
	GetTextureData getTextureData;
	unsigned int textureID;
	GlSet glset;
	glset.genTextures(textureID);
	glset.bindTexture(textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	int distanceX = double(brushSizeRangeBarValue + 0.1f) * 500.0 + 10.0;

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 3);
	GLubyte* resizedPixelsX = new GLubyte[distanceX * distanceX * 3];

	if (data != NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glset.generateMipmap();
	}
	else
	{
		const char* reason = "[unknown reason]";
		if (stbi_failure_reason())
		{
			reason = stbi_failure_reason();
		}
		std::cout << "Failed to load texture!" << " Reason : " << reason<< std::endl;
	}
	stbir_resize_uint8(data, width, height, 0, resizedPixelsX, distanceX, distanceX, 0, 3);

	stbi_image_free(data);
	getTextureData.Id = textureID;
	getTextureData.imgData = resizedPixelsX;
	return getTextureData;
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
unsigned int Texture::createScreenPaintTexture(GLubyte* &screenTexture,GLFWwindow* window) {
	std::fill_n(screenTexture, 1920 * 1080* 3, 0);
	GlSet glset;

	glset.activeTexture(GL_TEXTURE4);
	unsigned int textureID;
	glset.genTextures(textureID);
	glset.bindTexture(textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, screenTexture);
	glset.generateMipmap();
	return textureID;
}