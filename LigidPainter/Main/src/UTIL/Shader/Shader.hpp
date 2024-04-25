/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include <unordered_map> //std::unordered_map
#include <any> //std::any

/// @brief Manages shader programs
class Shader
{
public:
    //OpenGL shader pipeline : .vert -> .tc -> .te -> .geom -> .frag

    /// @brief Id of the shader in an OpenGL context
    unsigned int ID = 0;

    /// @brief Default constructor (is not used) 
    Shader();

    std::string fragPath = "";
    std::string vertPath = "";

    /// @brief Creates shader program using source codes
    void loadShader(std::string vertexCode, std::string fragmentCode);
    
    /// @brief Creates shader program using vertex shader path & fragment shader source code
    void loadShaderPS(std::string vertexPath, std::string fragmentCode);
    
    /// @brief Creates shader program using vertex shader source code & fragment shader path
    void loadShaderSP(std::string vertexCode, std::string fragmentPath);
    
    /// @brief Creates shader program using vertex shader path & fragment shader path
    void loadShaderPP(std::string vertexPath, std::string fragmentPath);

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
    void setVec3(const std::string &name, const glm::vec3 &value);
    void setVec4(const std::string &name, const glm::vec4 &value);
    void setMat2(const std::string &name, const glm::mat2 &mat);
    void setMat3(const std::string &name, const glm::mat3 &mat);
    void setMat4(const std::string &name, const glm::mat4 &mat);

private:
    /// @brief Checks if an error is occured (print the error msg to the terminal)
    /// @param shader shader id
    /// @param type indicate if compiling a .frag file, .tc file or .vert file etc. (to print to the terminal) 
    void checkCompileErrors(GLuint shader, std::string type);

    std::unordered_map<std::string, std::any> uniformValues;
};

namespace ShaderSystem{
    Shader tdModelShader();
    Shader PBRDisplayOnly();
    Shader skyboxShader();
    Shader skyboxModelShader();
    Shader skyboxBall();
    Shader buttonShader();
    Shader prefilteringShader();
    Shader vectoralCurve();
    Shader twoDPainting();
    Shader depth3D();
    Shader renderModelData();
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
    Shader sceneTilesShader();
    Shader sceneAxisDisplayerShader();
    Shader paintOverTextureGen();
    Shader txtrEditorResizeShader();
    Shader txtrEditorBlurShader();
    Shader txtrEditorDistortionShader();
    Shader txtrEditorNormalMapShader();
    Shader txtrEditorColoringShader();
    Shader splitTexturesShader();
    Shader solidShadingShader();
    Shader textureRenderingShader();
    Shader normalVectorShader();
    Shader edgeWearShader();
    Shader bakingShader();
    Shader AOGen();
    Shader defaultFramebufferShader();
    Shader nodeConnectionCurve();
    Shader projectingPaintedTextureShader();
    Shader projectedPaintingTextureMixerShader();
    Shader primitiveIDShader();
    Shader modelObjectID();
    Shader objectTexturingAssign();
    Shader twoPassBlur();
    Shader gizmo();
    Shader color3d();
    Shader color2d();
    Shader threeDTextureRenderingShader();
    Shader alphaZero3D();
    Shader color3DInstancing();
    Shader layersUpdate();
    Shader solidPaintingShader();
    Shader removeUnselectedFacesShader();

    
    Shader textureModifierShader();
    Shader dustModifierShader();
    Shader asphaltModifierShader();
    Shader liquidModifierShader();
    Shader mossModifierShader();
    Shader rustModifierShader();
    Shader skinModifierShader();
    Shader solidModifierShader();
    Shader woodenModifierShader();
    Shader mathModifierShader();

    void initShaderSystem();
};

#endif