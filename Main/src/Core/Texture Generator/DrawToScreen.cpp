#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "LigidPainter.h"
#include "gl.h"
#include "Texture.h"
#include "TextureGenerator.h"
#include "stb_image_resize.h"
#include "stb_image.h"
#include <vector>

using namespace std;

GLubyte* resizedPixels = new GLubyte[50 * 50 * 3]; //Resized mask texture
GLubyte* renderedImage;
//Check if values changed
double lastMouseXPos = 0;
double lastMouseYPos = 0;
//Check if values changed


void TextureGenerator::drawToScreen(GLFWwindow* window, string path, bool brushTextureChanged, unsigned int screenPaintingTextureId, float brushSize,unsigned int FBOScreen, bool brushBlurChanged,bool brushSizeChanged,float rotationValue,bool brushRotationChanged) {
	Texture texture;
	
	//Get context data
	double mouseXpos;
	double mouseYpos;
	glfwGetCursorPos(window, &mouseXpos, &mouseYpos);
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);
	//Get context data

	if (lastMouseXPos - mouseXpos != 0 || lastMouseYPos - mouseYpos != 0) {
		GlSet glset;
		Texture txtr;

		//Get brush size
		int distanceX = brushSize;
		int distanceY = distanceX;
		//Get brush size

		//----------------------SET BRUSH TEXTURE----------------------\\
						  (Interpreted with blur value)
		if ((brushSizeChanged || brushTextureChanged || brushBlurChanged|| brushRotationChanged)) {
			CommonData commonData;
			delete(resizedPixels);
			delete(renderedImage);

			//Setup
			resizedPixels = new GLubyte[distanceX * distanceY * 3]; 
			
			renderedImage = txtr.updateMaskTexture(FBOScreen,screenSizeX, screenSizeY,rotationValue);
			//Resize
			if (true) {
				stbir_resize_uint8(renderedImage, 540, 540, 0, resizedPixels, distanceX, distanceY, 0, 3); //Resize (causes lags)
			}
			//Resize
		}

		//----------------------PAINTING----------------------\\

		GLubyte* resultSquare = new GLubyte[distanceX * distanceY * 3]; //Write to that array after interpreting resized texture with screen texture
		GLfloat* screenTextureSquare = new GLfloat[distanceX * distanceY * 3]; //Painting area of the screen texture

		//Get the painting area of the screen texture to the screenTextureSquare
		unsigned int FBO;
		glset.genFramebuffers(FBO);
		glset.bindFramebuffer(FBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenPaintingTextureId, 0);
		if (screenTextureSquare) {
			glReadPixels(mouseXpos + ((1920 - screenSizeX) / 2) - distanceX / 2, 1080 - mouseYpos - ((1080 - screenSizeY) / 2) - distanceY / 2, distanceX, distanceY, GL_RGB, GL_FLOAT, screenTextureSquare);
		}
		glset.bindFramebuffer(0);
		//Get the painting area of the screen texture to the screenTextureSquare

		//Avoid writing low value onto high value
		for (size_t i = 0; i < distanceX * distanceY * 3; i++)
		{
			//glm::mix();
			//resultSquare[i] = max((int)resizedPixels[i], (int)(screenTextureSquare[i] * 255)); //take max value
			resultSquare[i] = glm::mix(screenTextureSquare[i], 1.0f , resizedPixels[i]/255.0f) * 255; //Mix
			//resultSquare[i] = min(resizedPixels[i] + (int)(screenTextureSquare[i] * 255), 255); //sum up
		}
		//Avoid writing low value onto high value

		//Paint screen mask texture with resultSquare
		glset.activeTexture(GL_TEXTURE4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, mouseXpos + ((1920 - screenSizeX) / 2) - distanceX / 2, 1080 - mouseYpos - ((1080 - screenSizeY) / 2) - distanceY / 2, distanceX, distanceY, GL_RGB, GL_UNSIGNED_BYTE, resultSquare);
		glset.generateMipmap();
		//Paint screen mask texture with resultSquare

		//finish
		delete(screenTextureSquare);
		delete(resultSquare);
		glset.deleteFramebuffers(FBO);
		//finish
	}
	lastMouseXPos = mouseXpos;
	lastMouseYPos = mouseYpos;
}