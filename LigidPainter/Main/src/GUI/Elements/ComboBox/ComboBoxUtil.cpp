/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

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
    shader.setVec3("pos"    ,     resultPos );
    shader.setVec2("scale"  ,     resultScale);
    
    
    if(pressed)//If button is pressed
        shader.setVec4("properties.color"  ,     aColor * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
    else
        shader.setVec4("properties.color"  ,     aColor     ); //Default button color
    

    shader.setVec4("properties.color2"  ,     color2     ); //Second color that is used by hover or click animations
    

    shader.setFloat("properties.colorMixVal"  ,     (aClickedMixVal + aHoverMixVal)/2.f   );


    //Properties
    shader.setFloat("properties.radius",     resultRadius    );


    if(outline)
        shader.setInt("properties.outline.state" ,     1      ); 
    else
        shader.setInt("properties.outline.state" ,    0      ); 


    //Outline extra color (affected by the colorMixVal)
    shader.setVec3("properties.outline.color" ,     outlineColor     );  


    shader.setVec3("properties.outline.color2" ,     outlineColor2     );   


    shader.setFloat("properties.outline.thickness" ,    resultOutlineThickness + aClickedMixVal*4.f ); 
    
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}