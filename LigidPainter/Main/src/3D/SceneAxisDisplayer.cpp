/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

SceneAxisDisplayer.hpp : Is used to render a single 2D square.
          Mostly used by GUI elements. (GUI folder)
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"
#include "3D/ThreeD.hpp"
    
SceneAxisDisplayer::SceneAxisDisplayer(){}

void SceneAxisDisplayer::init(){
    
    const float tileSize = 1000.f;

    std::vector<float> SceneAxisDisplayerVertices = { 
        // X      
        tileSize,  0.0f,  0.0f      ,      1,0      ,       0,0,0       ,       0,0,0       ,       0,0,0,
       -tileSize,  0.0f,  0.0f      ,      1,1      ,       0,0,0       ,       0,0,0       ,       0,0,0,
        
        // Y      
        0.0f,  tileSize,  0.0f      ,      1,0      ,       0,0,0       ,       0,0,0       ,       0,0,0,
        0.0f, -tileSize,  0.0f      ,      1,1      ,       0,0,0       ,       0,0,0       ,       0,0,0,
        
        // Z      
        0.0f, 0.0f,  tileSize       ,      1,0      ,       0,0,0       ,       0,0,0       ,       0,0,0,
        0.0f, 0.0f, -tileSize       ,      1,1      ,       0,0,0       ,       0,0,0       ,       0,0,0,
    };

    //Generate vertex objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, SceneAxisDisplayerVertices.size() * sizeof(float), &SceneAxisDisplayerVertices[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex TextureMs coords
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    // vertex normals
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

void SceneAxisDisplayer::bindBuffers(){
    //Bind vertex objects
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void SceneAxisDisplayer::unbindBuffers(){
    //Bind vertex objects
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SceneAxisDisplayer::draw(){
    //Bind vertex objects
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);

    //Draw the SceneAxisDisplayer
    glDrawArrays(GL_LINES, 0, 6);

    //Bind default buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}