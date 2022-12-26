#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include "tinyfiledialogs.h"

void Render::getDepthTexture(unsigned int FBOScreen,  int screenSizeX,  int screenSizeY, ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures,Programs programs,int currentMaterialIndex, int maxScreenWidth , int maxScreenHeight) {
	Texture txtr;
    GlSet gl;

	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	
	screenDepthShaderData.renderMirrored = 0;
	gl.useScreenDepthShader(programs.screenDepthProgram, screenDepthShaderData);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model.Draw(currentMaterialIndex,programs.PBRProgram,false,albedoTextures);

	GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE9);
	gl.texImage(screen, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]screen;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Mirrored
	screenDepthShaderData.renderMirrored = 1;
	gl.useScreenDepthShader(programs.screenDepthProgram, screenDepthShaderData);

	
	model.Draw(currentMaterialIndex,programs.PBRProgram,false,albedoTextures);

	GLubyte* screenMirrored = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE8);
	gl.texImage(screenMirrored, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[] screenMirrored;

	glUseProgram(programs.uiProgram);
	
	glViewport(-(maxScreenWidth - screenSizeX)/2, -(maxScreenHeight - screenSizeY), maxScreenWidth, maxScreenHeight);

	gl.bindFramebuffer(0);
}