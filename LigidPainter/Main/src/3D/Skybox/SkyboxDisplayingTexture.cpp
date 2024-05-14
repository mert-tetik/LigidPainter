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
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"

#include "3D/Skybox/Skybox.hpp"
#include "3D/Model/Model.hpp"

#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"

void Skybox::createDisplayingTxtr(){
	if(!displayingTexture.ID){
		displayingTexture = Texture((char*)nullptr, 100, 100);
	}

	// Create the capture framebuffer object
	Framebuffer FBO = FBOPOOL::requestFBO_with_RBO(this->displayingTexture, this->displayingTexture.getResolution(), "Skybox displaying texture");

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	ShaderSystem::skyboxBall().use();
	ShaderSystem::skyboxBall().setVec3("viewPos",viewPos);
	ShaderSystem::skyboxBall().setMat4("view",view);
	ShaderSystem::skyboxBall().setMat4("projection",projectionMatrix);
	ShaderSystem::skyboxBall().setMat4("modelMatrix",glm::mat4(1));
	ShaderSystem::skyboxBall().setInt("skybox", GL::bindTexture_CubeMap(this->ID, "Skybox::createDisplayingTxtr"));
	
	getSphereModel()->Draw();

	//Finish
	GL::releaseBoundTextures("Skybox::createDisplayingTxtr");
	FBOPOOL::releaseFBO(FBO);
	ShaderUTIL::release_bound_shader();
}