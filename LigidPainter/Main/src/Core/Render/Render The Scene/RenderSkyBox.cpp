#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/gl.h"

void Render::renderSkyBox(SkyBoxShaderData data,Programs programs,float exposureVal,float skyboxRotationVal) {
	GlSet gls;

	gls.useSkyBoxShader(programs.skyboxProgram,data);
	
	const float uniExpo = (exposureVal + 0.11f) * 4.54545454545 * 2 + 0.1;
	gls.uniform1f(programs.skyboxProgram,"exposure",uniExpo);
	
	const float uniRot = (skyboxRotationVal + 0.11f) * 1636.36363636;
	glm::mat4 rotation = glm::mat4(1);
	rotation = glm::rotate(rotation, glm::radians(uniRot), glm::vec3(0.0, 1.0, 0.0));
	
	gls.uniformMatrix4fv(programs.skyboxProgram,"rotation",rotation);
	
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
	glDepthMask(GL_TRUE);
	
	glUseProgram(programs.PBRProgram);
	gls.uniform1f(programs.PBRProgram,"skyboxExposure",uniExpo);

	glUseProgram(programs.uiProgram);
}

void Render::renderBackgroundImage(unsigned int renderTheTextureProgram, unsigned int viewportBGImage){
	glUseProgram(renderTheTextureProgram);

	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D,viewportBGImage);
	std::vector<float> renderVertices = { 
		// first triangle
		 1.0f,  1.0f, 0.0f,1,1,0,0,0,  // top right
		 1.0f,  -1.0f, 0.0f,1,0,0,0,0,  // bottom right
		 -1.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
		// second tria0gle	  ,0,0,0,
		 1.0f,  -1.0f, 0.0f,1,0,0,0,0,  // bottom right
		 -1.0f,  -1.0f, 0.0f,0,0,0,0,0,  // bottom left
		 -1.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};
	GlSet gls;
	gls.uniform1i(renderTheTextureProgram, "isPressed" ,0);
	gls.uniform1i(renderTheTextureProgram, "isHover" ,0);
		
	gls.drawArrays(renderVertices,0);
	glClear(GL_DEPTH_BUFFER_BIT);
}