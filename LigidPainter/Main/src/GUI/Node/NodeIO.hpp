#ifndef LGDUINODEIO_HPP
#define LGDUINODEIO_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"
#include "Mouse.hpp"

#include "GUI/UI.hpp"
#include "GUI/Button.hpp"

#include "Node/NodeIO.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

//class NodeConnection;

class NodeIO
{
private:
    /* data */
public:
    //std::vector<NodeConnection> connections; //An input can have only one connection unlike the outputs
    std::string title; //Title of the connection
    Element element; //Which element will manipulate the value

    Button IOCircle;
    glm::vec4 color; //Color of the IO circle

    //Texture sampler2D output
    //Will be generated once
    unsigned int result; 

    NodeIO(std::string title ,Element element,glm::vec4 color,ColorPalette colorPalette,Shader buttonShader){
        this->title = title;
        this->element = element;
        this->color = color;

        IOCircle = Button(0,glm::vec2(1.f,1.f),colorPalette,buttonShader,"",Texture(),0.f,false);
    }
};

#endif