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

void Render::renderSkyBox(SkyBoxShaderData data,Programs programs) {
	GlSet gls;

	gls.useSkyBoxShader(programs.skyboxProgram,data);
	
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	std::vector<float> skyboxVertices = {
	    // positions          
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f,  1.0f ,0,0,  0,0,0,
	    -1.0f,  1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f, -1.0f ,0,0,  0,0,0,
	    -1.0f, -1.0f,  1.0f ,0,0,  0,0,0,
	     1.0f, -1.0f,  1.0f ,0,0,  0,0,0
	};

	gls.drawArrays(skyboxVertices,false);
	glDepthFunc(GL_LESS);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	
	glUseProgram(programs.uiProgram);
}