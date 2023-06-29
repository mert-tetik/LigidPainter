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
    shader.setVec3("pos"    ,     resultPos );
    shader.setVec2("scale"  ,     resultScale);
    
    
    if(clickState1)//If button is pressed
        shader.setVec4("properties.color"  ,     color * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
    else
        shader.setVec4("properties.color"  ,     color     ); //Default button color
    
    
    shader.setVec4("properties.color2"  ,     color2     ); //Second color that is used by hover or click animations
    
    
    if(animationStyle == 1) //If hover or clicked change the color of the button
        shader.setFloat("properties.colorMixVal"  ,     (clickedMixVal + hoverMixVal)/2.f   );
    else //If clicked change the color of the button
        shader.setFloat("properties.colorMixVal"  ,     (clickedMixVal)   );


    shader.setFloat("properties.radius",     resultRadius    );


    shader.setInt("properties.outline.state" ,     1      ); 


    shader.setVec3("properties.outline.color" ,     outlineColor     );  


    shader.setVec3("properties.outline.color2" ,     outlineColor2     );   


    if(animationStyle == 0) //Increase the thicness of the button if hover
        shader.setFloat("properties.outline.thickness" ,    resultOutlineThickness + clickedMixVal*4.f ); 
    else  //Set the thickness value of the button
        shader.setFloat("properties.outline.thickness" ,    resultOutlineThickness); 


    glDrawArrays(GL_TRIANGLES, 0, 6);
}



/// @brief manages the activites regarding the mouse like mouse hover, mouse clicked to the button etc.
void CheckBox::manageMouseActivity  (
                                        Mouse &mouse, 
                                        glm::vec2 resultScale, 
                                        glm::vec3 resultPos
                                    )
{
    //Check if hover
    if(doMouseTracking)
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else 
        hover = false;
    
    //Stuff
    if(hover)
        //Set the cursor as pointer
        mouse.setCursor(mouse.pointerCursor);// mouse.activeCursor = mouse.pointerCursor
    if(hover && mouse.LClick){
        //Mouse left button pressed on top of the button
        clickState1 = !clickState1;
    }
}