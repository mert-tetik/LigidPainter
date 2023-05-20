#ifndef LGDCOLORPALETTE_HPP
#define LGDCOLORPALETTE_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class ColorPalette
{
private:
    /* data */
public:
    //                                       R             G           B          A
    glm::vec4 themeColor = glm::vec4(       0.043f      ,0.635f     ,0.823f     ,1.f); //LigidPainter's theme color which is cyan-like light blue 
    glm::vec4 mainColor = glm::vec4(        0.26        ,0.26       ,0.26       ,1.f); //That color will be used oftenly
    glm::vec4 secondColor = glm::vec4(      0.16        ,0.16       ,0.16       ,1.f); //That too
    glm::vec4 thirdColor = glm::vec4(       0.46        ,0.46       ,0.46       ,1.f); //Will be used few times
    glm::vec4 oppositeColor = glm::vec4(    0.86        ,0.86       ,0.86       ,1.f); //Will be used for text / icon color etc.

    ColorPalette(/* args */){}
};

#endif