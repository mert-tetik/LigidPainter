/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include <LigidGL/LigidGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#include "3D/Mesh/Mesh.hpp"
#include "GUI/GUI.hpp"

// initializes all the buffer objects/arrays
void Mesh::initVertexBuffers(unsigned int* VBO, unsigned int* VAO, unsigned int* EBO)
{
    LigidGL::cleanGLErrors();
    
    // create buffers/arrays
    glGenVertexArrays(1, VAO);
    LigidGL::testGLError("Mesh::initVertexBuffers : Generating VAO");
    glGenBuffers(1, VBO);
    LigidGL::testGLError("Mesh::initVertexBuffers : Generating VBO");
    glGenBuffers(1, EBO);
    LigidGL::testGLError("Mesh::initVertexBuffers : Generating EBO");

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    LigidGL::testGLError("Mesh::initVertexBuffers : Binding VBO");
    glBindVertexArray(*VAO);
    LigidGL::testGLError("Mesh::initVertexBuffers : Binding VAO");
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
    LigidGL::testGLError("Mesh::initVertexBuffers : GL_ARRAY_BUFFER data");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    LigidGL::testGLError("Mesh::initVertexBuffers : Binding EBO");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    LigidGL::testGLError("Mesh::initVertexBuffers : GL_ELEMENT_ARRAY_BUFFER data");

    // vertex Positions
    glEnableVertexAttribArray(0);	
    LigidGL::testGLError("Mesh::initVertexBuffers : glEnableVertexAttribArray 0");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    LigidGL::testGLError("Mesh::initVertexBuffers : glVertexAttribPointer 0");
    
    // vertex TextureMs coords
    glEnableVertexAttribArray(1);	
    LigidGL::testGLError("Mesh::initVertexBuffers : glEnableVertexAttribArray 1");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    LigidGL::testGLError("Mesh::initVertexBuffers : glVertexAttribPointer 1");
    
    // vertex normals
    glEnableVertexAttribArray(2);	
    LigidGL::testGLError("Mesh::initVertexBuffers : glEnableVertexAttribArray 2");
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    LigidGL::testGLError("Mesh::initVertexBuffers : glVertexAttribPointer 2");
    
    // vertex tangent
    glEnableVertexAttribArray(3);
    LigidGL::testGLError("Mesh::initVertexBuffers : glEnableVertexAttribArray 3");
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
    LigidGL::testGLError("Mesh::initVertexBuffers : glVertexAttribPointer 3");
    
    // vertex bitangent
    glEnableVertexAttribArray(4);
    LigidGL::testGLError("Mesh::initVertexBuffers : glEnableVertexAttribArray 4");
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
    LigidGL::testGLError("Mesh::initVertexBuffers : glVertexAttribPointer 4");

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::initVertexBuffers : Binding 0 vertex buffer");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::initVertexBuffers : Binding 0 EBO");
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::initVertexBuffers : Binding 0 array buffer");
}