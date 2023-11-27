
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


void LogDialog::undo(Painter& painter){
    
    if(this->activeHistoryMode == HISTORY_VECTORS_MODE && actions_Vectors.size()){
        painter.vectorStrokes = actions_Vectors[actions_Vectors.size() - 1].vectorStrokes;
        actions_Vectors.pop_back();
    }
    
    if(actions_Library[actions_Library.size()-1].ID == TEXTURE_UPDATING_ACTION || actions_Library[actions_Library.size()-1].ID == TEXTURE_DELETION_ACTION){
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

                        if(indexVal == actions_Library.size() - 1){
                            if(actions_Library[actions_Library.size()-1].ID == TEXTURE_UPDATING_ACTION){
                                for (size_t i = 0; i < Library::getTextureArraySize(); i++)
                                {   
                                    if(Library::getTexture(i)->uniqueId == IDVal){
                                        Library::getTexture(i)->readTMP("_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal));
                                    }
                                }
                            }
                            else if(actions_Library[actions_Library.size()-1].ID == TEXTURE_DELETION_ACTION){
                                Texture regeneratedTxtr = Texture(nullptr, 1, 1); 
                                regeneratedTxtr.readTMP("_history_" + std::to_string(indexVal) + "_" + std::to_string(IDVal));
                                regeneratedTxtr.title = actions_Library[actions_Library.size()-1].texture.title;
                                Library::getTextureVectorPointer()->insert(Library::getTextureVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex, regeneratedTxtr);
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

    else if(actions_Library[actions_Library.size()-1].ID == TEXTURE_ADDITION_ACTION){
        Library::getTextureVectorPointer()->erase(Library::getTextureVectorPointer()->begin() + actions_Library[actions_Library.size()-1].textureIndex);
        Library::setChanged(true);
    }

    actions_Library.pop_back();
}