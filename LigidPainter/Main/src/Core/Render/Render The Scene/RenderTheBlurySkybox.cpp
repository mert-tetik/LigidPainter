#include<iostream>

#include <vector>

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/gl.h"

void Render::renderBlurySkybox(Cubemaps &cubemaps,SkyBoxShaderData &skyBoxShaderData,Programs & renderPrograms,std::vector<UIElement> UIElements, float &createProjectPanelBlurVal,float w,float h,float posX,float posY,bool roundCorners){
        GlSet gls;
        gls.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE13);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.cubemap);
		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_CUBE_MAP,cubemaps.prefiltered);
		renderSkyBox(skyBoxShaderData,renderPrograms,UIElements[UIskyBoxExposureRangeBar].rangeBar.value,UIElements[UIskyBoxRotationRangeBar].rangeBar.value);

		//TODO Delete that framebuffer
		unsigned int SBFBO;
		gls.genFramebuffers(SBFBO);
		gls.bindFramebuffer(SBFBO);

		//Texture
		//TODO Delete that texture
		unsigned int textureColorbuffer;
		gls.genTextures(textureColorbuffer);
		gls.bindTexture(textureColorbuffer);
		glViewport(0,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width/20,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/20);
		gls.texImage(NULL, glfwGetVideoMode(glfwGetPrimaryMonitor())->width/20,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/20,GL_RGBA); //TODO : Use texture quality variable
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		gls.generateMipmap();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		
		renderSkyBox(skyBoxShaderData,renderPrograms,UIElements[UIskyBoxExposureRangeBar].rangeBar.value,UIElements[UIskyBoxRotationRangeBar].rangeBar.value);

		glUseProgram(renderPrograms.renderTheTextureBlur);
		gls.uniform1i(renderPrograms.renderTheTextureBlur, "roundCorners" ,createProjectPanelBlurVal*100.f+5);
		gls.uniform1i(renderPrograms.renderTheTextureBlur, "blurVal" ,createProjectPanelBlurVal*100.f+5);
		gls.uniform1i(renderPrograms.renderTheTextureBlur, "txtr" ,14);

		glActiveTexture(GL_TEXTURE14);
		gls.bindTexture(textureColorbuffer);

		float textureWidth = w;
		float textureHeight = h;
		float position_x = posX;
		float position_y = posY;
		float panelZ = 0.5f;

		std::vector<float> buttonCoorSq{
			// first triangle
			 textureWidth + position_x,  textureHeight + position_y, 	panelZ+0.02f,	1,		1		,0,0,0,  // top right
			 textureWidth + position_x,  -textureHeight + position_y, 	panelZ+0.02f,	1,		0		,0,0,0,  // bottom right
			-textureWidth + position_x,  textureHeight + position_y, 	panelZ+0.02f,	0,		1		,0,0,0,  // top left 
			// second triangle						   	
			 textureWidth + position_x,  -textureHeight + position_y, 	panelZ+0.02f,	1,		0		,0,0,0,  // bottom right
			-textureWidth + position_x,  -textureHeight + position_y, 	panelZ+0.02f,	0,		0		,0,0,0,  // bottom left
			-textureWidth + position_x,  textureHeight + position_y, 	panelZ+0.02f,	0,		1		,0,0,0  // top left
		};
		
		gls.bindFramebuffer(0);
		LigidPainter lp;
		lp.setViewportToDefault();

		gls.drawArrays(buttonCoorSq,false);
} 