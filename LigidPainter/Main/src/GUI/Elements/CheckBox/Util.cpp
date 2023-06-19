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
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

void CheckBox::render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness){
    
    //Set the transform data (used by vertex shader)
    shader.setVec3("pos"    ,     resultPos );
    shader.setVec2("scale"  ,     resultScale);
    
    if(clickState1)//If button is pressed
        shader.setVec4("color"  ,     color * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
    else
        shader.setVec4("color"  ,     color     ); //Default button color
    
    shader.setVec4("color2"  ,     color2     ); //Second color that is used by hover or click animations
    
    if(animationStyle == 1) //If hover or clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (clickedMixVal + hoverMixVal)/2.f   );
    else //If clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (clickedMixVal)   );
    //Properties
    shader.setFloat("radius",     resultRadius    );
    shader.setInt("outlineState" ,     1      ); 
    //Outline extra color (affected by the colorMixVal)
    shader.setVec3("outlineColor" ,     outlineColor     );  
    shader.setVec3("outlineColor2" ,     outlineColor2     );   
    if(animationStyle == 0) //Increase the thicness of the button if hover
        shader.setFloat("thickness" ,    resultOutlineThickness + clickedMixVal*4.f ); 
    else  //Set the thickness value of the button
        shader.setFloat("thickness" ,    resultOutlineThickness); 
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}