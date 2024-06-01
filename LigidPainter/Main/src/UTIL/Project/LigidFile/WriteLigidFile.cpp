/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
    LigidPainter's project file writer
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Project/Project.hpp"
#include "UTIL/Project/ProjectUTIL.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

#define WRITE_BITS(var, type, loc)   if(!wf.write(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                                    LGDLOG::start<< "ERROR : Writing ligid file. Failed to write at : " << loc << LGDLOG::end;\
                                                    return false; \
                                                }

static bool writeStr(std::ofstream& wf, std::string str){
    int strSize = str.size(); 
    WRITE_BITS(strSize, int, "String size"); 
    for (size_t i = 0; i < strSize; i++) 
    {
        char c = str[i]; 
        WRITE_BITS(c, char, "String character");
    }
}  

static bool write_element(Element element, std::string element_title){
    if(element.state == 0){
        //WRITE_BITS(element.button.);
    }
    if(element.state == 1){
        //WRITE_BITS(element.rangeBar.value);
    }
}

bool wrtLigidFile(std::string path){
    
    // Open writing stream and truncate the file
    std::ofstream wf = std::ofstream(path, std::ios::out | std::ios::binary);

    if(!wf) {
        LGDLOG::start<< "ERROR! : Writing ligid file." << LGDLOG::end;
        return false;
    }

    // ------------- Description ------------
    uint64_t h1 = 0xBBBBBBBB; 
    uint64_t h2 = 0xCA4B6C78; 
    uint64_t h3 = 0x9A9A2C48; 
    WRITE_BITS(h1, uint64_t, "");
    WRITE_BITS(h2, uint64_t, "");
    WRITE_BITS(h3, uint64_t, "");
    
    // ------------- Version number ------------
    uint32_t versionNumber2600 = 2600;

    WRITE_BITS(versionNumber2600, uint32_t, "Version number");

    // ------------- Date ------------
    time_t currentDate = time(0);
    WRITE_BITS(currentDate, time_t, "Date");

    time_t lastOpenedDate = time(0);
    WRITE_BITS(lastOpenedDate, time_t, "Date");

    // ------------- Current model data ------------
    if(!writeStr(wf, getScene()->model->title))
        return false;

    // ------------- Settings ------------
    WRITE_BITS(panel_library_modes.selectedElement, int, "panel_library_modes.selectedElement"); // Selected library mode on the left
    WRITE_BITS(checkBox_wrap_mode.clickState1, bool, "checkBox_wrap_mode.clickState1"); // Wrap mode checkbox
    WRITE_BITS(panel_displaying_modes.selectedElement, int, "panel_displaying_modes.selectedElement"); // Paint display mode
    WRITE_BITS(button_mesh_selection.selectedMeshI, int, "button_mesh_selection.selectedMeshI"); // Selected mesh
    int layers_resolution = std::stoi(comboBox_layers_resolution.texts[comboBox_layers_resolution.selectedIndex]); 
    WRITE_BITS(layers_resolution, int, "layers_resolution"); // Layers resolution
    WRITE_BITS(comboBox_PBR_displaying_mode.selectedIndex, int, "comboBox_PBR_displaying_mode.selectedIndex"); // Main scene displaying mode
    WRITE_BITS(twoD_painting_mode, bool, "twoD_painting_mode"); // 2D mode or 3D mode
    std::string selected_texture_title = "";
    for (size_t i = 0; i < Library::getTextureArraySize(); i++){if(Library::getTextureObj(i).ID == panel_library_selected_texture.ID)selected_texture_title = Library::getTextureObj(i).title;}
    writeStr(wf, selected_texture_title); // Title of the selected texture

    WRITE_BITS(Settings::properties()->VSync, bool, "Settings::properties()->VSync"); 
    WRITE_BITS(Settings::properties()->cat_allowComments, bool, "Settings::properties()->cat_allowComments"); 
    WRITE_BITS(Settings::properties()->cat_hide, bool, "Settings::properties()->cat_hide"); 
    WRITE_BITS(Settings::properties()->cat_verifyTheExit, bool, "Settings::properties()->cat_verifyTheExit"); 
    WRITE_BITS(Settings::properties()->framebufferResolutionDivider, float, "Settings::properties()->framebufferResolutionDivider"); 
    
    WRITE_BITS(getScene()->aFar, float, "getScene()->aFar"); 
    WRITE_BITS(getScene()->aNear, float, "getScene()->aNear"); 
    WRITE_BITS(getScene()->backfaceCulling, bool, "getScene()->backfaceCulling"); 
    WRITE_BITS(getScene()->fov, float, "getScene()->fov"); 
    WRITE_BITS(getScene()->heightMapStrength, float, "getScene()->heightMapStrength"); 
    WRITE_BITS(getScene()->renderAxisDisplayer, bool, "getScene()->renderAxisDisplayer"); 
    WRITE_BITS(getScene()->renderTiles, bool, "getScene()->renderTiles"); 
    WRITE_BITS(getScene()->useHeightMap, bool, "getScene()->useHeightMap"); 
    WRITE_BITS(getScene()->useOrtho, bool, "getScene()->useOrtho"); 
    WRITE_BITS(getScene()->transformRotation.x, float, "getScene()->transformRotation.x"); 
    WRITE_BITS(getScene()->transformRotation.y, float, "getScene()->transformRotation.y");
    WRITE_BITS(getScene()->transformRotation.z, float, "getScene()->transformRotation.z"); 
    WRITE_BITS(getScene()->camera.cameraPos.x, float, "getScene()->cameraPos.x"); 
    WRITE_BITS(getScene()->camera.cameraPos.y, float, "getScene()->cameraPos.y"); 
    WRITE_BITS(getScene()->camera.cameraPos.z, float, "getScene()->cameraPos.z"); 
    WRITE_BITS(getScene()->camera.originPos.x, float, "getScene()->originPos.x"); 
    WRITE_BITS(getScene()->camera.originPos.y, float, "getScene()->originPos.y"); 
    WRITE_BITS(getScene()->camera.originPos.z, float, "getScene()->originPos.z"); 

    return true;
}

bool projectUTIL_write_ligid_file(std::string path){
    bool res = wrtLigidFile(path);
    if(!res){
        try
        {
            if(std::filesystem::exists(path))
                std::filesystem::remove(path);
            
            std::ofstream outputFile(path);
            
            outputFile.close();
        }
        catch (const std::filesystem::filesystem_error& ex) {
            LGDLOG::start << "ERROR : Filesystem : Recreating ligid file : " << ex.what() << LGDLOG::end;
        }
    }

    return res;
}



//------------ UTIL FUNCTIONS -------------


/*
    Displaying dialog

    Checkcombolists

    VectorScene
    TextureFieldScene

    Mesh selected faces
*/