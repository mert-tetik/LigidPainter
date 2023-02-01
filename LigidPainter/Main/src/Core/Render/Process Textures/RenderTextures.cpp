#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/Model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Gl.h"
#include "Core/Texture/Texture.h"

void Render::renderTextures(unsigned int FBOScreen, int screenSizeX,  int screenSizeY, OutShaderData outShaderData,Model &model,bool renderDefault,std::vector<aTexture> &albedoTextures,bool paintOut,bool isRenderTexture,bool paintRender,bool firstPaint,int currentMaterialIndex,Programs programs, int maxScreenWidth , int maxScreenHeight,std::vector<MaterialOut> &modelMaterials,glm::mat4 view,int chosenTextureIndex) {
	int maxHistoryHold = 20;

	if(isRenderTexture){
		
		//UNDO
		Texture txtr;
		if(albedoTextures.size() > 0){
			GLubyte* originalImage = txtr.getTextureFromProgram(GL_TEXTURE0, 1080, 1080, 3);

			GlSet glset;
			glActiveTexture(GL_TEXTURE28);
			unsigned int orgTexture;
			glset.genTextures(orgTexture);
			glset.bindTexture(orgTexture);
			glset.texImage(originalImage,1080,1080,GL_RGB);
			glset.generateMipmap();

			albedoTextures[chosenTextureIndex].undoList.push_back(orgTexture);

    		//Delete the first element from undoList if undoList's count is greated than max history holding value which is 20
			if (albedoTextures[chosenTextureIndex].undoList.size() > maxHistoryHold){
				//Delete the texture
				unsigned int undoTexture = albedoTextures[chosenTextureIndex].undoList[0];
				glDeleteTextures(0,&undoTexture);

				//Remove the element
				albedoTextures[chosenTextureIndex].undoList.erase(albedoTextures[chosenTextureIndex].undoList.begin());
			}
		}

		std::vector<float> renderVertices = { //Render backside of the uv
		// first triangle
		 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
		// second triangle	  ,0,0,0,
		 1.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
		 0.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
		 0.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
		};

    	GlSet gl;
		//Setup
		glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		outShaderData.renderTextureProjection = projection;
		gl.useOutShader(programs.outProgram, outShaderData);


		gl.uniform1i(programs.outProgram, "isTwoDimensional", 0);
		gl.viewport(1920, 1080);
		gl.bindFramebuffer(FBOScreen);
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Setup


		UserInterface ui;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int chosenTxtr = 0;
		if(albedoTextures.size() != 0)
			chosenTxtr = albedoTextures[chosenTextureIndex].id;

		//Render painted image
		model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0));

		// if (!paintOut)
		// 	gl.drawArrays(renderVertices, false);

		GLubyte* renderedImage = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
		glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, renderedImage);
		gl.activeTexture(GL_TEXTURE0);
		gl.texImage(renderedImage, 1080, 1080, GL_RGB);
		gl.generateMipmap();
		delete[]renderedImage;
		//Render painted image

		txtr.refreshScreenDrawingTexture();

		//Render uv mask
		gl.uniform1i(programs.outProgram, "whiteRendering", 1);
		renderTexture(renderVertices,1080, 1080,GL_TEXTURE7,GL_RGB,model,true,modelMaterials,view,albedoTextures,chosenTextureIndex);
		gl.uniform1i(programs.outProgram, "whiteRendering", 0);
		//Render uv mask

		//interpret the albedo with ui mask texture
		gl.uniform1i(programs.outProgram, "interpretWithUvMask", 1);
		renderTexture(renderVertices,1080, 1080,GL_TEXTURE0,GL_RGB,model, false,modelMaterials,view,albedoTextures,chosenTextureIndex);//Render enlarged texture
		gl.uniform1i(programs.outProgram, "interpretWithUvMask", 0);
		//interpret the albedo with ui mask texture


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Finish
		gl.bindFramebuffer(0);
		glViewport(-(maxScreenWidth - screenSizeX)/2, -(maxScreenHeight - screenSizeY), maxScreenWidth, maxScreenHeight);

		//Finish
		glUseProgram(programs.uiProgram);
	}
	else if(paintRender){
		GlSet gl;
		UserInterface ui;
		Texture txtr;
		
		glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
		outShaderData.renderTextureProjection = projection;
		gl.useOutShader(programs.outProgram, outShaderData);

		gl.uniform1i(programs.outProgram, "isTwoDimensional", 0);
		gl.viewport(1920, 1080);
		gl.bindFramebuffer(FBOScreen);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl.uniform1i(programs.outProgram,"renderPaintedTxtrMask",0);
		//Render painted image
			
		unsigned int chosenTxtr = 0;
		if(albedoTextures.size() != 0)
			chosenTxtr = albedoTextures[chosenTextureIndex].id;
		model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view,true,chosenTxtr,glm::vec3(0));

		GLubyte* renderedImage = new GLubyte[1080 * 1080 * 3 * sizeof(GLubyte)];
		glReadPixels(0, 0, 1080, 1080, GL_RGB, GL_UNSIGNED_BYTE, renderedImage);
		
		gl.activeTexture(GL_TEXTURE0);
		gl.texImage(renderedImage, 1080, 1080, GL_RGB);
		gl.generateMipmap();
		
		delete[]renderedImage;
		gl.bindFramebuffer(0);
		txtr.refreshScreenDrawingTexture();

		glViewport(-(maxScreenWidth - screenSizeX)/2, -(maxScreenHeight - screenSizeY), maxScreenWidth, maxScreenHeight);
	}
	

}