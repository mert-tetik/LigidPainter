#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Model/Model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/Gl.h"
#include "Core/Texture/Texture.h"

void Render::getDepthTexture(unsigned int FBOScreen,  int screenSizeX,  int screenSizeY, ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,std::vector<MaterialOut> &modelMaterials,Programs programs,int currentMaterialIndex, int maxScreenWidth , int maxScreenHeight,glm::mat4 view,std::vector<aTexture> albedoTextures,int chosenTextureIndex) {
	Texture txtr;
    GlSet gl;

	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	
	screenDepthShaderData.renderMirrored = 0;
	gl.useScreenDepthShader(programs.screenDepthProgram, screenDepthShaderData);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	unsigned int chosenTxtr = 0;
	if(albedoTextures.size() != 0)
		chosenTxtr = albedoTextures[chosenTextureIndex].id;

	model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0));

	GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE9);
	gl.texImage(screen, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]screen;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Mirrored
	screenDepthShaderData.renderMirrored = 1;
	gl.useScreenDepthShader(programs.screenDepthProgram, screenDepthShaderData);

	
	model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0));

	GLubyte* screenMirrored = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE8);
	gl.texImage(screenMirrored, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[] screenMirrored;

	glUseProgram(programs.uiProgram);
	
	glViewport(-(maxScreenWidth - screenSizeX)/2, -(maxScreenHeight - screenSizeY), maxScreenWidth, maxScreenHeight);

	gl.bindFramebuffer(0);
}