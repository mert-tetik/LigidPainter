#ifndef LGDUIELEMENTS_HPP
#define LGDUIELEMENTS_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class UI{
public:
    Box box;
    
    UI(){

    }
};


class Button
{
private:
    /* data */
public:
    Button(Box box){
        
        box.draw();
    }
};

#endif