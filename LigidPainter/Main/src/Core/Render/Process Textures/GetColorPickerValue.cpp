#include<iostream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Render/Render.h"
#include "Core/UI/UserInterface.h"
#include "Core/LigidPainter.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

glm::vec3 Render::getColorPickerValue(unsigned int FBOScreen, ColorPicker &colorPicker,  int screenSizeX,  int screenSizeY, Programs programs, int maxScreenWidth,int maxScreenHeight,SaturationValShaderData &saturationValShaderData) {
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
	saturationValShaderData.boxColor = colorPicker.hueColorValue / 255.0f;
    gl.useSaturationValBoxShader(programs.saturationValBoxProgram,saturationValShaderData);

	//Setup
	gl.viewport(1920, 1080);
	gl.bindFramebuffer(FBOScreen);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Setup

	//Render color box
	GLubyte* colorBoxPixel = new GLubyte[1 * 1 * 3];//Color val
	gl.drawArrays(colorBox, false); 
	glReadPixels(1080.0f - ((colorPicker.saturationValuePosX * -1.0f + 0.1f) * 5.0f * 1080.0f), (colorPicker.saturationValuePosY + 0.2f) * 2.5f * 1080.0f, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, colorBoxPixel);
	//Render color box

	//Finish
	gl.bindFramebuffer(0);
	glViewport(-(glfwGetVideoMode(glfwGetPrimaryMonitor())->width - screenSizeX)/2, -(glfwGetVideoMode(glfwGetPrimaryMonitor())->height - screenSizeY), glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height);

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