/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/GL/GL.hpp"

#include "3D/Skybox/Skybox.hpp"

void Skybox::draw(){ //Draw the skybox
	LigidGL::cleanGLErrors();

	//Disable the depth testing
	glDisable(GL_DEPTH_TEST);
	LigidGL::testGLError("Skybox::draw : glDisable(GL_DEPTH_TEST)");
	glDepthMask(GL_FALSE);
	LigidGL::testGLError("Skybox::draw : glDepthMask(GL_FALSE)");
	glDepthFunc(GL_LEQUAL);
	LigidGL::testGLError("Skybox::draw : glDepthFunc(GL_LEQUAL)");

	//Bind vertex objects
	glBindVertexArray(VAO);
	LigidGL::testGLError("Skybox::draw : binding VAO");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	LigidGL::testGLError("Skybox::draw : binding VBO");

	//Draw the skybox
	GL::makeDrawCall(GL_TRIANGLES, 0, 108, "Skybox draw");

	//Bind default buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	LigidGL::testGLError("Skybox::draw : binding default VBO");
	glBindVertexArray(0);
	LigidGL::testGLError("Skybox::draw : binding default VAO");

	//Enable the depth testing
	glEnable(GL_DEPTH_TEST);
	LigidGL::testGLError("Skybox::draw : glEnable(GL_DEPTH_TEST)");
	glDepthFunc(GL_LESS);
	LigidGL::testGLError("Skybox::draw : glDepthFunc(GL_LESS)");
	glDepthMask(GL_TRUE);
	LigidGL::testGLError("Skybox::draw : glDepthMask(GL_TRUE)");
}