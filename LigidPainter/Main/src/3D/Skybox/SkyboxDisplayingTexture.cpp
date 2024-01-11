/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "ShaderSystem/Shader.hpp"
#include "SettingsSystem/Settings.hpp"

void Skybox::createDisplayingTxtr(){
	
	// Generate the displaying texture
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

	// Create the capture framebuffer object
	Framebuffer FBO = Framebuffer(this->displayingTexture, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(100)));
	FBO.bind();

	//	
	glViewport(0,0,100,100);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderSystem::skyboxBall().use();
	glm::vec3 viewPos = glm::vec3(1.5f,0,0);
	
	glm::mat4 view = glm::lookAt(	
									viewPos, 
									glm::vec3(0), 
									glm::vec3(0.0, -1.0, 0.0)
								);
	
	glm::mat4 projectionMatrix = glm::perspective(
													glm::radians(90.f), 
													1.f, 
													100.f, 
													0.1f
												);


	ShaderSystem::skyboxBall().setVec3("viewPos",viewPos);
	ShaderSystem::skyboxBall().setMat4("view",view);
	ShaderSystem::skyboxBall().setMat4("projection",projectionMatrix);

	//ShaderSystem::skyboxBall().setInt("useTransformUniforms",0);

	glm::mat4 modelMatrix = glm::mat4(1);
	ShaderSystem::skyboxBall().setMat4("modelMatrix",modelMatrix);
	
	ShaderSystem::skyboxBall().setInt("skybox",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,ID);
	
	getSphereModel()->Draw();

	//Finish
	Settings::defaultFramebuffer()->FBO.bind();
	Settings::defaultFramebuffer()->setViewport();

	FBO.deleteBuffers(false, true);
}