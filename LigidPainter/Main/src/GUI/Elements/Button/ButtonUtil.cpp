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
#include <glm/gtx/string_cast.hpp>

#include "GUI/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

//Front declarations of the util functions
glm::vec2 getTextureScale(Texture texture, glm::vec2 resultScale, float textureSizeScale);


/// @brief renders the button using buttonShader
/// @param resultPos 
/// @param resultScale 
/// @param resultRadius 
/// @param resultOutlineThickness 
void Button::render(
                        glm::vec3 resultPos,
                        glm::vec2 resultScale,
                        float resultRadius,
                        float resultOutlineThickness
                    ){
    
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
    else if(animationStyle == 2) //If hover or clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (hover/2.f)   );
    else //If clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (clickedMixVal)   );


    shader.setFloat("radius",     resultRadius    );


    if(outline)
        shader.setInt("outlineState" ,     1      ); 
    else if(outlineExtra)
        shader.setInt("outlineState" ,     2      ); 
    else
        shader.setInt("outlineState" ,    0      ); 
    

    //Outline extra color (affected by the colorMixVal)
    shader.setVec3("outlineColor" ,     outlineColor     );  
    
    
    shader.setVec3("outlineColor2" ,     outlineColor2     );   


    if(animationStyle == 0) //Increase the thicness of the button if hover
        shader.setFloat("thickness" ,    resultOutlineThickness + hoverMixVal*2.f ); 
    else if(animationStyle == 2)
        shader.setFloat("thickness" ,    resultOutlineThickness + clickedMixVal*2.f ); 
    else  //Set the thickness value of the button
        shader.setFloat("thickness" ,    resultOutlineThickness); 
   
    
    //Render
    glDrawArrays(GL_TRIANGLES, 0, 6);
}




/// @brief renders the texture of the button
/// @param resultPos 
/// @param resultScale 
/// @param resultScaleText 
/// @param videoScale 
/// @param textRenderer 
/// @param textureRadius 
/// @return returns the renderthetext, If returns yes render the text, If not don't render the text
bool Button::renderTheTexture(
                                glm::vec3 resultPos,
                                glm::vec2 resultScale,
                                float resultScaleText,
                                glm::vec2 videoScale,
                                TextRenderer &textRenderer,
                                float &textureRadius
                            ){
        
    shader.setVec4("color"  ,     glm::vec4(0)     );
    shader.setVec4("color2"  ,     glm::vec4(0)     );


    //Render the texture
    bool renderTheText = true;
    if(texture.ID != 0){
        
        //Get the scale value of the texture
        glm::vec2 resultScaleTexture = getTextureScale(texture,resultScale,textureSizeScale);
        
        //Give the radius value (used to calculate the distance between text and the texture)
        textureRadius = resultScaleTexture.x;
        
        //Position of the texture
        glm::vec3 resultPosTexture = resultPos;
        
        //Gets the texture on top of the button instead of the center if textureStickToTop button property bool variable set to true
        if(textureStickToTop)
            resultPosTexture.y = resultPosTexture.y - resultScale.y + resultScaleTexture.y;

        //Get the location of where the text is started (text renderer manipulates the position of the text for the alignment)
        glm::vec3 textStartPos = textRenderer.positionTheText();
        
        //If there is no text get the texture into the middle
        if(text == ""){
            resultPosTexture.x = textStartPos.x; 
        }
        
        //If there is text get the texture left side of the button
        else{
            resultPosTexture.x = resultPos.x - resultScale.x + resultScaleTexture.x * 2.f; 
        }
        
        //If the button is small enough get the texture into middle and don't render the text
        if(textStartPos.x - resultScaleTexture.x * 3.f < resultPos.x - resultScale.x){
            resultPosTexture.x = resultPos.x; //Button's position
            renderTheText = false;
        }

        //Take texture to the front of the button
        resultPosTexture.z += 0.02f; 

        //Tell the button shader to render the texture
        shader.setInt("renderTexture"  ,     1    );
        shader.setInt("txtr"  ,     0    );

        //Set transform data of the texture
        shader.setVec3("pos"    ,     resultPosTexture);
        shader.setVec2("scale"  ,     resultScaleTexture);
        
        //Bind the texture to the slot 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture.ID);
        
        //Draw the texture
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Tell the button shader to rendering the texture is done
        shader.setInt("renderTexture"  ,     0    );
    }

    return renderTheText;
}



