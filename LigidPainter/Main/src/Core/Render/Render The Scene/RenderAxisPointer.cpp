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

void Render::renderAxisPointer(AxisPointerShaderData axisPointerShaderData,Programs programs) {
	GlSet glset;
		//Axis Pointer
		std::vector<float>axisPointer{
		0.0f, -10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Y
		0.0f, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //X
		10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Z
		0.0f, 0.0f, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	};
	glset.useAxisPointerShader(programs.axisPointerProgram,axisPointerShaderData);
	
	glset.blendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glset.drawArrays(axisPointer, true);
	glset.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(programs.uiProgram);
}