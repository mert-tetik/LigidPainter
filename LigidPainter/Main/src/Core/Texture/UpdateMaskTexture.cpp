#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Gl.h"
#include "Core/Texture/Texture.h"

#include "Core/TextureGenerator/TextureGenerator.h"

using namespace std;

GLubyte* Texture::updateMaskTexture(unsigned int FBOScreen,  int screenSize_x, int screenSize_y, float brushRotationRangeBarValue,bool renderTiny,float brushBorderRangeBarValue,float brushBlurVal,OutShaderData outShaderData, Programs programs, int maxScreenWidth, int maxScreenHeight) { //rotationValue = rotationBarValue
	GlSet glset;
	UserInterface ui;
	TextureGenerator txtrGen;

	glset.viewport(1080, 1080);

	float rotation = ((brushRotationRangeBarValue +0.11f) * 4.54545454545f) * 360.0f; // -0.11 - 0.11 --> 0 - 360

	float scaleVal = ((brushBorderRangeBarValue+0.11f)/2.0f * 8.18181818182f) + 0.3f;
	float scaleValTiny = scaleVal/4.0f;

	glset.useOutShader(programs.outProgram,outShaderData);

	//Rotate and scale
	glm::mat4 trans = glm::mat4(1.0f);
	if(!renderTiny){
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(scaleVal, scaleVal, scaleVal));
		glset.uniformMatrix4fv(programs.outProgram, "renderTrans", trans);
	}
	else{
		trans = glm::translate(trans, glm::vec3(-0.875f, -0.875f, 0.0f));
		trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(scaleValTiny, scaleValTiny, scaleValTiny));
		glset.uniformMatrix4fv(programs.outProgram, "renderTrans", trans);
	}
	//Rotate and scale

	int size;
	if(renderTiny)
		size = 135;
	else
		size = 540;
	
	//16:9 ---> 1:1 (makes easier to get vertices into the middle)
	glUseProgram(programs.outProgram);
	glm::mat4 renderTextureProjection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
	glset.uniformMatrix4fv(programs.outProgram, "renderTextureProjection", renderTextureProjection);
	//16:9 ---> 1:1

	glClearColor(0, 0, 0, 1.0f); //Back color while rendering mask texture

	std::vector<float> centerVertices = { 
	// first triangle
	 0.75f,  0.75f, 0.0f,1,1,0,0,0,  // top right
	 0.75f,  0.25f, 0.0f,1,0,0,0,0,  // bottom right
	 0.25f,  0.75f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.75f,  0.25f, 0.0f,1,0,0,0,0,  // bottom right
	 0.25f,  0.25f, 0.0f,0,0,0,0,0,  // bottom left
	 0.25f,  0.75f, 0.0f,0,1,0,0,0   // top left
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
	glset.drawArrays(centerVertices, false);
	GLubyte* rotatedMaskTxtr = new GLubyte[size * size * 3 ];
	glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, rotatedMaskTxtr);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(rotatedMaskTxtr, size, size, GL_RGB);
	glset.generateMipmap();
	//Rotation

	//Get back to previous projection after rendering rotated & scaled mask texture
	glset.uniform1i(programs.outProgram, "modifiedMaskTexture", 12);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glset.viewport(1920, 1080);
	trans = glm::mat4(1.0f);

	glset.uniformMatrix4fv(programs.outProgram, "renderTrans", trans);
	renderTextureProjection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);//1920 - 1080 -> 1.77777777778 - 1
	glset.uniformMatrix4fv(programs.outProgram, "renderTextureProjection", renderTextureProjection);
	//Get back to previous projection after rendering rotated & scaled mask texture


	glUseProgram(programs.blurProgram);

	BlurShaderData blurShaderData;

	blurShaderData.brushBlurVal = brushBlurVal;
	blurShaderData.inputTexture = 12; 
	blurShaderData.isRenderVerticalBlur = 0;
	blurShaderData.renderTextureProjection = renderTextureProjection; 




	glset.useBlurShader(programs.blurProgram,blurShaderData);

	

	std::vector<float> oneDimensionalGaussian = txtrGen.gaussianFunc(min(brushBlurVal,120.0f),0.0f);




	glset.uniform1i(programs.blurProgram , "kernel_sizeF" , oneDimensionalGaussian.size());


	for (int i = 0; i < oneDimensionalGaussian.size(); i++)
	{
		std::string target = "oneDimensionalGaussF1[" + std::to_string(i) + "]";
		glset.uniform1f(programs.blurProgram , target.c_str() , oneDimensionalGaussian[i]);
	}



	//Horizontal Blur
	glset.uniform1i(programs.blurProgram, "isRenderVerticalBlur", 0); 
	glset.uniform1i(programs.blurProgram, "inputTexture", 12); 

	if(renderTiny)
		glset.drawArrays(cornerVerticesQrtr, false);
	else
		glset.drawArrays(cornerVertices, false);

	GLubyte* horizontalBlurMaskTxtr = new GLubyte[size * size * 3];
	glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, horizontalBlurMaskTxtr);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(horizontalBlurMaskTxtr, size, size, GL_RGB);
	glset.generateMipmap();
	//Horizontal Blur

	//Vertical Blur setup
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	blurShaderData.isRenderVerticalBlur = 1;
	glset.useBlurShader(programs.blurProgram,blurShaderData);
	//Vertical Blur setup

	//Vertical blur
		if(renderTiny)
			glset.drawArrays(cornerVerticesQrtr, false);
		else
			glset.drawArrays(cornerVertices, false);

	GLubyte* verticalBlurMaskTxtr = new GLubyte[size * size * 3 ];
	glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, verticalBlurMaskTxtr);

	glset.activeTexture(GL_TEXTURE12);
	glset.texImage(verticalBlurMaskTxtr, size, size, GL_RGB);
	glset.generateMipmap();

	//Verical blur

	//Finish

	
	glUseProgram(programs.uiProgram);


	glset.bindFramebuffer(0);
	glViewport(-(maxScreenWidth - screenSize_x)/2, -(maxScreenHeight - screenSize_y), maxScreenWidth, maxScreenHeight);

	delete(rotatedMaskTxtr);
	delete(horizontalBlurMaskTxtr);
	return verticalBlurMaskTxtr;
}