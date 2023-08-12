/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef SHADERS_HPP
#define SHADERS_HPP

//OpenGL & window 
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map

/// @brief Manages shader programs
class Shader
{
public:
    //OpenGL shader pipeline : .vert -> .tc -> .te -> .geom -> .frag

    /// @brief Id of the shader in an OpenGL context
    unsigned int ID = 0;

    /// @brief Default constructor (is not used) 
    Shader();

    /// @brief Create the shader program compiling the shader code in the given paths (give nullptr if the shader is not used)
    /// @param vertexPath Vertex shader file (.vert file)
    /// @param fragmentPath Fragment shader file (.frag file)
    /// @param geometryPath Geometry shader file (.geom file)
    /// @param tessControlPath Tessellation control shader file (.tc file)
    /// @param tessEvalPath Tessellation evaluation shader file (.te file)
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, const char* tessControlPath, const char* tessEvalPath);

    /// @brief replace the #pragma LIGID_INCLUDE with the code in the given path (LIGID_INCLUDE(givenPath))
    /// @param code the code
    void processShaderCode(std::string &code);

    /// @brief use the shader program 
    void use();
    
    // Utility uniform functions
    
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setVec2(const std::string &name, const glm::vec2 &value);
    void setVec2(const std::string &name, float x, float y);
    void setVec3(const std::string &name, const glm::vec3 &value);
    void setVec3(const std::string &name, float x, float y, float z);
    void setVec4(const std::string &name, const glm::vec4 &value);
    void setVec4(const std::string &name, float x, float y, float z, float w);
    void setMat2(const std::string &name, const glm::mat2 &mat);
    void setMat3(const std::string &name, const glm::mat3 &mat);
    void setMat4(const std::string &name, const glm::mat4 &mat);
    

private:
    /// @brief Checks if an error is occured (print the error msg to the terminal)
    /// @param shader shader id
    /// @param type indicate if compiling a .frag file, .tc file or .vert file etc. (to print to the terminal) 
    void checkCompileErrors(GLuint shader, std::string type);
};

namespace ShaderSystem{
    Shader tdModelShader();
    Shader skyboxShader();
    Shader skyboxBall();
    Shader buttonShader();
    Shader prefilteringShader();
    Shader connectionCurve();
    Shader colorPicker();
    Shader twoDPainting();
    Shader depth3D();
    Shader textureUpdatingShader();
    Shader twoDPaintingModeAreaShader();
    Shader circleShader();
    Shader dotsShader();
    Shader heightToNormalMap();
    Shader boundaryExpandingShader();
    Shader uvMaskShader();
    Shader bluringShader();
    Shader proceduralDisplayerShader();
    Shader colorIDMaskingShader();
    Shader grayScaleIDMaskingShader();
    Shader to2DProcedural();

    void initShaderSystem();
};

#endif