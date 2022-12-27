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

glm::vec3 Render::getColorBoxValue(unsigned int FBOScreen,float colorBoxPickerValue_x, float colorBoxPickerValue_y,  int screenSizeX,  int screenSizeY, glm::vec3 hueVal, Programs programs, int maxScreenWidth,int maxScreenHeight,SaturationValShaderData &saturationValShaderData) {
	std::vector<float> colorBox = { //Render color box into the screen
	// first triangle
	 0.0f,  1.0f, 0.0f,1,1,1,1,1,  // top right
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right
	 1.0f,  1.0f, 0.0f,0,1,0,0,0,  // top left 
	// second triangle	  ,0,0,0,
	 0.0f,  0.0f, 0.0f,1,0,0,0,0,  // bottom right7
	 1.0f,  0.0f, 0.0f,0,0,0,0,0,  // bottom left
	 1.0f,  1.0f, 0.0f,0,1,0,0,0   // top left
	};

    GlSet gl;
	glm::mat4 projection = glm::ortho(0.0f, 1.77777777778f, 0.0f, 1.0f);
	saturationValShaderData.renderTextureProjection = projection;
	saturationValShaderData.boxColor = hueVal / 255.0f;
    gl.useSaturationValBoxShader(programs.saturationValBoxProgram,saturationValShaderData);

	//Setup
	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render color box
	GLubyte* colorBoxPixel = new GLubyte[1 * 1 * 3];//Color val
	gl.drawArrays(colorBox, false); //Render Model
	glReadPixels(1080.0f - ((colorBoxPickerValue_x * -1.0f + 0.1f) * 5.0f * 1080.0f), (colorBoxPickerValue_y + 0.2f) * 2.5f * 1080.0f, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorBoxPixel);
	//Render color box

	//Finish
	gl.bindFramebuffer(0);
	glViewport(-(maxScreenWidth - screenSizeX)/2, -(maxScreenHeight - screenSizeY), maxScreenWidth, maxScreenHeight);

	glUseProgram(programs.uiProgram);


	//Get color value to the color vec
    glm::vec3 colorBoxPixelVal = glm::vec3(0);
	colorBoxPixelVal.r = colorBoxPixel[0];
	colorBoxPixelVal.g = colorBoxPixel[1];
	colorBoxPixelVal.b = colorBoxPixel[2];
	delete[]colorBoxPixel;
    return colorBoxPixelVal;
	//Finish
}