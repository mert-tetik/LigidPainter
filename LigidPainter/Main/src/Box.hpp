/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Box.hpp : Is used to render a single 2D square.
          Mostly used by GUI elements. (GUI folder)
*/

#ifndef LGDBOX_HPP
#define LGDBOX_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.hpp"
#include "Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class Box
{
private:
    /* data */
public:
    unsigned int VBO;
    unsigned int VAO;

    Box(){}

    void init(){
        std::vector<float> boxVertices = { 
			// first triangle      
            //Position              //Texture coordinates
			1.0f,  1.0f, 0.0f       ,      1,1,         // top right
			1.0f,  -1.0f, 0.0f      ,      1,0,         // bottom right
			-1.0f,  1.0f, 0.0f      ,      0,1,         // top left 
			
            // second triangle              
			1.0f,  -1.0f, 0.0f      ,      1,0,         // bottom right
			-1.0f,  -1.0f, 0.0f     ,      0,0,         // bottom left
			-1.0f,  1.0f, 0.0f      ,      0,1          // top left
		};

        //Generate vertex objects
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), &boxVertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); 

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);
    }

    void bindBuffers(){
        //Bind vertex objects
	    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }
    void unbindBuffers(){
        //Bind vertex objects
	    glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw(Shader shader,glm::vec3 pos,glm::vec2 scale){
        
        shader.setVec3("pos",pos);
        shader.setVec2("scale",scale);

        //Bind vertex objects
	    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //Draw the box
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Bind default buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);
    }
};

#endif 