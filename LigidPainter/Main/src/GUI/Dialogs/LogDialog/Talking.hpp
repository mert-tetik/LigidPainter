
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "3D/ThreeD.hpp" 

#include "GUI/GUI.hpp" 

#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/Library/Library.hpp" 
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp" 
#include "UTIL/Painting/Painter.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

struct CatMSG{
    std::string text;
    Texture face;

    CatMSG(){}
    CatMSG(std::string text, Texture face){
        this->text = text;
        this->face = face;
    }
};

CatMSG pickText(Timer &timer ,std::vector<CatMSG> texts){
    for (size_t i = 0; i < texts.size(); i++)
    {
        if(timer.seconds % texts.size() == i)
            return texts[i];
    }
    return CatMSG("", Texture());
}

Button messageInfoBtn;
bool messageInfoActive = false;
float messageInfoBtnMixVal = 0.f;
size_t messageInfoBtnStartTime = 0.f;

extern std::string catMSG = "";
extern float petPoints = 0.f; 
extern Texture msgFace;

void LogDialog::talking(Timer& timer){
    if(petPoints > 300.f){
        petPoints = 0.f;
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Mrrrr :3   ", appTextures.mascotCat_bread),
                                        CatMSG("Purrr :3   ", appTextures.mascotCat_relaxed),
                                        CatMSG("Purrr <3   ", appTextures.mascotCat_bread),
                                        CatMSG("Meow :3   ", appTextures.mascotCat_relaxed),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        
        messageInfoBtn.text = catMSG;
    }
    else if(painting_paint_condition()){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Starting off with style! Keep those colors flowing!", appTextures.mascotCat_smile),
                                        CatMSG("Looking good! Don't stop now!", appTextures.mascotCat_smile),
                                        CatMSG("Purr-fect progress! You're on a roll!", appTextures.mascotCat_smile),
                                        CatMSG("Wow, that's coming together nicely! Keep it up!", appTextures.mascotCat_smile),
                                        CatMSG("Impressive strokes! This is shaping up beautifully!", appTextures.mascotCat_smile),
                                        CatMSG("Meow-velous! You're almost there!", appTextures.mascotCat_smile),
                                        CatMSG("Simply purr-fect! Your art is amazing!", appTextures.mascotCat_smile),
                                        CatMSG("Absolutely clawsome! Can't wait to see the final touches!", appTextures.mascotCat_smile),
                                        CatMSG("You're a true artist! Keep painting!", appTextures.mascotCat_smile),
                                        CatMSG("Looking meow-tastic! Keep those creative juices flowing!", appTextures.mascotCat_smile),
                                        CatMSG("Purr-fact strokes!", appTextures.mascotCat_smile),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
    else if(dialog_settings.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Yikes! So technical", appTextures.mascotCat_dizzy),
                                        CatMSG("Gears!! shiver me timbers", appTextures.mascotCat_dizzy),
                                        CatMSG("Here we come gears!", appTextures.mascotCat_smile),
                                        CatMSG("Don't be scared! It's just a few buttons and switches.", appTextures.mascotCat_thinking),
                                        CatMSG("Let's see what we can get into with these settings!!", appTextures.mascotCat_smile),
                                        CatMSG("Time to tinker with the cat-figurations!", appTextures.mascotCat_smile),
                                        CatMSG("Paws and reflect on these settings!", appTextures.mascotCat_smile),
                                        CatMSG("Meow-stering the art of customization!", appTextures.mascotCat_smile),
                                        CatMSG("Adjusting settings? Piece of catnip cake!", appTextures.mascotCat_smile),
                                        CatMSG("Ready to purr-sonalize your experience?", appTextures.mascotCat_smile),
                                        CatMSG("DON'T YOU DARE TURN ME OFF FROM THERE!!! >:3", appTextures.mascotCat_rock),
                                        CatMSG("You're not gonna turn me off are u??", appTextures.mascotCat_thinking),
                                        CatMSG("Pweasee don't turn me off. It's so dark here :3", appTextures.mascotCat_crying),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
    else if(dialog_baking.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Bakey bakey!", appTextures.mascotCat_bread),
                                        CatMSG("Let's bake those textures!", appTextures.mascotCat_bread),
                                        CatMSG("Mixing pixels together for a mouthwatering result!", appTextures.mascotCat_bread),
                                        CatMSG("Turning up the heat to cook those textures to perfection!", appTextures.mascotCat_bread),
                                        CatMSG("Baking textures - the secret ingredient for a purr-fect render!", appTextures.mascotCat_bread),
                                        CatMSG("Just like baking cookies, but for your 3D masterpiece!", appTextures.mascotCat_bread),
                                        CatMSG("Baking those pixels like a baker layers cake!", appTextures.mascotCat_bread),
                                        CatMSG("Adding some flavor to your project with freshly baked textures!", appTextures.mascotCat_bread),
                                        CatMSG("Creating a baking recipe that'll make your project scrumptious!", appTextures.mascotCat_bread),
                                        CatMSG("Baking time!", appTextures.mascotCat_bread),
                                        CatMSG("The oven's preheated!", appTextures.mascotCat_bread),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
    else if(dialog_displayer.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Let's tailor that purr-fect skybox.", appTextures.mascotCat_smile),
                                        CatMSG("How about a sunrise, or perhaps a cozy sunset?", appTextures.mascotCat_relaxed),
                                        CatMSG("Adjust the sky to match my mood :3", appTextures.mascotCat_relaxed),
                                        CatMSG("Pawsitively purr-fect skies!!", appTextures.mascotCat_relaxed),
                                        CatMSG("Sunrise or sunset? Let's make your skybox a masterpiece of the heavens!", appTextures.mascotCat_smile),
                                        CatMSG("Ahh the soft, pure sky <3", appTextures.mascotCat_relaxed),
                                        CatMSG("OoOW!! Did you get bored of ur background??", appTextures.mascotCat_smile),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
    else if(dialog_export.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("You can access your textures via file explorer! No need to export!!", appTextures.mascotCat_thinking),
                                        CatMSG("Time to package these textures!", appTextures.mascotCat_smile),
                                        CatMSG("Exporting textures with a sprinkle of whisker-dust!", appTextures.mascotCat_smile),
                                        CatMSG("Preparing your textures for a journey to your computer's catnip stash!", appTextures.mascotCat_smile),
                                        CatMSG("Sending your textures home!", appTextures.mascotCat_smile),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
    else if(dialog_newTexture.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Brand-new textures!!", appTextures.mascotCat_smile),
                                        CatMSG("New textures on the horizon! Let's unleash the artist within!", appTextures.mascotCat_smile),
                                        CatMSG("Paws to new textures!!", appTextures.mascotCat_smile),
                                        CatMSG("Don't forget to name your texture!!", appTextures.mascotCat_smile),
                                        CatMSG("Pick the best color possible :3", appTextures.mascotCat_relaxed),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
    else if(dialog_newProject.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("New project on the horizon! Let's unleash the artist within!", appTextures.mascotCat_smile),
                                        CatMSG("Don't forget to name your project!!", appTextures.mascotCat_smile),
                                        CatMSG("New project new adventure!!", appTextures.mascotCat_smile),
                                        CatMSG("A project is a folder that held inside of a folder. Meownd-blowing!", appTextures.mascotCat_smile),
                                        CatMSG("Time to embark on a new project, whiskers ready!", appTextures.mascotCat_smile),
                                        CatMSG("Starting a new project, let's paw-tner up for some creative fun!", appTextures.mascotCat_smile),
                                        CatMSG("Crafting a new project with the finesse and curiosity of a cat!", appTextures.mascotCat_smile),
                                        CatMSG("I had a friend named 'project' once. Hope he is doing well now :3", appTextures.mascotCat_thinking),
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
    else if(dialog_textureEditor.dialogControl.globalFirstFrameActivated){
        messageInfoActive = true;
        messageInfoBtnStartTime = timer.seconds;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Time to give those pixels a cat-tastic makeover!", appTextures.mascotCat_smile),
                                        CatMSG("Meow-difying images!", appTextures.mascotCat_smile),
                                        CatMSG("Pawsitively purr-fect editing!", appTextures.mascotCat_smile),
                                        CatMSG("Whisking images into shape!", appTextures.mascotCat_smile),
                                        CatMSG("Catnip for your images! Let's create something paw-some :3", appTextures.mascotCat_smile),
                                        CatMSG("Tweaking pixels with whisker-precision!", appTextures.mascotCat_smile),
                                        CatMSG("Meow-gical image transformations!", appTextures.mascotCat_smile),
                                        CatMSG("Crafting pixel into purr-fection!", appTextures.mascotCat_smile)
                                    });

            catMSG = msg.text;
            msgFace = msg.face;
        }
        messageInfoBtn.text = catMSG;
    }
}