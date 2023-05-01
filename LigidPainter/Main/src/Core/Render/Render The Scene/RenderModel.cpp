#include<iostream>

#include <vector>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/gl.h"


void Render::renderModel(bool backfaceCulling,PBRShaderData &data,Model &model,bool renderDefault, vector<MaterialOut> &modelMaterials,Programs programs,
int currentMaterialIndex,glm::mat4 view,bool paintingMode,std::vector<aTexture> albedoTextures,int chosenTextureIndex,glm::vec3 viewPos,float skyboxExposureVal,
float skyboxRotationVal,Objects &objects,std::vector<NodeScene> nodeScenes,glm::mat4 &modelMatrix,bool useUV,float lightRotation) {
	glDepthFunc(GL_LESS); 

    GlSet gl;
	if(paintingMode)
		gl.usePBRShader(programs.PBRProgram,data);

	if (backfaceCulling) { //if backface culling checked in the model panel
		gl.enable(GL_CULL_FACE);
		gl.cullFace(GL_BACK);
	}
	
	
	//gl.meshDataToShaders();
	unsigned int chosenTxtr = 0;
	if(albedoTextures.size() != 0)
		chosenTxtr = albedoTextures[chosenTextureIndex].id;
	
	std::vector<unsigned int> materialOutputs;
	for (size_t i = 0; i < nodeScenes.size(); i++)
	{
		materialOutputs.push_back(nodeScenes[i].outTexture);
	}
	
	model.Draw(currentMaterialIndex,programs.PBRProgram,true,modelMaterials,view,paintingMode,chosenTxtr,viewPos,skyboxExposureVal,skyboxRotationVal,false,materialOutputs,programs.materialResult,modelMatrix,useUV,lightRotation);

	gl.disable(GL_CULL_FACE); //Disable backface culling if enabled

	glUseProgram(programs.uiProgram);
	glDepthFunc(GL_LESS); 
    //glDrawBuffer(GL_COLOR_ATTACHMENT0);
}