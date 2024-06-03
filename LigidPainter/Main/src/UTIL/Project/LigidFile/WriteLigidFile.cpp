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

#define WRITE_VEC3(vec, loc)    WRITE_BITS(vec.x, float, loc);\
                                WRITE_BITS(vec.y, float, loc);\
                                WRITE_BITS(vec.z, float, loc);\

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
    
    UTIL::correctFolderDistinguishers(path);

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
    uint32_t versionNumber2700 = 2700;

    WRITE_BITS(versionNumber2700, uint32_t, "Version number");

    // ------------- Date ------------
    time_t currentDate = time(0);
    WRITE_BITS(currentDate, time_t, "Date");

    time_t lastOpenedDate = time(0);
    WRITE_BITS(lastOpenedDate, time_t, "Date");

    // ------------- Current model data ------------
    if(!writeStr(wf, getScene()->model->title))
        return false;

    // ------------- Settings ------------
    // GUI
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

    // Settings
    WRITE_BITS(Settings::properties()->VSync, bool, "Settings::properties()->VSync"); 
    WRITE_BITS(Settings::properties()->cat_allowComments, bool, "Settings::properties()->cat_allowComments"); 
    WRITE_BITS(Settings::properties()->cat_hide, bool, "Settings::properties()->cat_hide"); 
    WRITE_BITS(Settings::properties()->cat_verifyTheExit, bool, "Settings::properties()->cat_verifyTheExit"); 
    WRITE_BITS(Settings::properties()->framebufferResolutionDivider, float, "Settings::properties()->framebufferResolutionDivider"); 
    
    // Scene
    WRITE_BITS(getScene()->aFar, float, "getScene()->aFar"); 
    WRITE_BITS(getScene()->aNear, float, "getScene()->aNear"); 
    WRITE_BITS(getScene()->backfaceCulling, bool, "getScene()->backfaceCulling"); 
    WRITE_BITS(getScene()->fov, float, "getScene()->fov"); 
    WRITE_BITS(getScene()->heightMapStrength, float, "getScene()->heightMapStrength"); 
    WRITE_BITS(getScene()->renderAxisDisplayer, bool, "getScene()->renderAxisDisplayer"); 
    WRITE_BITS(getScene()->renderTiles, bool, "getScene()->renderTiles"); 
    WRITE_BITS(getScene()->useHeightMap, bool, "getScene()->useHeightMap"); 
    WRITE_BITS(getScene()->useOrtho, bool, "getScene()->useOrtho"); 
    WRITE_VEC3(getScene()->transformRotation, "getScene()->transformRotation"); 
    WRITE_VEC3(getScene()->camera.cameraPos, "getScene()->camera.cameraPos"); 
    WRITE_VEC3(getScene()->camera.originPos, "getScene()->originPos"); 
    writeStr(wf, getScene()->skybox.load_path);
    WRITE_VEC3(getScene()->skybox.bgColor, "getScene()->skybox.bgColor");
    WRITE_BITS(getScene()->skybox.opacity, float, "getScene()->skybox.opacity");
    WRITE_BITS(getScene()->skybox.lod, float, "getScene()->skybox.lod");
    WRITE_BITS(getScene()->skybox.rotation, float, "getScene()->skybox.rotation");
    WRITE_BITS(getScene()->skybox.bg_txtr_opacity, float, "getScene()->skybox.bg_txtr_opacity");
    getScene()->skybox.bg_txtr.writePixelData(wf);
    
    // Scenes
    getVectorScene()->write_data(wf);
    getTextureFieldScene()->write_data(wf); 
    
    // Color Settings 
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1, bool, "Enable Albedo Channel");
    WRITE_VEC3(checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.clr1_Btn.color, "Selected color 1");
    WRITE_VEC3(checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.clr2_Btn.color, "Selected color 2");
    WRITE_VEC3(checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.clr3_Btn.color, "Selected color 3");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1, bool, "Enable Roughness Channel");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value, float, "Roughness Channel Value");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1, bool, "Enable Metallic Channel");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value, float, "Metallic Channel Value");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1, bool, "Enable NormalMap Channel");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value, float, "NormalMap Channel Value");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1, bool, "Enable HeightMap Channel");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value, float, "HeightMap Channel Value");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1, bool, "Enable AO Channel");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value, float, "AO Channel Value");
    WRITE_BITS(checkComboList_painting_color.panel.sections[0].elements[13].checkBox.clickState1, bool, "Use custom material");
    FileHandler::writeMaterialData(wf, checkComboList_painting_color.panel.sections[0].elements[14].button.material);
    
    // Face selection settings
    WRITE_BITS(checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1, bool, "Mask to selected mesh");
    WRITE_BITS(checkComboList_mesh_face_selection.panel.sections[0].elements[1].checkBox.clickState1, bool, "Mesh selection mode");
    checkComboList_mesh_face_selection.panel.sections[0].elements[1].button.texture.writeTextureData(wf);

    // Mirror settings
    WRITE_BITS(checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1, bool, "X axis");
    WRITE_BITS(checkComboList_painting_mirror.panel.sections[0].elements[1].rangeBar.value, float, "X axis mirror offset");
    WRITE_BITS(checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1, bool, "Y axis");
    WRITE_BITS(checkComboList_painting_mirror.panel.sections[0].elements[3].rangeBar.value, float, "Y axis mirror offset");
    WRITE_BITS(checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1, bool, "Z axis");
    WRITE_BITS(checkComboList_painting_mirror.panel.sections[0].elements[5].rangeBar.value, float, "Z axis mirror offset");

    // Painting over settings
    WRITE_BITS(checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1, bool, "Use Painting Over");
    WRITE_BITS(checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1, bool, "Painting over edit mode");
    WRITE_BITS(checkComboList_painting_over.panel.sections[0].elements[4].checkBox.clickState1, bool, "Painting over grayscale masking");

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