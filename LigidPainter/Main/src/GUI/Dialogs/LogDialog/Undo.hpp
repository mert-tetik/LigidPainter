
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    TODO : Check multithreading effects

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "MouseSystem/Mouse.hpp" 
#include "LibrarySystem/Library.hpp" 
#include "NodeSystem/Node/Node.hpp" 
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include "GUI/Dialogs/LogDialog/Registering.hpp"

extern std::vector<Action> __actions;

static void undo(){
    if(__actions[__actions.size()-1].ID == TEXTURE_UPDATING_ACTION || __actions[__actions.size()-1].ID == TEXTURE_DELETION_ACTION){
        try
        {
            for (const auto& entry : std::filesystem::directory_iterator(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter/tmp")) {
                if (entry.is_regular_file()) {
                    std::string fileName = entry.path().filename().string();

                    // Check if the file starts with "_history_"
                    if (fileName.find("_history_") == 0) {
                        // Use string stream to split the filename into parts
                        std::istringstream iss(fileName);
                        std::string part;
                        std::getline(iss, part, '_'); // Skip the first part "_history_"
                        std::getline(iss, part, '_'); // Read the first integer value
                        std::getline(iss, part, '_'); // Read the first integer value
                        int indexVal = std::stoi(part);

                        std::getline(iss, part, '_'); // Read the second integer value
                        int IDVal = std::stoi(part);
                        
                        /*
                            TODO
                            Library::getTexture(i)->copyDataToTheCopyContext();
                            projectUpdatingThreadElements.updateTextures = true;
                        */

                        if(indexVal == __actions.size() - 1){
                            if(__actions[__actions.size()-1].ID == TEXTURE_UPDATING_ACTION){
                                for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                                {   
                                    if(Library::getTexture(i)->uniqueId == IDVal){
                                        Library::getTexture(i)->readTMP("_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal));
                                    }
                                }
                            }
                            else if(__actions[__actions.size()-1].ID == TEXTURE_DELETION_ACTION){
                                Texture regeneratedTxtr = Texture(nullptr, 1, 1); 
                                regeneratedTxtr.readTMP("_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal));
                                regeneratedTxtr.title = __actions[__actions.size()-1].texture.title;
                                Library::getTextureVectorPointer()->insert(Library::getTextureVectorPointer()->begin() + __actions[__actions.size()-1].textureIndex, regeneratedTxtr);
                                Library::setChanged(true);
                            }
                        }
                    }
                }
            }
        }
        catch (const std::filesystem::filesystem_error& ex) {
            LGDLOG::start << "ERROR : Filesystem : Location ID 337523 " << ex.what() << LGDLOG::end;
        }
    }

    else if(__actions[__actions.size()-1].ID == TEXTURE_ADDITION_ACTION){
        Library::getTextureVectorPointer()->erase(Library::getTextureVectorPointer()->begin() + __actions[__actions.size()-1].textureIndex);
        Library::setChanged(true);
    }

    else if(__actions[__actions.size()-1].ID == NODE_ACTION){
        *NodeScene::getNodeArrayPointer() = __actions[__actions.size()-1].nodeScene;
        //NodeScene::updateNodeResults();
    }
    else if(__actions[__actions.size()-1].ID == BUTTON_ACTION){
        *__actions[__actions.size()-1].button.button = __actions[__actions.size()-1].button.previousBtn;
        glDeleteTextures(1, &__actions[__actions.size()-1].button.button->texture.ID);
        __actions[__actions.size()-1].button.button->texture = __actions[__actions.size()-1].button.previousBtnTexture;
    }
    
    __actions.pop_back();
}