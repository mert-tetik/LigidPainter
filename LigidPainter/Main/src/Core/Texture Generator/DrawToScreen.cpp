#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/Render/Render.h"
#include "Core/Texture Generator/TextureGenerator.h"

#include "stb_image_resize.h"
#include "stb_image.h"
#include <vector>

using namespace std;

std::vector<float> textureRenderingVerticesFlipped = {
	// first triangle
	 1.0f,  1.0f, 0.0f,0,1,0,0,0,  // top right
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom right
	 0.0f,  1.0f, 0.0f,1,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom left
	 0.0f,  1.0f, 0.0f,1,1,0,0,0   // top left
};


//Check if values changed
double lastMouseXPosIn = 0;
double lastMouseYPosIn = 0;

bool addToScreenMaskTxtr = true;

std::vector<glm::vec2> holdLocations;

bool refreshTheScreenMask = false;

unsigned int lastTxtr = 0;

void TextureGenerator::drawToScreen(GLFWwindow*& window, unsigned int  screenPaintingTxtrId, float brushSize,unsigned int FBOScreen,float rotationValue, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos, bool mirrorUsed, bool useNegativeForDrawing,bool brushValChanged,Programs& programs,int removeThisParam2,int removeThisParam,float brushBorderRangeBarValue,float brushBlurVal,unsigned int FBO,OutShaderData &outShaderData,Model &model,std::vector<MaterialOut> &modelMaterials,bool fillBetween,glm::mat4 view) {

	if(true){
		holdLocations.clear();
	}


	Texture texture;
	Render render;

	double mouseXposIn;
	double mouseYposIn;
	glfwGetCursorPos(window, &mouseXposIn, &mouseYposIn);

	int screenSizeX;
	int screenSizeY;
	glfwGetFramebufferSize(window,&screenSizeX,&screenSizeY);
	//Get context data


	//Get brush size
	int distanceX = brushSize;
	int distanceY = distanceX;


	float opacity = ((opacityRangeBarValue + 0.11f) * 4.54545454545f); //-0.11 - 0.11 --> 0 - 1

	glm::vec3 drawColor = glm::vec3(opacity);

	std::vector<float> paintingMaskCoords = {
	// first triangle									//Normals will be used for colors
	 	-distanceX/2.0f,  -distanceX/2.0f, 0.0f	,0,1,	drawColor.r,drawColor.g,drawColor.b,  // top right
	 	-distanceX/2.0f,   distanceX/2.0f, 0.0f	,0,0,	drawColor.r,drawColor.g,drawColor.b,  // bottom right
	     distanceX/2.0f,  -distanceX/2.0f, 0.0f	,1,1,	drawColor.r,drawColor.g,drawColor.b,  // top left
	// second triangle
	 	-distanceX/2.0f,   distanceX/2.0f, 0.0f	,0,0,	drawColor.r,drawColor.g,drawColor.b,  // bottom right
	     distanceX/2.0f,   distanceX/2.0f, 0.0f	,1,0,	drawColor.r,drawColor.g,drawColor.b,  // bottom left
	     distanceX/2.0f,  -distanceX/2.0f, 0.0f	,1,1,	drawColor.r,drawColor.g,drawColor.b   // top left
	};

	glm::mat4 paintingProjection;
	paintingProjection = glm::ortho(0.0f,(float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width,(float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height,0.0f);


	float screenGapX = ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width - (float)screenSizeX)/2.0f;

	float screenGapY = ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height - (float)screenSizeY);

	GlSet glset;
	Texture txtr;

	//Get brush size

	//----------------------SET BRUSH TEXTURE----------------------\\
						  (Interpreted with blur value)

	if (lastMouseXPosIn - mouseXposIn != 0 || lastMouseYPosIn - mouseYposIn != 0) {

		//----------------------PAINTING----------------------\\


		int differenceBetweenMousePoints = glm::distance(glm::vec2(mouseXpos, mouseYpos), glm::vec2(lastMouseXPos, lastMouseYPos));

		float xposDif = (mouseXpos - lastMouseXPos) / differenceBetweenMousePoints;
		float yposDif = (mouseYpos - lastMouseYPos) / differenceBetweenMousePoints;

		if (differenceBetweenMousePoints <= 10 || !fillBetween) {
			differenceBetweenMousePoints = 1;
		}
			for (size_t i = 0; i < differenceBetweenMousePoints; i++)
			{
				if (differenceBetweenMousePoints > 10) {
					mouseXpos -= xposDif;
					mouseYpos -= yposDif;
				}

				if(true){
					glm::vec2 strokeLocation = glm::vec2(mouseXpos + screenGapX , mouseYpos);
					holdLocations.push_back(strokeLocation);
				}
				else{
					addToScreenMaskTxtr = true;
				}

			}

		glUseProgram(programs.twoDPaintingProgram);
		glset.uniform1i(programs.twoDPaintingProgram, "invert" , useNegativeForDrawing);
		glset.uniform1i(programs.twoDPaintingProgram, "modifiedMaskTexture" , 12);
		glset.uniformMatrix4fv(programs.twoDPaintingProgram, "renderProjection" , paintingProjection);
		for (int i = 0; i < holdLocations.size(); i++)
		{
			float z = 0.5f;

			std::string target = "transform[" + std::to_string(i) + "]";
			glset.uniform3f(programs.twoDPaintingProgram , target.c_str() , holdLocations[i].x , holdLocations[i].y , z);
		}


		glActiveTexture(GL_TEXTURE4);

		glViewport(0,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);

		// glset.blendFunc(GL_ONE_MINUS_SRC_COLOR, GL_DST_ALPHA);
		glBlendEquationSeparate(GL_MAX,GL_FUNC_REVERSE_SUBTRACT);
		
		glset.blendFunc( GL_SRC_ALPHA, GL_ONE);

		glset.bindFramebuffer(FBO);

		if(refreshTheScreenMask){
			glClear(GL_DEPTH_BUFFER_BIT);
			refreshTheScreenMask = false;
		}
		
		glBufferSubData(GL_ARRAY_BUFFER , 0 , paintingMaskCoords.size() * sizeof(float) , &paintingMaskCoords[0]);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_ALWAYS);

		glDrawArraysInstanced(GL_TRIANGLES , 0 , 6 , holdLocations.size());

		glset.activeTexture(GL_TEXTURE4);
		glset.generateMipmap();
		glset.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glDepthFunc(GL_LESS);

		glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);


		if(mirrorUsed){
			if(lastTxtr)
				glDeleteTextures(1,&lastTxtr);

			//Update mirrored screen mask texture
			//setup
			glActiveTexture(GL_TEXTURE3);
			unsigned int FBOMr;
			glset.genFramebuffers(FBOMr);
			glset.bindFramebuffer(FBOMr);
			unsigned int textureColorbuffer;
			glset.genTextures(textureColorbuffer);
			glset.bindTexture(textureColorbuffer);
			glset.texImage(NULL, 1080,1080,GL_RGBA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

			lastTxtr = textureColorbuffer;

			glset.viewport(1080, 1080);

			glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
			outShaderData.renderTextureProjection = projection;
			glset.useOutShader(programs.outProgram,outShaderData);

			glset.uniform1i(programs.outProgram, "isTwoDimensional", 0);
			glset.uniform1i(programs.outProgram, "isRenderScreenMaskMode", 1);
			//setup

			//Get texture
			glset.drawArrays(textureRenderingVerticesFlipped,0);
			//Get texture

			//Finish
			glset.uniform1i(programs.outProgram, "isRenderScreenMaskMode", 0);
			//Finish
			//Update mirrored screen mask texture
			glDeleteFramebuffers(1,&FBOMr);
		}

			glUseProgram(programs.uiProgram);
	}


	LigidPainter lp;
	lp.setViewportToDefault();
	glset.bindFramebuffer(0);
	
	lastMouseXPosIn = mouseXposIn;
	lastMouseYPosIn = mouseYposIn;
	addToScreenMaskTxtr = true;
}

void Texture::refreshScreenTxtr(){
	refreshTheScreenMask = true;
}