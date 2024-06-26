/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Box.hpp : Is used to render a single 2D square.
          Mostly used by GUI elements.
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/GL/GL.hpp"

#include "3D/ThreeD.hpp"
    
Box::Box(){}

void Box::init(){
    std::vector<float> boxVertices = { 
        // first triangle      
        //Position              //Texture coordinates
        1.0f,  1.0f, 0.0f       ,      1,0,         // top right
        1.0f,  -1.0f, 0.0f      ,      1,1,         // bottom right
        -1.0f,  1.0f, 0.0f      ,      0,0,         // top left 
        
        // second triangle              
        1.0f,  -1.0f, 0.0f      ,      1,1,         // bottom right
        -1.0f,  -1.0f, 0.0f     ,      0,1,         // bottom left
        -1.0f,  1.0f, 0.0f      ,      0,0          // top left
    };

    LigidGL::cleanGLErrors();

    //Generate vertex objects
    glGenVertexArrays(1, &VAO);
    LigidGL::testGLError("Box::init : glGenVertexArrays(1, &VAO)");
    glGenBuffers(1, &VBO);
    LigidGL::testGLError("Box::init : glGenBuffers(1, &VBO)");
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    LigidGL::testGLError("Box::init : glBindVertexArray(VAO)");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("Box::init : glBindBuffer(GL_ARRAY_BUFFER, VBO)");
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), &boxVertices[0], GL_STATIC_DRAW);
    LigidGL::testGLError("Box::init : glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), &boxVertices[0], GL_STATIC_DRAW)");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    LigidGL::testGLError("Box::init : glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0)");
    glEnableVertexAttribArray(0);
    LigidGL::testGLError("Box::init : glEnableVertexAttribArray(0)");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    LigidGL::testGLError("Box::init : glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)))");
    glEnableVertexAttribArray(1); 
    LigidGL::testGLError("Box::init : glEnableVertexAttribArray(1);");

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    LigidGL::testGLError("Box::init : glBindBuffer(GL_ARRAY_BUFFER, 0);");

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    LigidGL::testGLError("Box::init : ");
}

void Box::customMeshInit(glm::vec3 pos, glm::vec2 scale){
    std::vector<float> boxVertices = { 
        // first triangle      
        pos.x + scale.x,   pos.y + scale.y, pos.z      ,      1,0,    0,0,0,   0,0,0,  0,0,0,         // top right
        pos.x + scale.x,   pos.y - scale.y, pos.z      ,      1,1,    0,0,0,   0,0,0,  0,0,0,         // bottom right
        pos.x - scale.x,   pos.y + scale.y, pos.z      ,      0,0,    0,0,0,   0,0,0,  0,0,0,         // top left 
        pos.x + scale.x,   pos.y - scale.y, pos.z      ,      1,1,    0,0,0,   0,0,0,  0,0,0,         // bottom right
        pos.x - scale.x,   pos.y - scale.y, pos.z      ,      0,1,    0,0,0,   0,0,0,  0,0,0,         // bottom left
        pos.x - scale.x,   pos.y + scale.y, pos.z      ,      0,0,    0,0,0,   0,0,0,  0,0,0          // top left
    };

    //Generate vertex objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), &boxVertices[0], GL_DYNAMIC_DRAW);

    // vertex Positions
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

void Box::customMeshUpdate(glm::vec3 pos, glm::vec2 scale){
    
    this->customPos = pos;
    this->customScale = scale;
    
    std::vector<float> boxVertices = { 
        // first triangle      
        pos.x + scale.x,   pos.y + scale.y, pos.z      ,      1,0,    0,0,0,   0,0,0,  0,0,0,         // top right
        pos.x + scale.x,   pos.y - scale.y, pos.z      ,      1,1,    0,0,0,   0,0,0,  0,0,0,         // bottom right
        pos.x - scale.x,   pos.y + scale.y, pos.z      ,      0,0,    0,0,0,   0,0,0,  0,0,0,         // top left 
        pos.x + scale.x,   pos.y - scale.y, pos.z      ,      1,1,    0,0,0,   0,0,0,  0,0,0,         // bottom right
        pos.x - scale.x,   pos.y - scale.y, pos.z      ,      0,1,    0,0,0,   0,0,0,  0,0,0,         // bottom left
        pos.x - scale.x,   pos.y + scale.y, pos.z      ,      0,0,    0,0,0,   0,0,0,  0,0,0          // top left
    };

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), &boxVertices[0], GL_STATIC_DRAW);

    // vertex Positions
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

void Box::bindBuffers(){
    //Bind vertex objects
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindVertexArray(this->VAO);
}

void Box::unbindBuffers(){
    //Bind vertex objects
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Box::draw(std::string location){
    GLint currentVboId;

    // Query the currently bound buffer for the GL_ARRAY_BUFFER target
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentVboId);

    // Compare the current VBO ID with the ID you want to check
    bool isBound = (this->VBO == static_cast<GLuint>(currentVboId));
    
    //Bind vertex objects
    if(!isBound)
        this->bindBuffers();

    //Draw the box
    GL::makeDrawCall(GL_TRIANGLES, 0, 6, location);
}