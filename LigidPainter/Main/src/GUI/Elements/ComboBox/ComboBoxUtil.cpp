/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements/Elements.hpp"
#include "ShaderSystem/Shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

void ComboBox::render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness,float aClickedMixVal,float aHoverMixVal,
                        bool outline,glm::vec4 aColor){
    
    //Set the transform data (used by vertex shader)
    ShaderSystem::buttonShader().setVec3("pos"    ,     resultPos );
    ShaderSystem::buttonShader().setVec2("scale"  ,     resultScale);
    
    
    if(pressed)//If button is pressed
        ShaderSystem::buttonShader().setVec4("properties.color"  ,     aColor * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
    else
        ShaderSystem::buttonShader().setVec4("properties.color"  ,     aColor     ); //Default button color
    

    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     color2     ); //Second color that is used by hover or click animations
    

    ShaderSystem::buttonShader().setFloat("properties.colorMixVal"  ,     (aClickedMixVal + aHoverMixVal)/2.f   );


    //Properties
    ShaderSystem::buttonShader().setFloat("properties.radius",     resultRadius    );


    if(outline)
        ShaderSystem::buttonShader().setInt("properties.outline.state" ,     1      ); 
    else
        ShaderSystem::buttonShader().setInt("properties.outline.state" ,    0      ); 


    //Outline extra color (affected by the colorMixVal)
    ShaderSystem::buttonShader().setVec3("properties.outline.color" ,     outlineColor     );  


    ShaderSystem::buttonShader().setVec3("properties.outline.color2" ,     outlineColor2     );   


    ShaderSystem::buttonShader().setFloat("properties.outline.thickness" ,    resultOutlineThickness + aClickedMixVal*4.f ); 
    
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}