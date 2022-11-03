#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "RigidPainter.h"
#include "gl.h"
#include "Texture.h"
#include "TextureGenerator.h"
#include <vector>

using namespace std;

TextureData textureData;
GLubyte* resizedPixels = new GLubyte[50 * 50 * 3];;
float lastBrushSizeRangeBarValue = 0.0f;

double lastMouseXPos = 0;
double lastMouseYPos = 0;
void TextureGenerator::drawTexture(GLFWwindow* window, string path, bool brushTextureChanged, unsigned int screenPaintingTextureId, float brushSizeRangeBarValue) {
	Texture texture;
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

			getTextureData = texture.getTexture(path, 0, 0, true, brushSizeRangeBarValue);
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
		glTexSubImage2D(GL_TEXTURE_2D, 0, mouseXpos + ((1920 - screenSizeX) / 2) - distanceX / 2, 1080 - mouseYpos - ((1080 - screenSizeY) / 2) - distanceY / 2, distanceX, distanceY, GL_RGB, GL_UNSIGNED_BYTE, resultSquare);
		glset.generateMipmap();

		delete(screenTextureSquare);
		delete(resultSquare);
		glset.deleteFramebuffers(FBO);
	}
	lastMouseXPos = mouseXpos;
	lastMouseYPos = mouseYpos;

}
