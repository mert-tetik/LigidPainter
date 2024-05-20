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
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "GUI/GUI.hpp"

///@brief Default constructor 
Shader::Shader(){}

void Shader::loadShader(std::string vertexCode, std::string fragmentCode, std::string shader_name){
    if(vertexCode == "" || fragmentCode == ""){
        LGDLOG::start << "ERROR : Shader source code is empty" << LGDLOG::end;
        return;
    }

    this->shader_name = shader_name;

    //Process the shader codes
    this->processShaderCode(vertexCode);
    this->processShaderCode(fragmentCode);
    
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    
    // 2. compile shaders
    unsigned int vertex, fragment;
    
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::loadShaderPS(std::string vertexPath, std::string fragmentCode, std::string shader_name){
    std::string vertexCode;
    std::ifstream vShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    this->shader_name = shader_name;
    
    try
    {
        if(vertexPath != "")
        {
            LGDLOG::start<< "Loading : " << vertexPath << LGDLOG::end;   
            vShaderFile.open(vertexPath);
            std::stringstream vShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            vShaderFile.close();
            vertexCode = vShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        LGDLOG::start<< "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
            << e.what() << LGDLOG::end;
    }

    this->loadShader(vertexCode, fragmentCode, shader_name);
}

void Shader::loadShaderSP(std::string vertexCode, std::string fragmentPath, std::string shader_name){
    std::string fragCode;
    std::ifstream fShaderFile;
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    this->shader_name = shader_name; 

    try
    {
        if(fragmentPath != "")
        {
            LGDLOG::start<< "Loading : " << fragmentPath << LGDLOG::end;   
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream;
            vShaderStream << fShaderFile.rdbuf();
            fShaderFile.close();
            fragCode = vShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        LGDLOG::start<< "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
            << e.what() << LGDLOG::end;
    }

    this->loadShader(vertexCode, fragCode, shader_name);
}

void Shader::loadShaderPP(std::string vertexPath, std::string fragmentPath, std::string shader_name){
    
    this->fragPath = fragmentPath;
    this->vertPath = vertexPath;
    this->shader_name = shader_name;

    std::string fragCode;
    std::ifstream fShaderFile;
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try
    {
        if(fragmentPath != "")
        {
            LGDLOG::start<< "Loading : " << fragmentPath << LGDLOG::end;   
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream;
            vShaderStream << fShaderFile.rdbuf();
            fShaderFile.close();
            fragCode = vShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        LGDLOG::start<< "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
            << e.what() << LGDLOG::end;
    }

    std::string vertexCode;
    std::ifstream vShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    try
    {
        if(vertexPath != "")
        {
            LGDLOG::start<< "Loading : " << vertexPath << LGDLOG::end;   
            vShaderFile.open(vertexPath);
            std::stringstream vShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            vShaderFile.close();
            vertexCode = vShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        LGDLOG::start<< "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
            << e.what() << LGDLOG::end;
    }

    this->loadShader(vertexCode, fragCode, shader_name);
}