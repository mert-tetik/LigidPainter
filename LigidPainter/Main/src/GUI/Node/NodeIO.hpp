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

struct NodeConnection{
    int nodeIndex; //Connected to which node
    int inputIndex; //Connected to which input

    NodeConnection(int nodeIndex,int inputIndex){
        this->nodeIndex = nodeIndex;
        this->inputIndex = inputIndex;
    }
};

class NodeIO
{
private:
    /* data */
public:
    std::vector<NodeConnection> connections; //An input can have only one connection unlike the outputs
    std::string title; //Title of the connection
    Element element; //Which element will manipulate the value

    Button IOCircle;
    glm::vec4 color; //Color of the IO circle

    //Texture sampler2D output
    //Will be generated once
    unsigned int result; 

    NodeIO(std::string title ,Element element,glm::vec4 color,ColorPalette colorPalette,Shader buttonShader,glm::vec2 videoScale){
        this->title = title;
        this->element = element;
        this->color = color;

        IOCircle = Button(0,glm::vec2(videoScale.y,videoScale.x)/glm::vec2(videoScale.y*2.5f),colorPalette,buttonShader,"",Texture(),0.f,false);
        IOCircle.radius = 0.25f;
    }
};

#endif