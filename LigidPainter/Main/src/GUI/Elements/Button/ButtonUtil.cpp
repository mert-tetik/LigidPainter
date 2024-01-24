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
#include <glm/gtx/string_cast.hpp>

#include "GUI/Elements/Elements.hpp"
#include "ShaderSystem/Shader.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>

//forward declarations of the util functions
glm::vec2 getTextureScale(Texture texture, glm::vec2 resultScale, float textureSizeScale);


/// @brief renders the button using buttonShader
/// @param resultPos 
/// @param glm::abs(resultScale) 
/// @param resultRadius 
/// @param resultOutlineThickness 
void Button::render(
                        glm::vec3 resultPos,
                        glm::vec2 resultScale,
                        float resultRadius,
                        float resultOutlineThickness
                    ){
    
    //Set the transform data (used by vertex shader)
    ShaderSystem::buttonShader().setVec3("pos"    ,     resultPos );
    ShaderSystem::buttonShader().setVec2("scale"  ,     glm::abs(resultScale));
    

   
    
    //Render
    LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Button::render");
}




/// @brief renders the texture of the button
/// @param resultPos 
/// @param glm::abs(resultScale) 
/// @param resultScaleText 
/// @param videoScale 
/// @param textRenderer 
/// @param textureRadius 
/// @return returns the renderthetext, If returns yes render the text, If not don't render the text
bool Button::renderTheTexture(
                                glm::vec3 resultPos,
                                glm::vec2 resultScale,
                                float resultScaleText,
                                float &textureRadius
                            ){
        
    ShaderSystem::buttonShader().setVec4("properties.color"  ,     glm::vec4(0)     );
    ShaderSystem::buttonShader().setVec4("properties.color2"  ,     glm::vec4(0)     );


    //Render the texture
    bool renderTheText = true;
    if(texture.ID != 0){
        
        //Get the scale value of the texture
        glm::vec2 resultScaleTexture = getTextureScale(texture,glm::abs(resultScale),textureSizeScale);
        
        //Give the radius value (used to calculate the distance between text and the texture)
        textureRadius = resultScaleTexture.x;
        
        //Position of the texture
        glm::vec3 resultPosTexture = resultPos;
        
        //Gets the texture on top of the button instead of the center if textureStickToTop button property bool variable set to true
        if(textureStickToTop)
            resultPosTexture.y = resultPosTexture.y - glm::abs(resultScale).y + resultScaleTexture.y;

        //Get the location of where the text is started (text renderer manipulates the position of the text for the alignment)
        glm::vec3 textStartPos = textRenderer.positionTheText();
        
        //If there is no text get the texture into the middle
        if(text == ""){
            resultPosTexture.x = textStartPos.x; 
        }
        
        //If there is text get the texture left side of the button
        else{
            resultPosTexture.x = resultPos.x - glm::abs(resultScale).x + resultScaleTexture.x * 2.f; 
        }
        
        //If the button is small enough get the texture into middle and don't render the text
        if(textStartPos.x - resultScaleTexture.x * 3.f < resultPos.x - glm::abs(resultScale).x){
            resultPosTexture.x = resultPos.x; //Button's position
            renderTheText = false;
        }

        //Take texture to the front of the button
        resultPosTexture.z += 0.02f; 

        //Tell the button shader to render the texture
        ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     1    );
        ShaderSystem::buttonShader().setInt("properties.txtr"  ,     0    );

        if(this->stretchTexture){
            resultPosTexture.x = this->resultPos.x;
            resultPosTexture.y = this->resultPos.y;
            resultScaleTexture = glm::abs(this->resultScale);
        }

        //Set transform data of the texture
        ShaderSystem::buttonShader().setVec3("pos"    ,     resultPosTexture);
        ShaderSystem::buttonShader().setVec2("scale"  ,     resultScaleTexture * glm::sign(resultScale));
        
        //Bind the texture to the slot 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture.ID);
        
        //Draw the texture
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Button::renderTheTexture");
        
        //Tell the button shader to rendering the texture is done
        ShaderSystem::buttonShader().setInt("states.renderTexture"  ,     0    );
    }

    return renderTheText;
}



/// @brief manages the activites regarding the mouse like mouse hover, mouse clicked to the button etc.
/// @param mouse  
void Button::manageMouseActivity(
                                    
                                ){
    //Check if mouse on top of the button
    if(doMouseTracking && !locked)
        hover = Mouse::isMouseHover(glm::abs(resultScale),glm::vec2(resultPos.x,resultPos.y));
    else
        hover = false;

    //Set the cursor as pointer if cursor on button
    if(hover)
        Mouse::setCursor(*Mouse::pointerCursor()); //mouse.activeCursor = *Mouse::pointerCursor()
    
    //Mouse left button pressed on top of the button
    if(hover && *Mouse::LClick()){
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
    if(!*Mouse::LPressed() && !keepPressingState){
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
                                    float resultScaleText
                                ){
    
    //Load the text data
    if(textAlignLeft)
        textRenderer.loadTextData(  
                                    text,
                                    glm::vec3(resultPos.x - glm::abs(resultScale).x,resultPos.y,resultPos.z),
                                    false,
                                    resultScaleText,
                                    resultPos.x - glm::abs(resultScale).x,
                                    resultPos.x + glm::abs(resultScale).x,
                                    TEXTRENDERER_ALIGNMENT_LEFT
                                  );
    else
        textRenderer.loadTextData(  
                                    text,
                                    glm::vec3(resultPos.x,resultPos.y,resultPos.z),
                                    false,
                                    resultScaleText,
                                    resultPos.x - glm::abs(resultScale).x,
                                    resultPos.x + glm::abs(resultScale).x,
                                    TEXTRENDERER_ALIGNMENT_MID
                                  );
    
    //Render the texture
    float textureRadius = 0.f;
    bool renderTheText = renderTheTexture(resultPos, resultScale, resultScaleText,textureRadius);
    
    //Render the text
    if(renderTheText){
        ShaderSystem::buttonShader().setVec4("properties.color"  ,     textColor     );
        ShaderSystem::buttonShader().setVec4("properties.color2"  ,     textColor2     );
        
        textRenderer.renderText();
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

    glm::vec2 resultScaleTexture = glm::vec2(std::min(glm::abs(resultScale).x,glm::abs(resultScale).y)) / glm::vec2(textureSizeScale);
    return resultScaleTexture /= txtrResDivider;
} 