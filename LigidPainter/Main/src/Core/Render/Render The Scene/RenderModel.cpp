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
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include "tinyfiledialogs.h"

void Render::renderModel(bool backfaceCulling,PBRShaderData &data,Model &model,bool renderDefault, vector<unsigned int> &albedoTextures,Programs programs,int currentMaterialIndex) {
	glDepthFunc(GL_LESS); 

    GlSet gl;
	gl.usePBRShader(programs.PBRProgram,data);

	if (backfaceCulling) { //if backface culling checked in the model panel
		gl.enable(GL_CULL_FACE);
		gl.cullFace(GL_BACK);
	}
	gl.meshDataToShaders();
	
	model.Draw(currentMaterialIndex,programs.PBRProgram,true,albedoTextures);


	gl.disable(GL_CULL_FACE); //Disable backface culling if enabled

	glUseProgram(programs.uiProgram);
	glDepthFunc(GL_LESS); 

}