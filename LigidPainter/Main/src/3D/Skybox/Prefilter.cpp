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
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"


unsigned int Skybox::createPrefilterMap(Shader prefilteringShader,glm::vec2 videoScale){
	//Creates the prefildered cubemap for the skybox
	//Which is used for PBR

	//Is required for rendering the skybox
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	//Skybox will be rendered to that FBO
	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	//For rendering every side of the skybox
	glm::mat4 captureViews[] = 
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};	


		//Create the cube map texture
		glActiveTexture(GL_TEXTURE28);
		if(!IDPrefiltered)
			glGenTextures(1, &IDPrefiltered);
		glBindTexture(GL_TEXTURE_CUBE_MAP, IDPrefiltered);
		for (unsigned int txtrI = 0; txtrI < 6; ++txtrI)
		{
			//Every side
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + txtrI, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		//Texture Parameters
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//Generate mipmap after creating the txtr
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


		//Use the related program for rendering
		prefilteringShader.use();
		
		prefilteringShader.setInt("environmentMap",0);
		prefilteringShader.setMat4("projection",captureProjection);
		prefilteringShader.setMat4("transformMatrix",glm::mat4(1));

		//Bind the skybox to the Slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

		//Bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		//5 levels will be created
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			//Every level

			//resize framebuffer according to mip-level size.
			unsigned int mipWidth  = 1024 * std::pow(0.5, mip);
			unsigned int mipHeight = 1024 * std::pow(0.5, mip);
			glViewport(0, 0, mipWidth+5, mipHeight+5);

			//Adjust the roughness value
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilteringShader.setFloat("roughness",roughness);


			//Create the texture
			for (unsigned int txtrI = 0; txtrI < 6; ++txtrI)
			{
				//Every side
				prefilteringShader.setMat4("view",captureViews[txtrI]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
										GL_TEXTURE_CUBE_MAP_POSITIVE_X + txtrI, IDPrefiltered, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				draw(false); //Draw the skybox
			}
		}
	
	//Set everything to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	
	//!Set viewport the default was there

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  

	//Bind the result cube map to 16th slote
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_CUBE_MAP, IDPrefiltered);

	//Delete the framebuffer
	glDeleteFramebuffers(1,&captureFBO);
	glDeleteRenderbuffers(1,&captureRBO);

	return IDPrefiltered;
}