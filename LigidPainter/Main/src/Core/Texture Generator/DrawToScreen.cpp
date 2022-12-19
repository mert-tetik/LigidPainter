#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Core/LigidPainter.h"
#include "Core/gl.h"
#include "Core/Texture.h"
#include "Core/Render.h"
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

bool addToScreenMaskTxtr = true;

void TextureGenerator::drawToScreen(GLFWwindow*& window, string& path, unsigned int  screenPaintingTxtrId, float brushSize,unsigned int FBOScreen,float rotationValue, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos, bool mirrorUsed, bool useNegativeForDrawing,bool brushValChanged,int paintingFillNumericModifierVal,Programs& programs,int maxScreenWidth,int maxScreenHeight,bool reduceScreenPaintingQuality,float brushBorderRangeBarValue,float brushBlurVal,unsigned int FBO,OutShaderData &outShaderData,Model &model,vector<unsigned int> &albedoTextures) {
	
	//TODO : Remove path parameter
	
	Texture texture;
	Render render;

	double mouseXposIn;
	double mouseYposIn;
	glfwGetCursorPos(window, &mouseXposIn, &mouseYposIn);

	int screenSizeX;
	int screenSizeY;
	glfwGetWindowSize(window,&screenSizeX,&screenSizeY);
	//Get context data

	float screenGapX = ((float)maxScreenWidth - screenSizeX)/4.0f; 

	GlSet glset;
	Texture txtr;

	int qualityDivider = reduceScreenPaintingQuality+1; //Divide by 2 if reduceScreenPaintingQuality is true

	//Get brush size
	int distanceX = brushSize/qualityDivider;
	int distanceY = distanceX;

	//Get brush size

	//----------------------SET BRUSH TEXTURE----------------------\\
						  (Interpreted with blur value)
	if ((brushValChanged)) {
		delete(resizedPixels);
		delete(renderedImage);
		//Setup
		resizedPixels = new GLubyte[distanceX * distanceY * 3];

		renderedImage = txtr.updateMaskTexture(FBOScreen, screenSizeX,screenSizeY, rotationValue,false,brushBorderRangeBarValue,brushBlurVal,outShaderData);
		//Resize
		if (true) {
			stbir_resize_uint8(renderedImage, 540, 540, 0, resizedPixels, distanceX, distanceY, 0, 3); //Resize (causes lags)
		}
		//Resize
	}
	if (lastMouseXPosIn - mouseXposIn != 0 || lastMouseYPosIn - mouseYposIn != 0) {
		
		//----------------------PAINTING----------------------\\
		
		int reduceTheDifference = 21 - (paintingFillNumericModifierVal*2);

		int differenceBetweenMousePoints = glm::distance(glm::vec2(mouseXpos, mouseYpos), glm::vec2(lastMouseXPos, lastMouseYPos))/reduceTheDifference;

		float xposDif = (mouseXpos - lastMouseXPos) / differenceBetweenMousePoints;
		float yposDif = (mouseYpos - lastMouseYPos) / differenceBetweenMousePoints;
		if (differenceBetweenMousePoints <= 10) {
			differenceBetweenMousePoints = 1;
		}


			float opacity = ((opacityRangeBarValue + 0.11f) * 4.54545454545f); //-0.11 - 0.11 --> 0 - 1

			glset.bindFramebuffer(FBO);

			for (size_t i = 0; i < differenceBetweenMousePoints; i++)
			{
				if (differenceBetweenMousePoints > 10) {
					mouseXpos -= xposDif;
					mouseYpos -= yposDif;
				}

				if(true){
					addToScreenMaskTxtr = false;
					GLubyte* resultSquare = new GLubyte[distanceX * distanceY]; //Write to that array after interpreting resized texture with screen texture
					GLfloat* screenTextureSquare = new GLfloat[distanceX * distanceY]; //Painting area of the screen texture

					//Get the painting area of the screen texture to the screenTextureSquare
					glReadPixels((mouseXpos/qualityDivider + screenGapX- distanceX / 2), (maxScreenHeight/qualityDivider - mouseYpos/qualityDivider - distanceY / 2), distanceX, distanceY, GL_RED, GL_FLOAT, screenTextureSquare);
					
					//Get the painting area of the screen texture to the screenTextureSquare
					int test = 0;
					//Avoid writing low value onto high value
					for (size_t i = 0; i < distanceX * distanceY; i++)
					{
						//resultSquare[i] = max((int)resizedPixels[i], (int)(screenTextureSquare[i] * 255)); //take max value
						//resultSquare[i] = min(max((int)resizedPixels[i], (int)(screenTextureSquare[i] * 255)) + (int)(screenTextureSquare[i] * 255)/10,255); //take max value

						if(useNegativeForDrawing)
							resultSquare[i] = max(255.0f - (float)resizedPixels[i*3] * opacity, (float)(screenTextureSquare[i] * 255.0f)); //take max value
							//resultSquare[i] = glm::mix(screenTextureSquare[i], opacity, 1.0f -(max(resizedPixels[i*3] - screenTextureSquare[i],0.0001f) / 255.0f)) * 255; //Mix

						else
							resultSquare[i] = max((float)resizedPixels[i*3] * opacity, (float)(screenTextureSquare[i] * 255.0f)); //take max value

							//resultSquare[i] = glm::mix(screenTextureSquare[i], opacity, (max(resizedPixels[i*3] - screenTextureSquare[i],0.0001f) / 255.0f)) * 255; //Mix



						//resultSquare[i] = glm::mix(screenTextureSquare[i], opacity, (max(resizedPixels[i] - screenTextureSquare[i],0.0001f) / 255.0f)) * 255; //Mix
						//resultSquare[i] = min(resizedPixels[i] + (int)(screenTextureSquare[i] * 255), 255); //sum up
					}
					//Avoid writing low value onto high value

					//Paint screen mask texture with resultSquare

					glset.activeTexture(GL_TEXTURE4);
					glTexSubImage2D(GL_TEXTURE_2D, 0, (mouseXpos/qualityDivider + screenGapX - distanceX / 2) ,( maxScreenHeight/qualityDivider - mouseYpos/qualityDivider - distanceY / 2), distanceX, distanceY, GL_RED, GL_UNSIGNED_BYTE, resultSquare);

					//Paint screen mask texture with resultSquare

					//finish
					delete[] screenTextureSquare;
					delete[] resultSquare;
					//finish
				}
				else{
					addToScreenMaskTxtr = true;
				}
				
			}
		
		glset.generateMipmap();

		if(mirrorUsed){
		//Update mirrored screen mask texture
		//setup
		glset.viewport(1920, 1080);
		glset.bindFramebuffer(FBOScreen);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		outShaderData.renderTextureProjection = projection;
		glset.useOutShader(programs.outProgram,outShaderData);

		glset.uniform1i(programs.outProgram, "isTwoDimensional", 0);
		glset.uniform1i(programs.outProgram, "isRenderScreenMaskMode", 1);
		//setup

		//Get texture
		render.renderTexture(renderVerticesFlipped,1080,1080,GL_TEXTURE3,GL_RED, model, false,albedoTextures);
		//Get texture

		//Finish
		glset.uniform1i(programs.outProgram, "isRenderScreenMaskMode", 0);
		//Finish
		//Update mirrored screen mask texture
		}
		glViewport(-(maxScreenWidth - screenSizeX)/2, -(maxScreenHeight - screenSizeY), maxScreenWidth, maxScreenHeight);

		glUseProgram(programs.uiProgram);
		glset.bindFramebuffer(0);
	}
	lastMouseXPosIn = mouseXposIn;
	lastMouseYPosIn = mouseYposIn;
	addToScreenMaskTxtr = true;
}