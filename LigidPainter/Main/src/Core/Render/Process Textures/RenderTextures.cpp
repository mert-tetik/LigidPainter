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

void Render::renderTextures(unsigned int FBOScreen, bool exportImage, bool JPG, bool PNG, const char* exportPath, int screenSizeX,  int screenSizeY,const char* exportFileName, OutShaderData outShaderData,Model &model,bool renderDefault,vector<unsigned int> &albedoTextures,bool paintOut,bool isRenderTexture,bool paintRender,bool firstPaint,int currentMaterialIndex,std::vector<UndoActions> &undoList,Programs programs, int maxScreenWidth , int maxScreenHeight,std::vector<MaterialOut> &modelMaterials,glm::mat4 view) {
	int maxHistoryHold = 20;
	
	if(firstPaint){
		//UNDO
		Texture txtr;

		GLubyte* originalImage = txtr.getTextureFromProgram(GL_TEXTURE0, 1080, 1080, 3);

		//Send the texture to the undoList before processing the texture (will be used for ctrl z)
		UndoActions undoAct;
		undoAct.activeMaterial = currentMaterialIndex;
		undoAct.undoTextures = originalImage; 
		undoList.push_back(undoAct);


    	//Delete the first element from undoList if undoList's count is greated than max history holding value which is 20
		if (undoList.size() > maxHistoryHold){
			//Delete the texture
			GLubyte* undoTexture = undoList[0].undoTextures;
			delete[] undoTexture;

			//Remove the element
			undoList.erase(undoList.begin());
		}
	}

	if(isRenderTexture){

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
		Texture txtr;

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
		ui.setViewportBgColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//Render painted image
		model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view);

		if (!paintOut)
			gl.drawArrays(renderVertices, false);

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
		renderTexture(renderVertices,1080, 1080,GL_TEXTURE7,GL_RGB,model,true,modelMaterials,view);
		gl.uniform1i(programs.outProgram, "whiteRendering", 0);
		//Render uv mask

		//interpret the albedo with ui mask texture
		gl.uniform1i(programs.outProgram, "interpretWithUvMask", 1);
		renderTexture(renderVertices,1080, 1080,GL_TEXTURE0,GL_RGB,model, false,modelMaterials,view);//Render enlarged texture
		gl.uniform1i(programs.outProgram, "interpretWithUvMask", 0);
		//interpret the albedo with ui mask texture

		//Download enlarged texture
		if (exportImage) {
    	    exportTexture(JPG,PNG,exportPath,exportFileName,albedoTextures);
		}

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

		ui.setViewportBgColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gl.uniform1i(programs.outProgram,"renderPaintedTxtrMask",0);
		//Render painted image
		model.Draw(currentMaterialIndex,programs.PBRProgram,false,modelMaterials,view);

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