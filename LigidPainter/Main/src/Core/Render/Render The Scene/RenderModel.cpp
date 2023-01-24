#include<iostream>

#include <vector>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/gl.h"


void Render::renderModel(bool backfaceCulling,PBRShaderData &data,Model &model,bool renderDefault, vector<MaterialOut> &modelMaterials,Programs programs,
int currentMaterialIndex,glm::mat4 view,bool paintingMode,std::vector<unsigned int> albedoTextures,int chosenTextureIndex) {
	glDepthFunc(GL_LESS); 

    GlSet gl;
	if(paintingMode)
		gl.usePBRShader(programs.PBRProgram,data);

	if (backfaceCulling) { //if backface culling checked in the model panel
		gl.enable(GL_CULL_FACE);
		gl.cullFace(GL_BACK);
	}
	//gl.meshDataToShaders();
	
	model.Draw(currentMaterialIndex,programs.PBRProgram,true,modelMaterials,view,paintingMode,albedoTextures,chosenTextureIndex);

	gl.disable(GL_CULL_FACE); //Disable backface culling if enabled

	glUseProgram(programs.uiProgram);
	glDepthFunc(GL_LESS); 
    //glDrawBuffer(GL_COLOR_ATTACHMENT0);
}