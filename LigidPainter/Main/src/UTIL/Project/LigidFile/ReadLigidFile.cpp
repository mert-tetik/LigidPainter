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

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
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

#define READ_BITS(var, type, loc)    if(!rf.read(reinterpret_cast<char*>(   &var     ), sizeof(type))){ \
                                                    LGDLOG::start<< "ERROR : Reading ligid file. Failed to read at : " << loc << LGDLOG::end;\
                                                    return false; \
                                                }

#define READ_STR(str)   {\
                            int strSize; \
                            READ_BITS(strSize, int, "Sring size") \
                            str.clear(); \
                            for (size_t i = 0; i < strSize; i++) \
                            {\
                                char c; \
                                READ_BITS(c, char, "String char") \
                                str.push_back(c); \
                            }\
                        }

#define READ_VEC3(vec, loc)    READ_BITS(vec.x, float, loc);\
                                READ_BITS(vec.y, float, loc);\
                                READ_BITS(vec.z, float, loc);\


//Returns true if path is a ligid file
bool projectUTIL_read_ligid_file(
                                    std::string path,
                                    time_t &creationDate,
                                    time_t &lastOpenedDate
                                )
{ 

    UTIL::correctFolderDistinguishers(path);
    
    if(path.size()){
        std::ifstream rf(path, std::ios::out | std::ios::binary);
		
        if(!rf) {
            LGDLOG::start<< "ERROR : Reading ligid file! Cannot open file : " << path << LGDLOG::end;
            return false;
        }

        // ---------- Description ------------
        uint64_t h1 = 0xBBBBBBBB; 
        uint64_t h2 = 0xCA4B6C78; 
        uint64_t h3 = 0x9A9A2C48; 
        
        uint64_t ch1; 
        uint64_t ch2; 
        uint64_t ch3; 

        READ_BITS(ch1, uint64_t, "Description header 1");
        READ_BITS(ch2, uint64_t, "Description header 2");
        READ_BITS(ch3, uint64_t, "Description header 3");

        if(ch1 != h1 || ch2 != h2 || ch3 != h3){
            LGDLOG::start<< "ERROR : Reading ligid file : Description header is not correct : " << path << LGDLOG::end;
            return false;
        }

        // ---------- Version Number ------------
        uint32_t versionNumber2000 = 2000;   
        uint32_t versionNumber2100 = 2100;   
        uint32_t versionNumber2200 = 2200;   
        uint32_t versionNumber2300 = 2300;   
        uint32_t versionNumber2400 = 2400;   
        uint32_t versionNumber2500 = 2500;   
        uint32_t versionNumber2600 = 2600;   
        uint32_t versionNumber2700 = 2700;   
        
        uint32_t versionNumber; //2700  
        READ_BITS(versionNumber, uint32_t, "Version number");
        
        std::cout << "File Version : " << versionNumber << std::endl;
        
        if(versionNumber != versionNumber2700){
            LGDLOG::start<< "ERROR : Reading ligid file : Invalid version : " << versionNumber << LGDLOG::end; 
            return false;
        }

        // ---------- Dates ------------
        READ_BITS(creationDate, time_t, "Creation date");
        std::cout << "Creation date : " << creationDate << std::endl;

        READ_BITS(lastOpenedDate, time_t, "Last opened date");
        std::cout << "Last opening date : " << lastOpenedDate << std::endl;
        
        // ---------- 3D Model ------------
        std::string modelTitle = "";
        READ_STR(modelTitle)
        std::cout << "Active 3D model : " << modelTitle << std::endl;
        for (size_t i = 0; i < Library::getModelArraySize(); i++)
        {
            if(Library::getModelObj(i).title == modelTitle){
                getScene()->model = Library::getModel(i);

                for (Mesh& mesh : getScene()->model->meshes)
                {
                    mesh.layerScene.update_result(std::stoi(comboBox_layers_resolution.texts[comboBox_layers_resolution.selectedIndex]), glm::vec3(0.f), mesh);
                }
            }
        }
        
        if(!getScene()->model->meshes.size())
            getScene()->model->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx", true, false);
        
        getScene()->model->newModelAdded = true;

        // ---------- Settings ------------
        // GUI
        READ_BITS(panel_library_modes.selectedElement, int, "panel_library_modes.selectedElement"); // Selected library mode on the left
        for (size_t i = 0; i < panel_library_modes.sections[0].elements.size(); i++){panel_library_modes.sections[0].elements[i].button.clickState1 = false;}
        if(panel_library_modes.selectedElement < panel_library_modes.sections[0].elements.size())panel_library_modes.sections[0].elements[panel_library_modes.selectedElement].button.clickState1 = true;
        
        READ_BITS(checkBox_wrap_mode.clickState1, bool, "checkBox_wrap_mode.clickState1"); // Wrap mode checkbox
        READ_BITS(panel_displaying_modes.selectedElement, int, "panel_displaying_modes.selectedElement"); // Paint display mode
        for (size_t i = 0; i < panel_displaying_modes.sections[0].elements.size(); i++){panel_displaying_modes.sections[0].elements[i].button.clickState1 = false;}
        if(panel_displaying_modes.selectedElement < panel_displaying_modes.sections[0].elements.size())panel_displaying_modes.sections[0].elements[panel_displaying_modes.selectedElement].button.clickState1 = true;
        READ_BITS(button_mesh_selection.selectedMeshI, int, "button_mesh_selection.selectedMeshI"); // Selected mesh
        int layers_resolution; 
        READ_BITS(layers_resolution, int, "layers_resolution"); for (size_t i = 0; i < comboBox_layers_resolution.texts.size(); i++){if(std::stoi(comboBox_layers_resolution.texts[i]) == layers_resolution)comboBox_layers_resolution.selectedIndex = i;}
        READ_BITS(comboBox_PBR_displaying_mode.selectedIndex, int, "comboBox_PBR_displaying_mode.selectedIndex"); // Main scene displaying mode
        READ_BITS(twoD_painting_mode, bool, "twoD_painting_mode"); // 2D mode or 3D mode
        std::string selected_texture_title = "";
        READ_STR(selected_texture_title); // Title of the selected texture
        for (size_t i = 0; i < Library::getTextureArraySize(); i++){if(Library::getTextureObj(i).title == selected_texture_title)panel_library_selected_texture = Library::getTextureObj(i);}

        // Settings
        READ_BITS(Settings::properties()->VSync, bool, "Settings::properties()->VSync"); 
        READ_BITS(Settings::properties()->cat_allowComments, bool, "Settings::properties()->cat_allowComments"); 
        READ_BITS(Settings::properties()->cat_hide, bool, "Settings::properties()->cat_hide"); 
        READ_BITS(Settings::properties()->cat_verifyTheExit, bool, "Settings::properties()->cat_verifyTheExit"); 
        READ_BITS(Settings::properties()->framebufferResolutionDivider, float, "Settings::properties()->framebufferResolutionDivider"); 
        
        // Scene
        READ_BITS(getScene()->aFar, float, "getScene()->aFar"); 
        READ_BITS(getScene()->aNear, float, "getScene()->aNear"); 
        READ_BITS(getScene()->backfaceCulling, bool, "getScene()->backfaceCulling"); 
        READ_BITS(getScene()->fov, float, "getScene()->fov"); 
        READ_BITS(getScene()->renderAxisDisplayer, bool, "getScene()->renderAxisDisplayer"); 
        READ_BITS(getScene()->renderTiles, bool, "getScene()->renderTiles"); 
        READ_BITS(getScene()->useOrtho, bool, "getScene()->useOrtho"); 
        READ_VEC3(getScene()->transformRotation, "getScene()->transformRotation"); 
        READ_VEC3(getScene()->camera.cameraPos, "getScene()->camera.cameraPos"); 
        READ_VEC3(getScene()->camera.originPos, "getScene()->originPos"); 
        getScene()->camera.updateViewMatrix();
        getScene()->camera.setCameraPosition(getScene()->camera.cameraPos);
        READ_STR(getScene()->skybox.load_path);
        if(std::filesystem::exists(getScene()->skybox.load_path))
            getScene()->skybox.load(getScene()->skybox.load_path);
        READ_VEC3(getScene()->skybox.bgColor, "getScene()->skybox.bgColor");
        READ_BITS(getScene()->skybox.opacity, float, "getScene()->skybox.opacity");
        READ_BITS(getScene()->skybox.lod, float, "getScene()->skybox.lod");
        READ_BITS(getScene()->skybox.rotation, float, "getScene()->skybox.rotation");
        READ_BITS(getScene()->skybox.bg_txtr_opacity, float, "getScene()->skybox.bg_txtr_opacity");
        getScene()->skybox.bg_txtr.readPixelData(rf);
        
        // Scenes
        getVectorScene()->read_data(rf);
        getTextureFieldScene()->read_data(rf); 
        
        // Color Settings 
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[1].checkBox.clickState1, bool, "Enable Albedo Channel");
        READ_VEC3(checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.clr1_Btn.color, "Selected color 1");
        READ_VEC3(checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.clr2_Btn.color, "Selected color 2");
        READ_VEC3(checkComboList_painting_color.panel.sections[0].elements[2].painterColorSelection.clr3_Btn.color, "Selected color 3");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[3].checkBox.clickState1, bool, "Enable Roughness Channel");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[4].rangeBar.value, float, "Roughness Channel Value");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[5].checkBox.clickState1, bool, "Enable Metallic Channel");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[6].rangeBar.value, float, "Metallic Channel Value");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[7].checkBox.clickState1, bool, "Enable NormalMap Channel");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[8].rangeBar.value, float, "NormalMap Channel Value");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[9].checkBox.clickState1, bool, "Enable HeightMap Channel");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[10].rangeBar.value, float, "HeightMap Channel Value");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[11].checkBox.clickState1, bool, "Enable AO Channel");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[12].rangeBar.value, float, "AO Channel Value");
        READ_BITS(checkComboList_painting_color.panel.sections[0].elements[13].checkBox.clickState1, bool, "Use custom material");
        FileHandler::readMaterialData(rf, checkComboList_painting_color.panel.sections[0].elements[14].button.material, {});
        
        // Face selection settings
        READ_BITS(checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1, bool, "Mask to selected mesh");
        READ_BITS(checkComboList_mesh_face_selection.panel.sections[0].elements[1].checkBox.clickState1, bool, "Mesh selection mode");
        checkComboList_mesh_face_selection.panel.sections[0].elements[1].button.texture.readTextureData(rf, true, 2, true);

        // Mirror settings
        READ_BITS(checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1, bool, "X axis");
        READ_BITS(checkComboList_painting_mirror.panel.sections[0].elements[1].rangeBar.value, float, "X axis mirror offset");
        READ_BITS(checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1, bool, "Y axis");
        READ_BITS(checkComboList_painting_mirror.panel.sections[0].elements[3].rangeBar.value, float, "Y axis mirror offset");
        READ_BITS(checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1, bool, "Z axis");
        READ_BITS(checkComboList_painting_mirror.panel.sections[0].elements[5].rangeBar.value, float, "Z axis mirror offset");

        // Painting over settings
        READ_BITS(checkComboList_painting_over.panel.sections[0].elements[0].checkBox.clickState1, bool, "Use Painting Over");
        READ_BITS(checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1, bool, "Painting over edit mode");
        READ_BITS(checkComboList_painting_over.panel.sections[0].elements[4].checkBox.clickState1, bool, "Painting over grayscale masking");   

        return true;
    }
}