/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"


void Skybox::createDisplayingTxtr(Shader skyboxBall,Model &sphereModel,glm::vec2 windowScale){
		//Update the material texture

	glActiveTexture(GL_TEXTURE0);
	if(displayingTexture == 0)
	glGenTextures(1,&displayingTexture);
	glBindTexture(GL_TEXTURE_2D,displayingTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	unsigned int FBO; //That framebuffer will be used to get the results of the shader (modifier)
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);
	
	glViewport(0,0,100,100);

	unsigned int RBO;
	glGenRenderbuffers(1,&RBO);
	glBindRenderbuffer(GL_RENDERBUFFER,RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 100, 100);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayingTexture, 0);

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skyboxBall.use();
	glm::mat4 view = glm::lookAt(glm::vec3(3.f,0,0), 
									glm::vec3(0), 
									glm::vec3(0.0, 1.0, 0.0));
	
	glm::vec3 viewPos = glm::vec3(3.f,0,0);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.f), 
										1.f, 
										100.f, 
										0.1f);


	skyboxBall.setVec3("viewPos",viewPos);
	skyboxBall.setMat4("view",view);
	skyboxBall.setMat4("projection",projectionMatrix);

	// skyboxBall.setInt("useTransformUniforms",0);

	glm::mat4 modelMatrix = glm::mat4(1);
	skyboxBall.setMat4("modelMatrix",modelMatrix);
	
	skyboxBall.setInt("skybox",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,ID);
	
	sphereModel.Draw();
	
	glGenerateMipmap(GL_TEXTURE_2D);
	

	//Finish
	//buttonShader.use();
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glDeleteFramebuffers(1,&FBO);

	glViewport(0,0,windowScale.x,windowScale.y);
}