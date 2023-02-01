#include<iostream>

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/Gl.h"

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