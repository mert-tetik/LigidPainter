#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

int lastTxtrRes = 1024;
void Render::renderTextures(unsigned int FBOScreen, int screenSizeX,  int screenSizeY, PBRShaderData pbrShaderData,Model &model,bool renderDefault,std::vector<aTexture> &albedoTextures,bool paintOut,bool isRenderTexture,bool paintRender,
							int currentMaterialIndex,Programs programs, int maxScreenWidth , int maxScreenHeight,std::vector<MaterialOut> &modelMaterials,glm::mat4 view,int chosenTextureIndex,int chosenTextureResIndex,bool useUV) {
	int maxHistoryHold = 10;

	int txtrRes = 256;
	for (size_t i = 0; i < chosenTextureResIndex; i++)
	{
		txtrRes*=2;
	}
	bool txtrResChanged = false;
	if(txtrRes != lastTxtrRes)
		txtrResChanged = true;

	albedoTextures[chosenTextureIndex].changed = true;

	if(isRenderTexture){
		glEnable(GL_DEPTH_TEST); 
		glDepthFunc(GL_LESS);
    	GlSet gl;
		
		glActiveTexture(GL_TEXTURE28);
		unsigned int FBO;
		gl.genFramebuffers(FBO);
		gl.bindFramebuffer(FBO);

		unsigned int textureColorbuffer;
		gl.genTextures(textureColorbuffer);
		gl.bindTexture(textureColorbuffer);
		gl.texImage(NULL, txtrRes,txtrRes,GL_RGBA);
		gl.generateMipmap();
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

		glViewport(0,0,txtrRes,txtrRes);


		glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
		gl.usePBRShader(programs.PBRProgram,pbrShaderData);
		gl.uniform1i(programs.PBRProgram,"getTheResultF",1);
		gl.uniform1i(programs.PBRProgram,"getTheResultV",1);
		gl.uniformMatrix4fv(programs.PBRProgram,"orthoProjection",projection);

		model.Draw();


		//Finish
		gl.uniform1i(programs.PBRProgram,"getTheResultF",0);
		gl.uniform1i(programs.PBRProgram,"getTheResultV",0);
		
		gl.bindFramebuffer(0);
		LigidPainter lp;
		lp.setViewportToDefault();

		//Finish
		glUseProgram(programs.uiProgram);

		glDeleteTextures(1,&albedoTextures[chosenTextureIndex].id);
		albedoTextures[chosenTextureIndex].id = textureColorbuffer;
		glActiveTexture(GL_TEXTURE0);
		gl.bindTexture(albedoTextures[chosenTextureIndex].id);


		glDeleteFramebuffers(1,&FBO);

		glActiveTexture(GL_TEXTURE28);

		Texture txtr;
		txtr.refreshScreenDrawingTexture();
	}
	else if(paintRender){
		GlSet gl;
		UserInterface ui;
		Texture txtr;
		
		glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		//outShaderData.renderTextureProjection = projection;
		//gl.useOutShader(programs.outProgram, outShaderData);

		gl.uniform1i(programs.outProgram, "isTwoDimensional", 0);
		gl.viewport(1920, 1080);
		gl.bindFramebuffer(txtrRes);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl.uniform1i(programs.outProgram,"renderPaintedTxtrMask",0);
		//Render painted image
			
		unsigned int chosenTxtr = 0;
		if(albedoTextures.size() != 0)
			chosenTxtr = albedoTextures[chosenTextureIndex].id;
		model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0),0,0,true,{},0,glm::mat4(0),useUV,0);

		GLubyte* renderedImage = new GLubyte[1080 * 1080 * 4 * sizeof(GLubyte)];
		glReadPixels(0, 0, 1080, 1080, GL_RGBA, GL_UNSIGNED_BYTE, renderedImage);
		
		gl.activeTexture(GL_TEXTURE0);
		gl.texImage(renderedImage, 1080, 1080, GL_RGBA);
		gl.generateMipmap();
		
		delete[]renderedImage;
		gl.bindFramebuffer(0);
		txtr.refreshScreenDrawingTexture();

		LigidPainter lp;
		lp.setViewportToDefault();
	}
}