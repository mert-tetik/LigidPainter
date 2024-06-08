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

// Render the mesh
void Mesh::Draw(std::string location) 
{
    LigidWindow* current_context = LigidGL::getBoundContext();
    
    // There is no currently bound context
    if(current_context == nullptr){
        LGDLOG::start << "ERROR : Mesh::Draw no context is bound" << LGDLOG::end;
        return;
    }

    // Vertex buffers for this context is not initialised
    if(this->vertex_buffers.find(current_context) == this->vertex_buffers.end())
    {
        // Initialize buffers
        this->initVertexBuffers(
                            &this->vertex_buffers[current_context].VBO, 
                            &this->vertex_buffers[current_context].VAO, 
                            &this->vertex_buffers[current_context].EBO
                        );
    }

    LigidGL::cleanGLErrors();

    // draw mesh
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffers[current_context].VBO);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding VBO");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertex_buffers[current_context].EBO);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding EBO"); 
    
    glBindVertexArray(this->vertex_buffers[current_context].VAO);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding VAO");
    
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Drawing Elements");

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding 0 vertex buffer");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding 0 element array buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding 0 array buffer");
}

void Mesh::draw_height_map_processed_vertices(std::string location){
    LigidGL::cleanGLErrors();

    if(this->height_map_processed_vertex_buffers.VBO == 0){
        LigidGL::testGLError("Mesh::draw_height_map_processed_vertices : invalid VBO ID");
        return;
    }
    if(this->height_map_processed_vertex_buffers.EBO == 0){
        LigidGL::testGLError("Mesh::draw_height_map_processed_vertices : invalid EBO ID");
        return;
    }
    if(this->height_map_processed_vertex_buffers.VAO == 0){
        LigidGL::testGLError("Mesh::draw_height_map_processed_vertices : invalid VAO ID");
        return;
    }

    // draw mesh
    glBindBuffer(GL_ARRAY_BUFFER, this->height_map_processed_vertex_buffers.VBO);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding VBO");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->height_map_processed_vertex_buffers.EBO);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding EBO"); 
    
    glBindVertexArray(this->height_map_processed_vertex_buffers.VAO);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding VAO");
    
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Drawing Elements");

    glBindVertexArray(0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding 0 vertex buffer");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding 0 element array buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    LigidGL::testGLError("Mesh::Draw : " + location + " : Binding 0 array buffer");
}