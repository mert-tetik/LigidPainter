/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

SceneTiles.hpp : Is used to render a single 2D square.
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
#include "UTIL/Shader/Shader.hpp"
#include "3D/ThreeD.hpp"
    
SceneTiles::SceneTiles(){}

void SceneTiles::init(){
    
    LigidGL::cleanGLErrors();

    Debugger::block("SceneTiles::init : start");
    Debugger::block("SceneTiles::init : start");

    Debugger::block("SceneTiles::init");

    const float tileSize = 1000.f;

    std::vector<float> SceneTilesVertices = { 
        // first triangle      
        tileSize,  0.0f,  tileSize      ,      1,0      ,       0,0,0       ,       0,0,0       ,       0,0,0,        // top right
        tileSize,  0.0f, -tileSize      ,      1,1      ,       0,0,0       ,       0,0,0       ,       0,0,0,        // bottom right
       -tileSize,  0.0f,  tileSize      ,      0,0      ,       0,0,0       ,       0,0,0       ,       0,0,0,        // top left 
        
        // second triangle              
        tileSize,  0.0f, -tileSize      ,      1,1      ,       0,0,0       ,       0,0,0       ,       0,0,0,        // bottom right
       -tileSize,  0.0f, -tileSize      ,      0,1      ,       0,0,0       ,       0,0,0       ,       0,0,0,        // bottom left
       -tileSize,  0.0f,  tileSize      ,      0,0      ,       0,0,0       ,       0,0,0       ,       0,0,0         // top left
    };

    //Generate vertex objects
    glGenVertexArrays(1, &VAO);
    LigidGL::testGLError("SceneTiles::init : Gen VAO");
    glGenBuffers(1, &VBO);
    LigidGL::testGLError("SceneTiles::init : Gen VBO");
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    LigidGL::testGLError("SceneTiles::init : Bind VAO");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("SceneTiles::init : Bind VBO");
    glBufferData(GL_ARRAY_BUFFER, SceneTilesVertices.size() * sizeof(float), &SceneTilesVertices[0], GL_STATIC_DRAW);
    LigidGL::testGLError("SceneTiles::init : VBO Data");

    // vertex pos
    glEnableVertexAttribArray(0);	
    LigidGL::testGLError("SceneTiles::init : Enable vertex attrib array 0");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    LigidGL::testGLError("SceneTiles::init : Set vertex attrib pointer 0");

    // vertex TextureMs coords
    glEnableVertexAttribArray(1);	
    LigidGL::testGLError("SceneTiles::init : Enable vertex attrib array 1");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    LigidGL::testGLError("SceneTiles::init : Set vertex attrib pointer 1");
    
    // vertex normals
    glEnableVertexAttribArray(2);	
    LigidGL::testGLError("SceneTiles::init : Enable vertex attrib array 2");
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    LigidGL::testGLError("SceneTiles::init : Set vertex attrib pointer 2");
    
    // vertex tangent
    glEnableVertexAttribArray(3);
    LigidGL::testGLError("SceneTiles::init : Enable vertex attrib array 3");
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
    LigidGL::testGLError("SceneTiles::init : Set vertex attrib pointer 3");
    
    // vertex bitangent
    glEnableVertexAttribArray(4);
    LigidGL::testGLError("SceneTiles::init : Enable vertex attrib array 4");
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
    LigidGL::testGLError("SceneTiles::init : Set vertex attrib pointer 4");


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    LigidGL::testGLError("SceneTiles::init : Binding default VBO");

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    LigidGL::testGLError("SceneTiles::init : Binding default VAO");
    
    Debugger::block("SceneTiles::init");
}

void SceneTiles::bindBuffers(){
    glBindVertexArray(VAO);
    LigidGL::testGLError("SceneTiles::bindBuffers : Bind VAO");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("SceneTiles::bindBuffers : Bind VBO");
}

void SceneTiles::unbindBuffers(){
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    LigidGL::testGLError("SceneTiles::unbindBuffers : Binding default VBO");

    glBindVertexArray(0);
    LigidGL::testGLError("SceneTiles::unbindBuffers : Binding default VAO");
}

void SceneTiles::draw(){
    this->bindBuffers();

    //Draw the SceneAxisDisplayer
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "SceneTiles::draw");

    this->unbindBuffers();
}