/// @brief manages the activites regarding the mouse like mouse hover, mouse clicked to the button etc.
/// @param mouse  
void Button::manageMouseActivity(
                                    Mouse &mouse
                                ){
    //Check if mouse on top of the button
    if(doMouseTracking)
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else
        hover = false;

    //Set the cursor as pointer if cursor on button
    if(hover)
        mouse.setCursor(mouse.pointerCursor); //mouse.activeCursor = mouse.pointerCursor
    
    //Mouse left button pressed on top of the button
    if(hover && mouse.LClick){
        //(if release on top of the button, the button will be clicked)
        if(keepPressingState)
            clickState1 = !clickState1;
        else
            clickState1 = true;
    }

    if(clicked)
        clicked = false;

    //Set the clickedMixVal to 1.f if button is pressed and released while the cursor is on top of the button
    //(Than the clickedMixVal will be set to 0 gradually by the click animation managment)
    if(!mouse.LPressed && !keepPressingState){
        //If clicked to the button
        if(clickState1 && hover){
            //Clicked
            clickedMixVal = 1.f;
            clicked = true;
        }
        clickState1 = false;
    }
}



/// @brief renders the text and the texture
/// @param textRenderer local TextRenderer class 
/// @param videoScale resolution of the monitor
/// @param resultScaleText scale value of the text in screen coordinates
void Button::renderTextAndTexture(
                                    TextRenderer &textRenderer, 
                                    glm::vec2 videoScale, 
                                    float resultScaleText
                                ){
    
    //Load the text data
    if(textAlignLeft)
        textRenderer.loadTextData(  
                                    shader,
                                    text,
                                    glm::vec3(resultPos.x - resultScale.x,resultPos.y,resultPos.z),
                                    false,
                                    resultScaleText,
                                    resultPos.x - resultScale.x,
                                    resultPos.x + resultScale.x,
                                    TEXTRENDERER_ALIGNMENT_LEFT
                                  );
    else
        textRenderer.loadTextData(  
                                    shader,
                                    text,
                                    glm::vec3(resultPos.x,resultPos.y,resultPos.z),
                                    false,
                                    resultScaleText,
                                    resultPos.x - resultScale.x,
                                    resultPos.x + resultScale.x,
                                    TEXTRENDERER_ALIGNMENT_MID
                                  );
    
    //Render the texture
    float textureRadius = 0.f;
    bool renderTheText = renderTheTexture(resultPos,resultScale,resultScaleText,videoScale,textRenderer,textureRadius);
    
    //Render the text
    if(renderTheText){
        shader.setVec4("color"  ,     textColor     );
        shader.setVec4("color2"  ,     textColor2     );
        
        textRenderer.renderText(shader);
    }
}









///-------- UTILS --------------

glm::vec2 getTextureScale(Texture texture, glm::vec2 resultScale, float textureSizeScale){
    //Divides the scale value of the texture to keep the ratio
    glm::vec2 txtrResDivider = glm::vec2(1);
    if(texture.getResolution().x > texture.getResolution().y){
        txtrResDivider.y = (float)texture.getResolution().x / (float)texture.getResolution().y;
    }
    else{
        txtrResDivider.x = (float)texture.getResolution().y / (float)texture.getResolution().x;
    }

    glm::vec2 resultScaleTexture = glm::vec2(std::min(resultScale.x,resultScale.y)) / glm::vec2(textureSizeScale);
    return resultScaleTexture /= txtrResDivider;
} 