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
		 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
};


//Check if values changed
double lastMouseXPosIn = 0;
double lastMouseYPosIn = 0;

bool addToScreenMaskTxtr = true;

std::vector<glm::vec2> holdLocations;

bool refreshTheScreenMask = false;

unsigned int lastTxtr = 0;

std::vector<unsigned int> mirTxtr1;
std::vector<unsigned int> mirTxtr;


void TextureGenerator::drawToScreen(GLFWwindow*& window, unsigned int  screenPaintingTxtrId, float brushSize,unsigned int FBOScreen,float rotationValue, float opacityRangeBarValue, double lastMouseXPos, double lastMouseYPos, double mouseXpos, double mouseYpos, bool useNegativeForDrawing,bool brushValChanged,Programs& programs,int removeThisParam2,int removeThisParam,float brushBorderRangeBarValue,float brushBlurVal,unsigned int FBO,OutShaderData &outShaderData,Model &model,std::vector<MaterialOut> &modelMaterials,bool fillBetween,glm::mat4 view,std::vector<MirrorParam> &mirrorParams,glm::vec3 cameraPos,glm::vec3 originPos,float xMirrorPos,float yMirrorPos,float zMirrorPos,bool dynamicPainting,bool firstClick,float spacingRangeBarVal) {

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
	std::vector<float> screenVertices = { 
		// first triangle
		 (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width,  (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0.0f,1,1,0,0,0,  // top right
		 (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 0.0f,  (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 0.0f,0,1,0,0,0   // top left
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

	if ((lastMouseXPosIn - mouseXposIn != 0 || lastMouseYPosIn - mouseYposIn != 0) || firstClick) {
		
		glm::vec2 fstrokeLocation = glm::vec2(mouseXpos + screenGapX , mouseYpos);
		holdLocations.push_back(fstrokeLocation);
		
		//----------------------PAINTING----------------------\\

		int differenceBetweenMousePoints = glm::distance(glm::vec2(mouseXpos, mouseYpos), glm::vec2(lastMouseXPos, lastMouseYPos));
		if(spacingRangeBarVal > -0.1)
			differenceBetweenMousePoints = 1; 

		float xposDif = (mouseXpos - lastMouseXPos) / differenceBetweenMousePoints;
		float yposDif = (mouseYpos - lastMouseYPos) / differenceBetweenMousePoints;

			for (size_t i = 0; i < differenceBetweenMousePoints; i++)
			{
				////differenceBetweenMousePoints > 10
				if (true) {
					mouseXpos -= xposDif;
					mouseYpos -= yposDif;
				}

				if(true){
					glm::vec2 strokeLocation = glm::vec2(mouseXpos + screenGapX , mouseYpos);
					holdLocations.push_back(strokeLocation);
				}
				
				//else{
				//	addToScreenMaskTxtr = true;
				//}

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
		glUseProgram(programs.dynamicPaintingProgram);
		glset.uniformMatrix4fv(programs.dynamicPaintingProgram, "renderProjection" , paintingProjection);
		glset.uniform1i(programs.dynamicPaintingProgram, "posCount" , holdLocations.size());
		for (int i = 0; i < holdLocations.size(); i++)
		{
			std::string target = "positions[" + std::to_string(i) + "]";
			glUniform2f(glGetUniformLocation(programs.dynamicPaintingProgram , target.c_str()), holdLocations[i].x/(float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width , holdLocations[i].y/(float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
		}


		glActiveTexture(GL_TEXTURE4);
		glViewport(0,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height);
		glset.bindFramebuffer(FBO);

		if(!dynamicPainting){
			glUseProgram(programs.twoDPaintingProgram);
			glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);		
			glset.blendFunc( GL_SRC_ALPHA, GL_ONE);
			if(refreshTheScreenMask){
				glClear(GL_DEPTH_BUFFER_BIT);
				refreshTheScreenMask = true;
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
		}
		else{
			glDepthFunc(GL_ALWAYS);
			glset.drawArrays(screenVertices,false);
			glset.generateMipmap();
			glDepthFunc(GL_LESS);
		}

		int loopSize = 0;

		if(mirrorParams.size()){
			for (size_t i = 0; i < mirrorParams.size(); i++)
			{
				unsigned int FBOMr;
				glset.genFramebuffers(FBOMr);
				glset.bindFramebuffer(FBOMr);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirTxtr1[i], 0);
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT);
				glset.deleteFramebuffers(FBOMr);
			}
			for (size_t i = 0; i < mirrorParams.size(); i++)
			{
				unsigned int FBOMr;
				glset.genFramebuffers(FBOMr);
				glset.bindFramebuffer(FBOMr);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirTxtr[i], 0);
				glClearColor(0,0,0,0);
				glClear(GL_COLOR_BUFFER_BIT);
				glset.deleteFramebuffers(FBOMr);
			}
			
			for (size_t i = 0; i < mirrorParams.size(); i++)
			{
				glClearColor(0,0,0,0);
				//std::cout << "i : "<< i << '\n';
				//if(lastTxtr)
				//glDeleteTextures(1,&lastTxtr);

				//Update mirrored screen mask texture
				//setup
				glActiveTexture(GL_TEXTURE3);
				unsigned int FBOMr;
				glset.genFramebuffers(FBOMr);
				glset.bindFramebuffer(FBOMr);
				glset.bindTexture(mirTxtr1[i]);
				glset.texImage(NULL, 1080,1080,GL_RGBA);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirTxtr1[i], 0);
				glset.generateMipmap();

				glset.viewport(1080, 1080);

				glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
				outShaderData.renderTextureProjection = projection;
				glset.useOutShader(programs.outProgram,outShaderData);

				glm::vec3 mirroredScreenMaskCamPos = cameraPos - originPos;
				glset.uniform3fv(programs.outProgram, "mirroredScreenMaskCamPos", cameraPos);
				glset.uniform1i(programs.outProgram, "isTwoDimensional", 0);
				glset.uniform1i(programs.outProgram, "isRenderScreenMaskMode", 1);
				glset.uniform1i(programs.outProgram, "mirrormaskrenderingX", mirrorParams[i].pos.x*-1);
				glset.uniform1i(programs.outProgram, "mirrormaskrenderingY", mirrorParams[i].pos.y*-1);
				glset.uniform1i(programs.outProgram, "mirrormaskrenderingZ", mirrorParams[i].pos.z*-1);
				//setup

				//Get texture
				glset.drawArrays(textureRenderingVerticesFlipped,0);
				//Get texture
				glset.uniform1i(programs.outProgram, "screenMaskTexture", 3);
				
				glDeleteFramebuffers(1,&FBOMr);


				//Finish
				//Finish
				//Update mirrored screen mask texture

				glset.uniform1i(programs.outProgram, "tdMaskRendering", 1);
				glset.uniform1i(programs.outProgram, "previous3DMaskTxtrs", 29);
				glActiveTexture(GL_TEXTURE9);
				glset.bindTexture(mirrorParams[i].depthTexture);
				glActiveTexture(GL_TEXTURE8);
				unsigned int FBOMr2;
				glset.genFramebuffers(FBOMr2);
				glset.bindFramebuffer(FBOMr2);
				glset.bindTexture(mirTxtr[i]);
				glset.texImage(NULL, 1080,1080,GL_RGBA);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirTxtr[i], 0);
				
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
				glActiveTexture(GL_TEXTURE29);
				
				if(i != 0){
					glset.bindTexture(mirTxtr[i-1]);
					glset.uniform1i(programs.outProgram, "usePrevious3DMaskTxtrs", 1);
				}
				else{
					glset.uniform1i(programs.outProgram, "usePrevious3DMaskTxtrs", 0);
				}

				glm::mat4 mirroredView;
				mirroredView = glm::lookAt((cameraPos - glm::vec3(xMirrorPos*(mirrorParams[i].pos.x == -1),yMirrorPos*(mirrorParams[i].pos.y == -1),zMirrorPos *(mirrorParams[i].pos.z == -1))) * mirrorParams[i].pos, (originPos-glm::vec3(xMirrorPos*(mirrorParams[i].pos.x == -1),yMirrorPos*(mirrorParams[i].pos.y == -1),zMirrorPos*(mirrorParams[i].pos.z == -1))) * mirrorParams[i].pos, glm::vec3(0.0, 1.0, 0.0));
		
				glset.uniformMatrix4fv(programs.outProgram,"view",mirroredView);

				std::vector<MaterialOut> emptyMaterials = {};
				model.Draw(0,programs.PBRProgram,false,modelMaterials,view,true,0,glm::vec3(0),0,0,true,{},0,glm::mat4(0));

				glset.uniform1i(programs.outProgram, "tdMaskRendering", 0);
				
				glset.uniform1i(programs.outProgram, "screenMaskTexture", 4);

				glDeleteFramebuffers(1,&FBOMr2);
			 	//glDeleteTextures(1,&textureColorbuffer);
			}
			glset.uniform1i(programs.outProgram, "isRenderScreenMaskMode", 0);
		}
	}


	glset.uniformMatrix4fv(programs.outProgram,"view",view);

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

void TextureGenerator::initDrawToScreen(){
	for (size_t i = 0; i < 7; i++)
	{
		unsigned int txtr;
		GlSet gl;
		gl.genTextures(txtr);
		mirTxtr.push_back(txtr);
	}
	for (size_t i = 0; i < 7; i++)
	{
		unsigned int txtr;
		GlSet gl;
		gl.genTextures(txtr);
		mirTxtr1.push_back(txtr);
	}
}