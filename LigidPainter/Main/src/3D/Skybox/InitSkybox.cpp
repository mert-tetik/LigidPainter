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

#include "3D/Skybox/Skybox.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "../../thirdparty/stb_image.h"

using namespace std;

Skybox::Skybox(){}

void Skybox::init(){
	//Couldn't use the constructor since the glad is not initialized before definition
	
	LigidGL::cleanGLErrors();

    Debugger::block("Skybox::init : start");
    Debugger::block("Skybox::init : start");

    Debugger::block("Skybox::init");

	std::vector<float> skyboxVertices = { //Array size is 108
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
	};

	//Generate the cubemap texture
	glGenTextures(1, &ID);
	LigidGL::testGLError("Skybox::init : Generating texture");

	//Generate vertex objects
	glGenVertexArrays(1, &VAO);
	LigidGL::testGLError("Skybox::init : Generating VAO");
	glGenBuffers(1, &VBO);
	LigidGL::testGLError("Skybox::init : Generating VBO");
	
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);
	LigidGL::testGLError("Skybox::init : Binding VAO");
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	LigidGL::testGLError("Skybox::init : Binding VBO");
	glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &skyboxVertices[0], GL_STATIC_DRAW);
	LigidGL::testGLError("Skybox::init : Buffer data");

	glEnableVertexAttribArray(0);
	LigidGL::testGLError("Skybox::init : glEnableVertexAttribArray 0");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	LigidGL::testGLError("Skybox::init : glVertexAttribPointer 0");

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	LigidGL::testGLError("Skybox::init : Binding default VBO");

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);
	LigidGL::testGLError("Skybox::init : Binding default VAO");
    
	Debugger::block("Skybox::init");
}