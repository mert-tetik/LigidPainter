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

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Elements/Elements.hpp"
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>


/// @brief Render the checkbox circle
/// @param resultPos position in screen coordinates
/// @param resultScale scale in screen coordinates
/// @param resultRadius radius in screen coordinates
/// @param resultOutlineThickness circle thickness in screen coordinates 
void CheckBox::render(
                        glm::vec3 resultPos,
                        glm::vec2 resultScale,
                        float resultRadius,
                        float resultOutlineThickness
                    )
{
    
    //Set the transform data (used by vertex shader)
    ShaderSystem::buttonShader().setVec3("pos"    ,     resultPos );
    ShaderSystem::buttonShader().setVec2("scale"  ,     resultScale);
    
    
    if(clickState1)//If button is pressed
        ShaderSystem::buttonShader().setVec4("properties.color"  ,     color * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
    else
        ShaderSystem::buttonShader().setVec4("properties.color"  ,     color     ); //Default button color
    
    
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     color2     ); //Second color that is used by hover or click animations
    
    
    if(animationStyle == 1) //If hover or clicked change the color of the button
        ShaderSystem::buttonShader().setFloat("properties.colorMixVal"  ,     (clickedMixVal + hoverMixVal)/2.f   );
    else //If clicked change the color of the button
        ShaderSystem::buttonShader().setFloat("properties.colorMixVal"  ,     (clickedMixVal)   );


    ShaderSystem::buttonShader().setFloat("properties.radius",     resultRadius    );


    ShaderSystem::buttonShader().setInt("properties.outline.state" ,     1      ); 


    ShaderSystem::buttonShader().setVec3("properties.outline.color" ,     outlineColor     );  


    ShaderSystem::buttonShader().setVec3("properties.outline.color2" ,     outlineColor2     );   


    if(animationStyle == 0) //Increase the thicness of the button if hover
        ShaderSystem::buttonShader().setFloat("properties.outline.thickness" ,    resultOutlineThickness + clickedMixVal*4.f ); 
    else  //Set the thickness value of the button
        ShaderSystem::buttonShader().setFloat("properties.outline.thickness" ,    resultOutlineThickness); 


    glDrawArrays(GL_TRIANGLES, 0, 6);
}



/// @brief manages the activites regarding the mouse like mouse hover, mouse clicked to the button etc.
void CheckBox::manageMouseActivity  (
                                        
                                        glm::vec2 resultScale, 
                                        glm::vec3 resultPos
                                    )
{
    //Check if hover
    if(doMouseTracking)
        hover = Mouse::isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else 
        hover = false;
    
    //Stuff
    if(hover)
        //Set the cursor as pointer
        Mouse::setCursor(*Mouse::pointerCursor());// mouse.activeCursor = *Mouse::pointerCursor()
    if(hover && *Mouse::LClick()){
        //Mouse left button pressed on top of the button
        clickState1 = !clickState1;
    }
}