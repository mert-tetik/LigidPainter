#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

void Render::getDepthTexture(unsigned int FBOScreen,  int screenSizeX,  int screenSizeY, ScreenDepthShaderData screenDepthShaderData,Model &model,bool renderDefault,std::vector<MaterialOut> &modelMaterials,Programs programs,int currentMaterialIndex, int maxScreenWidth , int maxScreenHeight,glm::mat4 view,std::vector<aTexture> albedoTextures,int chosenTextureIndex,std::vector<MirrorParam> &mirrorParams,unsigned int &depthTextureID,glm::vec3 cameraPos, glm::vec3 originPos,float xMirrorPos,float yMirrorPos,float zMirrorPos) {
	Texture txtr;
    GlSet gl;

	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	
	gl.useScreenDepthShader(programs.screenDepthProgram, screenDepthShaderData);
	glm::mat4 viewProjection;
	viewProjection = glm::lookAt(cameraPos, originPos, glm::vec3(0.0, 1.0, 0.0));
	gl.uniformMatrix4fv(programs.screenDepthProgram,"view",viewProjection);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	unsigned int chosenTxtr = 0;
	if(albedoTextures.size() != 0)
		chosenTxtr = albedoTextures[chosenTextureIndex].id;
	
	//TODO : Draw selected mesh
	model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0),0,0);

	GLubyte* screen = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
	gl.activeTexture(GL_TEXTURE9);
	gl.texImage(screen, 1920, 1080, GL_RGB);
	gl.generateMipmap();
	delete[]screen;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Mirrored
	for (size_t i = 0; i < mirrorParams.size(); i++)
	{
		std::cout << glm::to_string(mirrorParams[i].pos) << ' ';
		
		glm::mat4 mirroredView;
		mirroredView = glm::lookAt((cameraPos - glm::vec3(xMirrorPos*(mirrorParams[i].pos.x == -1),yMirrorPos*(mirrorParams[i].pos.y == -1),zMirrorPos *(mirrorParams[i].pos.z == -1))) * mirrorParams[i].pos, (originPos-glm::vec3(xMirrorPos*(mirrorParams[i].pos.x == -1),yMirrorPos*(mirrorParams[i].pos.y == -1),zMirrorPos*(mirrorParams[i].pos.z == -1))) * mirrorParams[i].pos, glm::vec3(0.0, 1.0, 0.0));
		
		gl.uniformMatrix4fv(programs.screenDepthProgram,"view",mirroredView);
	
		model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0),0,0);
		//TODO : Use framebuffers
		GLubyte* screenMirrored = txtr.getTextureFromProgram(GL_TEXTURE5, 1920, 1080, 3);
		gl.activeTexture(GL_TEXTURE28);

		gl.bindTexture(mirrorParams[i].depthTexture);
		gl.texImage(screenMirrored, 1920, 1080, GL_RGB);
		gl.generateMipmap();
		delete[] screenMirrored;
	}

	glUseProgram(programs.uiProgram);
	
	LigidPainter lp;
	lp.setViewportToDefault();

	gl.bindFramebuffer(0);
}