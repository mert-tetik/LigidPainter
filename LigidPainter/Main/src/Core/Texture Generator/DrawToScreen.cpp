#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Core/LigidPainter.h"
#include "Core/gl.h"
#include "Core/Texture.h"
#include "Core/Texture Generator/TextureGenerator.h"
#include "stb_image_resize.h"
#include "stb_image.h"
#include <vector>

using namespace std;

std::vector<float> renderVerticesFlipped = {
	// first triangle
	 1.0f,  1.0f, 0.0f,0,1,0,0,0,  // top right
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom right
	 0.0f,  1.0f, 0.0f,1,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom left
	 0.0f,  1.0f, 0.0f,1,1,0,0,0   // top left
};


GLubyte* resizedPixels = new GLubyte[50 * 50 * 3]; //Resized mask texture
GLubyte* renderedImage;
//Check if values changed

//Check if values changed
double lastMouseXPosIn = 0;
double lastMouseYPosIn = 0;

void TextureGenerator::drawToScreen(GLFWwindow* window, string path, bool brushTextureChanged, ScreenPaintingReturnData screenPaintingReturnData, float brushSize,unsigned int FBOScreen, bool brushBlurChanged,bool brushSizeChanged,float rotationValue,bool brushRotationChanged, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos) {
	Texture texture;
	CommonData commonData;
	
	double mouseXposIn;
	double mouseYposIn;
	glfwGetCursorPos(window, &mouseXposIn, &mouseYposIn);
	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window, &screenSizeX, &screenSizeY);
	//Get context data


	GlSet glset;
	Texture txtr;

	//Get brush size
	int distanceX = brushSize;
	int distanceY = distanceX;
	//Get brush size

	//----------------------SET BRUSH TEXTURE----------------------\\
						  (Interpreted with blur value)
	if ((brushSizeChanged || brushTextureChanged || brushBlurChanged || brushRotationChanged)) {
		delete(resizedPixels);
		delete(renderedImage);

		//Setup
		resizedPixels = new GLubyte[distanceX * distanceY * 3];

		renderedImage = txtr.updateMaskTexture(FBOScreen, screenSizeX, screenSizeY, rotationValue);
		//Resize
		if (true) {
			stbir_resize_uint8(renderedImage, 540, 540, 0, resizedPixels, distanceX, distanceY, 0, 3); //Resize (causes lags)
		}
		//Resize
	}
	if (lastMouseXPosIn - mouseXposIn != 0 || lastMouseYPosIn - mouseYposIn != 0) {
		

		//----------------------PAINTING----------------------\\

		int reduceTheDifference = 5;

		int differenceBetweenMousePoints = glm::distance(glm::vec2(mouseXpos, mouseYpos), glm::vec2(lastMouseXPos, lastMouseYPos))/reduceTheDifference;

		float xposDif = (mouseXpos - lastMouseXPos) / differenceBetweenMousePoints;
		float yposDif = (mouseYpos - lastMouseYPos) / differenceBetweenMousePoints;
		if (differenceBetweenMousePoints <= 10) {
			differenceBetweenMousePoints = 1;
		}
			unsigned int FBO;
			glset.genFramebuffers(FBO);
			glset.bindFramebuffer(FBO);
			for (size_t i = 0; i < differenceBetweenMousePoints; i++)
			{
				if (differenceBetweenMousePoints > 10) {
					mouseXpos -= xposDif;
					mouseYpos -= yposDif;
				}
				
				GLubyte* resultSquare = new GLubyte[distanceX * distanceY * 3]; //Write to that array after interpreting resized texture with screen texture
				GLfloat* screenTextureSquare = new GLfloat[distanceX * distanceY * 3]; //Painting area of the screen texture
				 
				//Get the painting area of the screen texture to the screenTextureSquare


				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenPaintingReturnData.normalId, 0);
				if (screenTextureSquare) {
					glReadPixels(mouseXpos + ((1920 - screenSizeX) / 2) - distanceX / 2, 1080 - mouseYpos - ((1080 - screenSizeY) / 2) - distanceY / 2, distanceX, distanceY, GL_RGB, GL_FLOAT, screenTextureSquare);
				}
				//Get the painting area of the screen texture to the screenTextureSquare

				float opacity = ((opacityRangeBarValue + 0.11f) * 4.54545454545f); //-0.11 - 0.11 --> 0 - 1
				//Avoid writing low value onto high value
				for (size_t i = 0; i < distanceX * distanceY * 3; i++)
				{
					//glm::mix();
					//resultSquare[i] = max((int)resizedPixels[i], (int)(screenTextureSquare[i] * 255)); //take max value
					resultSquare[i] = glm::mix(screenTextureSquare[i], opacity, resizedPixels[i] / 255.0f) * 255; //Mix
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
				//finish
			}

		//Update mirrored screen mask texture
		//setup
		glset.viewport(1920, 1080);
		glset.bindFramebuffer(FBOScreen);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glset.uniform1i(commonData.program, "isTwoDimensional", 0);
		glset.uniform1i(commonData.program, "isRenderTextureMode", 1);
		glset.uniform1i(commonData.program, "isRenderScreenMaskMode", 1);
		glset.uniform1i(commonData.program, "isRenderTextureModeV", 1);
		glset.uniform1i(commonData.program, "verticalMirror", 1);
		//setup

		//Get texture
		glset.drawArrays(renderVerticesFlipped, false);
		GLubyte* renderedScreen = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
		glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, renderedScreen);
		glset.activeTexture(GL_TEXTURE3);
		glset.texImage(renderedScreen, 1080, 1080, GL_RGB);
		glset.generateMipmap();
		delete(renderedScreen);
		//Get texture

		//Finish
		glset.uniform1i(commonData.program, "isRenderTextureModeV", 0);
		glset.uniform1i(commonData.program, "isRenderTextureMode", 0);
		glset.uniform1i(commonData.program, "isRenderScreenMaskMode", 0);
		glset.viewport(screenSizeX, screenSizeY);
		glset.bindFramebuffer(0);
		glset.deleteFramebuffers(FBO);
		//Finish
		//Update mirrored screen mask texture
	}
	lastMouseXPosIn = mouseXposIn;
	lastMouseYPosIn = mouseYposIn;
}