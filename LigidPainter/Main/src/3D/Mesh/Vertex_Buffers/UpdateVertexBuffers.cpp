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

void Mesh::update_vertex_buffers(){

    LigidWindow* current_context = LigidGL::getBoundContext();

    if(current_context == nullptr){
        LGDLOG::start << "ERROR : Mesh::update_vertex_buffers no context is bound" << LGDLOG::end;
        return;
    }

    if(!vertices.size())
        vertices.push_back(Vertex());

    if(!indices.size())
        indices.push_back(0);

    LigidGL::cleanGLErrors();
    
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffers[current_context].VBO);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding VBO");
    
    glBindVertexArray(this->vertex_buffers[current_context].VAO);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding VAO");

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);  
    LigidGL::testGLError("Mesh::update_vertex_buffers : GL_ARRAY_BUFFER data");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertex_buffers[current_context].EBO);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding EBO");

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);  
    LigidGL::testGLError("Mesh::update_vertex_buffers : GL_ELEMENT_ARRAY_BUFFER data");

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding 0 vertex buffer");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding 0 element array buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::update_vertex_buffers : Binding 0 array buffer");
}