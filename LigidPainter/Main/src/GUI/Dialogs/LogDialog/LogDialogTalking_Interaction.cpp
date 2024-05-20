
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
    unsigned int face;

    CatMSG(){}
    CatMSG(std::string text, unsigned int face){
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
    return CatMSG("", 0);
}

void LogDialog::talking_interaction(Timer& timer){

    if(getContext()->window.shouldClose()){

        if(Settings::properties()->cat_verifyTheExit){
            if(quitMSG == ""){
                CatMSG msg = pickText(timer, {
                                            CatMSG("Are you sure you want to exit the LigidPainter?", CAT_EMOTION_ROCK),
                                            CatMSG("Do you really want to exit the LigidPainter?", CAT_EMOTION_ROCK),
                                            CatMSG("Are you DETERMINED to close the LigidPainter???", CAT_EMOTION_ROCK),
                                            CatMSG("Are you done with the LigidPainter?  ", CAT_EMOTION_ROCK),
                                            CatMSG("Do you REALLY want to leave me???  ", CAT_EMOTION_CRYING),
                                            CatMSG("Closing the LigidPainter already??  ", CAT_EMOTION_THINKING),
                                            CatMSG("Are you absolutely, positively sure you want to close the app", CAT_EMOTION_ROCK),
                                            CatMSG("One last check: ready to exit and let the LigidPainter nap?", CAT_EMOTION_ROCK),
                                            CatMSG("Leaving so soon?   ", CAT_EMOTION_CRYING),
                                        });

                this->make_cat_talk(timer, msg.text, 1000);
                this->assert_emotion(timer, msg.face, 5);
            }
        }
        else{
            this->windowShouldClose = true;
        }

        return;
    }

    if(petPoints > 300.f){
        petPoints = 0.f;
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Mrrrr :3   ", CAT_EMOTION_RANDOM),
                                        CatMSG("Purrr :3   ", CAT_EMOTION_RANDOM),
                                        CatMSG("Purrr <3   ", CAT_EMOTION_RANDOM),
                                        CatMSG("Meow :3   ", CAT_EMOTION_RANDOM),
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(painting_paint_condition()){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Starting off with style! Keep those colors flowing!", CAT_EMOTION_SMILING),
                                        CatMSG("Looking good! Don't stop now!", CAT_EMOTION_SMILING),
                                        CatMSG("Purr-fect progress! You're on a roll!", CAT_EMOTION_SMILING),
                                        CatMSG("Wow, that's coming together nicely! Keep it up!", CAT_EMOTION_SMILING),
                                        CatMSG("Impressive strokes! This is shaping up beautifully!", CAT_EMOTION_SMILING),
                                        CatMSG("Meow-velous! You're almost there!", CAT_EMOTION_SMILING),
                                        CatMSG("Simply purr-fect! Your art is amazing!", CAT_EMOTION_SMILING),
                                        CatMSG("Absolutely clawsome! Can't wait to see the final touches!", CAT_EMOTION_SMILING),
                                        CatMSG("You're a true artist! Keep painting!", CAT_EMOTION_SMILING),
                                        CatMSG("Looking meow-tastic! Keep those creative juices flowing!", CAT_EMOTION_SMILING),
                                        CatMSG("Purr-fact strokes!", CAT_EMOTION_SMILING),
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(dialog_settings.dialogControl.globalFirstFrameActivated){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Yikes! So technical", CAT_EMOTION_DIZZY),
                                        CatMSG("Gears!! shiver me timbers", CAT_EMOTION_DIZZY),
                                        CatMSG("Here we come gears!", CAT_EMOTION_SMILING),
                                        CatMSG("Don't be scared! It's just a few buttons and switches.", CAT_EMOTION_THINKING),
                                        CatMSG("Let's see what we can get into with these settings!!", CAT_EMOTION_SMILING),
                                        CatMSG("Time to tinker with the cat-figurations!", CAT_EMOTION_SMILING),
                                        CatMSG("Paws and reflect on these settings!", CAT_EMOTION_SMILING),
                                        CatMSG("Meow-stering the art of customization!", CAT_EMOTION_SMILING),
                                        CatMSG("Adjusting settings? Piece of catnip cake!", CAT_EMOTION_SMILING),
                                        CatMSG("Ready to purr-sonalize your experience?", CAT_EMOTION_SMILING),
                                        CatMSG("DON'T YOU DARE TURN ME OFF FROM THERE!!! >:3", CAT_EMOTION_ROCK),
                                        CatMSG("You're not gonna turn me off are u??", CAT_EMOTION_THINKING)
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(dialog_baking.dialogControl.globalFirstFrameActivated){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Bakey bakey!", CAT_EMOTION_RANDOM),
                                        CatMSG("Let's bake those textures!", CAT_EMOTION_RANDOM),
                                        CatMSG("Mixing pixels together for a mouthwatering result!", CAT_EMOTION_RANDOM),
                                        CatMSG("Turning up the heat to cook those textures to perfection!", CAT_EMOTION_RANDOM),
                                        CatMSG("Baking textures - the secret ingredient for a purr-fect render!", CAT_EMOTION_RANDOM),
                                        CatMSG("Just like baking cookies, but for your 3D masterpiece!", CAT_EMOTION_RANDOM),
                                        CatMSG("Baking those pixels like a baker layers cake!", CAT_EMOTION_RANDOM),
                                        CatMSG("Adding some flavor to your project with freshly baked textures!", CAT_EMOTION_RANDOM),
                                        CatMSG("Creating a baking recipe that'll make your project scrumptious!", CAT_EMOTION_RANDOM),
                                        CatMSG("Baking time!", CAT_EMOTION_RANDOM),
                                        CatMSG("The oven's preheated!", CAT_EMOTION_RANDOM),
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(dialog_displayer.dialogControl.globalFirstFrameActivated){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Let's tailor that purr-fect skybox.", CAT_EMOTION_SMILING),
                                        CatMSG("How about a sunrise, or perhaps a cozy sunset?", CAT_EMOTION_RELAXED),
                                        CatMSG("Adjust the sky to match my mood :3", CAT_EMOTION_RELAXED),
                                        CatMSG("Pawsitively purr-fect skies!!", CAT_EMOTION_RELAXED),
                                        CatMSG("Sunrise or sunset? Let's make your skybox a masterpiece of the heavens!", CAT_EMOTION_SMILING),
                                        CatMSG("Ahh the soft, pure sky <3", CAT_EMOTION_RELAXED),
                                        CatMSG("OoOW!! Did you get bored of ur background??", CAT_EMOTION_SMILING),
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(dialog_export.dialogControl.globalFirstFrameActivated){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("You can access your textures via file explorer! No need to export!!", CAT_EMOTION_THINKING),
                                        CatMSG("Time to package these textures!", CAT_EMOTION_SMILING),
                                        CatMSG("Exporting textures with a sprinkle of whisker-dust!", CAT_EMOTION_SMILING),
                                        CatMSG("Preparing your textures for a journey to your computer's catnip stash!", CAT_EMOTION_SMILING),
                                        CatMSG("Sending your textures home!", CAT_EMOTION_SMILING),
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(dialog_newTexture.dialogControl.globalFirstFrameActivated){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Brand-new textures!!", CAT_EMOTION_SMILING),
                                        CatMSG("New textures on the horizon! Let's unleash the artist within!", CAT_EMOTION_SMILING),
                                        CatMSG("Paws to new textures!!", CAT_EMOTION_SMILING),
                                        CatMSG("Don't forget to name your texture!!", CAT_EMOTION_SMILING),
                                        CatMSG("Pick the best color possible :3", CAT_EMOTION_RELAXED),
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(dialog_newProject.dialogControl.globalFirstFrameActivated){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("New project on the horizon! Let's unleash the artist within!", CAT_EMOTION_SMILING),
                                        CatMSG("Don't forget to name your project!!", CAT_EMOTION_SMILING),
                                        CatMSG("New project new adventure!!", CAT_EMOTION_SMILING),
                                        CatMSG("A project is a folder that held inside of a folder. Meownd-blowing!", CAT_EMOTION_SMILING),
                                        CatMSG("Time to embark on a new project, whiskers ready!", CAT_EMOTION_SMILING),
                                        CatMSG("Starting a new project, let's paw-tner up for some creative fun!", CAT_EMOTION_SMILING),
                                        CatMSG("Crafting a new project with the finesse and curiosity of a cat!", CAT_EMOTION_SMILING),
                                        CatMSG("I had a friend named 'project' once. Hope he is doing well now :3", CAT_EMOTION_THINKING),
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
    else if(dialog_textureEditor.dialogControl.globalFirstFrameActivated){
        if(catMSG == ""){
            CatMSG msg = pickText(timer, {
                                        CatMSG("Time to give those pixels a cat-tastic makeover!", CAT_EMOTION_SMILING),
                                        CatMSG("Meow-difying images!", CAT_EMOTION_SMILING),
                                        CatMSG("Pawsitively purr-fect editing!", CAT_EMOTION_SMILING),
                                        CatMSG("Whisking images into shape!", CAT_EMOTION_SMILING),
                                        CatMSG("Catnip for your images! Let's create something paw-some :3", CAT_EMOTION_SMILING),
                                        CatMSG("Tweaking pixels with whisker-precision!", CAT_EMOTION_SMILING),
                                        CatMSG("Meow-gical image transformations!", CAT_EMOTION_SMILING),
                                        CatMSG("Crafting pixel into purr-fection!", CAT_EMOTION_SMILING)
                                    });

            this->make_cat_talk(timer, msg.text, 5);
            this->assert_emotion(timer, msg.face, 5);
        }
    }
}