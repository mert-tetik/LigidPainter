#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "Core/Texture Generator/TextureGenerator.h"

using namespace std;

void Texture::updateMaskTexture(unsigned int FBOScreen,  int screenSize_x, int screenSize_y, float brushRotationRangeBarValue,bool renderTiny,float brushBorderRangeBarValue,float brushBlurVal,OutShaderData outShaderData, Programs programs, int removeThisParam, int removeThisParam2) { //rotationValue = rotationBarValue
	GlSet glset;
	UserInterface ui;
	TextureGenerator txtrGen;


	float rotation = ((brushRotationRangeBarValue +0.11f) * 4.54545454545f) * 460.0f; // -0.11 - 0.11 --> 0 - 360

	float scaleVal = ((brushBorderRangeBarValue+0.11f)/2.0f * 8.18181818182f) + 0.3f;
	float scaleValTiny = scaleVal/4.0f;

	glset.useOutShader(programs.outProgram,outShaderData);

	//Rotate and scale
	glm::mat4 trans = glm::mat4(1.0f);
	if(!renderTiny){
		//trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		//trans = glm::scale(trans, glm::vec3(scaleVal, scaleVal, scaleVal));
		glset.uniformMatrix4fv(programs.outProgram, "renderTrans", trans);
	}
	else{
		//trans = glm::translate(trans, glm::vec3(-0.875f, -0.875f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		//trans = glm::scale(trans, glm::vec3(scaleValTiny, scaleValTiny, scaleValTiny));
		glset.uniformMatrix4fv(programs.outProgram, "renderTrans", trans);
	}
	//Rotate and scale

	int size;
	if(renderTiny)
		size = 135;
	else
		size = 540;
	
	//16:9 ---> 1:1 
	glUseProgram(programs.outProgram);
	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
	glset.uniformMatrix4fv(programs.outProgram, "renderTextureProjection", renderTextureProjection);
	//16:9 ---> 1:1

	glClearColor(0, 0, 0, 0); //Back color while rendering mask texture

	std::vector<float> centerVertices = { 
	// first triangle
	 1.f,  1.f, 0.0f,1,1,0,0,0,  // top right
	 1.f,  0.f, 0.0f,1,0,0,0,0,  // bottom right
	 0.f,  1.f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 1.f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};

	std::vector<float> cornerVertices = { 
	// first triangle
	 0.5f,  0.5f, 0.0f,1,1,0,0,0,  // top right
	 0.5f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.5f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.5f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.0f,  0.5f, 0.0f,0,1,0,0,0   // top left
	};
	std::vector<float> cornerVerticesQrtr = { 
	// first triangle
	 0.125f,  0.125f, 0.0f,1,1,0,0,0,  // top right
	 0.125f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.125f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.125f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 0.0f,  0.125f, 0.0f,0,1,0,0,0   // top left
	};
	
	//Setup
	glset.uniform1i(programs.outProgram, "isTwoDimensional", 1);
	glset.uniform1i(programs.outProgram, "renderMaskBrush", 1);
	glset.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.uniform1i(programs.outProgram, "modifiedMaskTexture", 1);
	//Setup

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_PACK_ALIGNMENT,1);

	//Rotation
	unsigned int rotationFBO;
	glset.genFramebuffers(rotationFBO);
	glset.bindFramebuffer(rotationFBO);
	
	glset.activeTexture(GL_TEXTURE12);
	unsigned int rotationColorBuffer;
	glset.genTextures(rotationColorBuffer);
	glset.bindTexture(rotationColorBuffer);
	glset.texImage(nullptr,size,size,GL_RGBA);
	glset.generateMipmap();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rotationColorBuffer, 0);
	glset.viewport(size,size);
	glset.drawArrays(centerVertices, false);

//	glReadPixels(0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, &rotatedMaskTxtr[0]);



	//Rotation

	//Get back to previous projection after rendering rotated & scaled mask texture
	glset.uniform1i(programs.outProgram, "modifiedMaskTexture", 12);
	trans = glm::mat4(1.0f);

	glset.uniformMatrix4fv(programs.outProgram, "renderTrans", trans);
	renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
	glset.uniformMatrix4fv(programs.outProgram, "renderTextureProjection", renderTextureProjection);
	//Get back to previous projection after rendering rotated & scaled mask texture


	glUseProgram(programs.blurProgram);

	BlurShaderData blurShaderData;

	blurShaderData.brushBlurVal = brushBlurVal;
	blurShaderData.inputTexture = 12; 
	blurShaderData.isRenderVerticalBlur = 0;
	blurShaderData.renderTextureProjection = renderTextureProjection; 




	glset.useBlurShader(programs.blurProgram,blurShaderData);

	

	std::vector<float> oneDimensionalGaussian = txtrGen.gaussianFunc(min(brushBlurVal+1.f,120.0f),0.0f);




	glset.uniform1i(programs.blurProgram , "kernel_sizeF" , oneDimensionalGaussian.size());


	for (int i = 0; i < oneDimensionalGaussian.size(); i++)
	{
		std::string target = "oneDimensionalGaussF1[" + std::to_string(i) + "]";
		glset.uniform1f(programs.blurProgram , target.c_str() , oneDimensionalGaussian[i]);
	}



	//Horizontal Blur
	glset.uniform1i(programs.blurProgram, "isRenderVerticalBlur", 0); 
	glset.uniform1i(programs.blurProgram, "inputTexture", 12); 

	unsigned int hBlurFBO;
	glset.genFramebuffers(hBlurFBO);
	glset.bindFramebuffer(hBlurFBO);
	
	glset.activeTexture(GL_TEXTURE28);
	unsigned int hBlurColorBuffer;
	glset.genTextures(hBlurColorBuffer);
	glset.bindTexture(hBlurColorBuffer);
	glset.texImage(nullptr,size,size,GL_RGBA);
	glset.generateMipmap();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hBlurColorBuffer, 0);

	glset.drawArrays(centerVertices, false);

	glset.activeTexture(GL_TEXTURE12);
	glset.bindTexture(hBlurColorBuffer);



	glset.bindFramebuffer(FBOScreen);



	//Vertical Blur setup
	
	blurShaderData.isRenderVerticalBlur = 1;
	glset.useBlurShader(programs.blurProgram,blurShaderData);
	//Vertical Blur setup

	//Vertical blur
		unsigned int vBlurFBO;
	glset.genFramebuffers(vBlurFBO);
	glset.bindFramebuffer(vBlurFBO);
	
	glset.activeTexture(GL_TEXTURE28);
	unsigned int vBlurColorBuffer;
	glset.genTextures(vBlurColorBuffer);
	glset.bindTexture(vBlurColorBuffer);
	glset.texImage(nullptr,size,size,GL_RGBA);
	glset.generateMipmap();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vBlurColorBuffer, 0);

	glset.drawArrays(centerVertices, false);

	glset.activeTexture(GL_TEXTURE12);
	glset.bindTexture(vBlurColorBuffer);


	glset.generateMipmap();

	//Verical blur

	//Finish

	
	glUseProgram(programs.uiProgram);


	glset.bindFramebuffer(0);
	LigidPainter lp;
	lp.setViewportToDefault();

	// return verticalBlurMaskTxtr;
}