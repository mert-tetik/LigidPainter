/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    From LearnOpenGL.com 

*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/GL/GL.hpp"
#include "GUI/GUI.hpp"

std::map<LigidWindow*, unsigned int> bound_shader_program_data;

static bool is_shader_program_bound_in_another_context(unsigned int ID, LigidWindow* bound_context){
    for (auto data : bound_shader_program_data)
    {
        if(data.first != bound_context && data.second == ID){
            return true;
        }
    }
    
    return false;
}

static double wait_time;

// activate the shader
// ------------------------------------------------------------------------
void Shader::use() 
{ 
    if(this->ID == 0){
        glUseProgram(0);
        return;
    }

    LigidWindow* bound_context = LigidGL::getBoundContext();

    // If no OpenGL context is bound
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : Shader::use : No OpenGL context is bound! : " << this->shader_name << LGDLOG::end; 
        return;
    }

    // Shader is already bound 
    if(ShaderUTIL::get_bound_shader().ID == ID){
        return;
    }

    // Don't let the code proceed while the texture is bound in another context 😡
    if(is_shader_program_bound_in_another_context(ID, bound_context)){
        wait_time = LigidGL::getTime();
        std::cout << "WARNING : Shader::use : Shader program  is already bound in a different context. Waiting until it's released! : " << this->shader_name << std::endl;
    }
    
    while(is_shader_program_bound_in_another_context(ID, bound_context)){
        if(LigidGL::getTime() - wait_time > 10)
            break;
    }

    LigidGL::cleanGLErrors();

    glUseProgram(ID); 
    LigidGL::testGLError("Shader::use : glUseProgram");
    bound_shader_program_data[bound_context] = ID;
}

void ShaderUTIL::release_bound_shader(){

    LigidWindow* bound_context = LigidGL::getBoundContext();

    // If no OpenGL context is bound
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : ShaderUTIL::release_bound_shader : No OpenGL context is bound!" << LGDLOG::end; 
        return;
    }
    
    glUseProgram(0); 
    bound_shader_program_data[bound_context] = 0;
}

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) 
{         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) 
{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) 
{ 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) 
{ 
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) 
{ 
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) 
{ 
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) 
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) 
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) 
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            LGDLOG::start<< "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << LGDLOG::end;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            LGDLOG::start<< "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << LGDLOG::end;
        }
    }
}


void ShaderUTIL::set_shader_struct_face_selection_data(Shader shader, Mesh& mesh){
    shader.use();
    if(mesh.face_selection_data.selectedFaces.ID && mesh.face_selection_data.meshMask.ID){
        shader.setFloat("face_selection_data.prim_txtr_res", mesh.face_selection_data.selectedFaces.getResolution().x);
        shader.setInt("face_selection_data.meshSelectionEditing", mesh.face_selection_data.editMode);
        shader.setInt("face_selection_data.hideUnselected", mesh.face_selection_data.hideUnselected);
        shader.setInt("face_selection_data.usingMeshSelection", mesh.face_selection_data.activated);
        shader.setInt("face_selection_data.selectedPrimitiveIDS", GL::bindTexture_2D(mesh.face_selection_data.selectedFaces.ID, "ShaderUTIL::set_shader_struct_face_selection_data"));
        shader.setInt("face_selection_data.meshMask", GL::bindTexture_2D(mesh.face_selection_data.meshMask.ID, "ShaderUTIL::set_shader_struct_face_selection_data"));
        shader.setInt("face_selection_data.primitiveCount", mesh.face_selection_data.selectedPrimitiveIDs.size());
    }
    else{
        shader.setInt("face_selection_data.usingMeshSelection", false);
    }
}

void ShaderUTIL::set_shader_struct_painting_data(Shader shader, PaintingData painting_data){
    shader.use();

    shader.setInt("painting_data.painting_buffers.albedo_txtr", GL::bindTexture_2D(painting_data.painting_buffers.albedo_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data : albedo_txtr"));
    shader.setInt("painting_data.painting_buffers.roughness_txtr", GL::bindTexture_2D(painting_data.painting_buffers.roughness_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data : roughness_txtr"));
    shader.setInt("painting_data.painting_buffers.metallic_txtr", GL::bindTexture_2D(painting_data.painting_buffers.metallic_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data : metallic_txtr"));
    shader.setInt("painting_data.painting_buffers.normal_map_txtr", GL::bindTexture_2D(painting_data.painting_buffers.normal_map_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data : normal_map_txtr"));
    shader.setInt("painting_data.painting_buffers.height_map_txtr", GL::bindTexture_2D(painting_data.painting_buffers.height_map_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data : height_map_txtr"));
    shader.setInt("painting_data.painting_buffers.ao_txtr", GL::bindTexture_2D(painting_data.painting_buffers.ao_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data : ao_txtr"));
    shader.setInt("painting_data.painting_buffers.brush_txtr", GL::bindTexture_2D(painting_data.painting_buffers.brush_txtr.ID, "ShaderUTIL::set_shader_struct_painting_data : brush_txtr"));
    
    shader.setFloat("painting_data.smear_data.transform_strength", painting_data.smear_data.transform_strength);
    shader.setFloat("painting_data.smear_data.blur_strength", painting_data.smear_data.blur_strength);
    
    shader.setFloat("painting_data.channel_data.roughness_strength", painting_data.channel_data.roughness_strength);
    shader.setFloat("painting_data.channel_data.metallic_strength", painting_data.channel_data.metallic_strength);
    shader.setFloat("painting_data.channel_data.normal_map_strength", painting_data.channel_data.normal_map_strength);
    shader.setFloat("painting_data.channel_data.height_map_strength", painting_data.channel_data.height_map_strength);
    shader.setFloat("painting_data.channel_data.ao_strength", painting_data.channel_data.ao_strength);
    
    shader.setInt("painting_data.brush_mode", painting_data.brush_mode); // Drawing 0, softening 1, smearing 2, normal map painting 3, filter painting 4, vector painting 5, bucket painting 6
    
    shader.setInt("painting_data.painting_over_active", painting_data.painting_over_active);
}

Shader ShaderUTIL::get_bound_shader(){

    LigidWindow* bound_context = LigidGL::getBoundContext();

    // If no OpenGL context is bound
    if(bound_context == nullptr){
        LGDLOG::start << "ERROR : ShaderUTIL::get_bound_shader : No OpenGL context is bound!" << LGDLOG::end; 
        return Shader();
    }

    Shader bound_shader;
    bound_shader.ID = bound_shader_program_data[bound_context];
    return bound_shader;
}