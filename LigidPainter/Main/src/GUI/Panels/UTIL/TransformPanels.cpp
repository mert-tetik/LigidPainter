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

#include "GUI/Panels.hpp"

#include <string>
#include <vector>

void panels_transform()
{
    //Calculate the total screen gap
    float screenGap = Settings::videoScale()->x - getContext()->windowScale.x; //Use that value to keep panels on the right side
    
    //Calculate the screen gap in 0 - 100 range
    float screenGapPerc = screenGap / Settings::videoScale()->x * 100.f; 

    //!Positioning the panels

    panel_window.scale.y = 50.f - panel_navigation.scale.y;

    panel_window.pos.x = 100.f - panel_window.scale.x - screenGapPerc; //Keep on the right side
    panel_library.pos.x = panel_library_modes.pos.x + panel_library_modes.scale.x + panel_library.scale.x; //Keep on the left side of the window panel 
    
    panel_window.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + panel_window.scale.y; //Keep beneath the navigation bar
    panel_window.scale.y = 50.f - panel_navigation.scale.y; //Keep beneath the navigation bar
    
    button_mesh_selection.scale.x = panel_layers.scale.x;
    button_mesh_selection.pos = panel_window.pos;
    button_mesh_selection.pos.x -= panel_window.scale.x + button_mesh_selection.scale.x;
    button_mesh_selection.pos.y -= panel_window.scale.y - button_mesh_selection.scale.y;
    
    comboBox_layers_resolution.scale.x = panel_layers.scale.x;
    comboBox_layers_resolution.pos = button_mesh_selection.pos;
    comboBox_layers_resolution.pos.y = button_mesh_selection.pos.y + button_mesh_selection.scale.y + comboBox_layers_resolution.scale.y;
    
    comboBox_PBR_displaying_mode.scale.x = comboBox_layers_resolution.scale.x;
    comboBox_PBR_displaying_mode.pos = comboBox_layers_resolution.pos;
    comboBox_PBR_displaying_mode.pos.y = comboBox_layers_resolution.pos.y + comboBox_layers_resolution.scale.y + comboBox_PBR_displaying_mode.scale.y;
    
    panel_add_layer.scale.x = panel_layers.scale.x;
    panel_add_layer.pos = comboBox_PBR_displaying_mode.pos;
    panel_add_layer.pos.y = comboBox_PBR_displaying_mode.pos.y + comboBox_PBR_displaying_mode.scale.y + panel_add_layer.scale.y;
    
    panel_layers.scale.y = panel_window.scale.y - panel_add_layer.scale.y - button_mesh_selection.scale.y - comboBox_PBR_displaying_mode.scale.y - comboBox_layers_resolution.scale.y;
    panel_layers.pos = panel_add_layer.pos;
    panel_layers.pos.y += panel_add_layer.scale.y + panel_layers.scale.y;

    panel_library_modes.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + panel_window.scale.y; //Keep beneath the navigation bar
    panel_library.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + panel_library.scale.y; //Keep beneath the navigation bar
    panel_library.scale.y = 50 - panel_navigation.scale.y - panel_selected_texture_display.scale.y;
    panel_library_modes.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + panel_library.scale.y; //Keep beneath the navigation bar
    panel_library_modes.scale.y = 50 - panel_navigation.scale.y - panel_selected_texture_display.scale.y;
    panel_selected_texture_display.pos.x = panel_library.pos.x - panel_library_modes.pos.x;
    panel_selected_texture_display.scale.x = panel_library.scale.x + panel_library_modes.scale.x; 
    
    panel_twoD_painting.scale.x = 50 - screenGapPerc/2.f - panel_selected_texture_display.scale.x - (panel_window.scale.x);
    panel_twoD_painting.scale.y = 50 - panel_navigation.scale.y;
    panel_twoD_painting.pos.x = panel_window.pos.x - panel_window.scale.x - panel_twoD_painting.scale.x; //Keep on the left side of the window panel 
    panel_twoD_painting.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + panel_twoD_painting.scale.y; //Keep beneath the navigation bar
    panel_painting_modes.pos.x = panel_library.pos.x + panel_library.scale.x + panel_painting_modes.scale.x + 1;
    panel_painting_modes.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + panel_painting_modes.scale.y;

    checkComboList_mesh_face_selection.pos = panel_painting_modes.pos;
    checkComboList_mesh_face_selection.pos.x += panel_painting_modes.scale.x + checkComboList_mesh_face_selection.scale.x + 0.5f;
    checkComboList_mesh_face_selection.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + checkComboList_mesh_face_selection.scale.y;
    checkComboList_painting_over.pos = checkComboList_mesh_face_selection.pos;  
    checkComboList_painting_over.pos.x += checkComboList_mesh_face_selection.scale.x + checkComboList_painting_over.scale.x + 0.25f;  
    checkComboList_painting_color.pos = checkComboList_painting_over.pos;  
    checkComboList_painting_color.pos.x += checkComboList_painting_over.scale.x + checkComboList_painting_color.scale.x + 0.25f;  
    checkComboList_painting_mirror.pos = checkComboList_painting_color.pos;  
    checkComboList_painting_mirror.pos.x += checkComboList_painting_color.scale.x + checkComboList_painting_mirror.scale.x + 0.25f;  
    button_painting_brush.scale.y = checkComboList_painting_mirror.scale.y;  
    button_painting_brush.pos = checkComboList_painting_mirror.pos;  
    button_painting_brush.pos.x += checkComboList_painting_mirror.scale.x + button_painting_brush.scale.x + 0.25f;  

    //checkComboList_mesh_face_selection
    //checkComboList_painting_over
    
    panel_smear_painting_properties.pos.x = panel_library.pos.x + panel_library.scale.x + panel_smear_painting_properties.scale.x + 1;
    panel_smear_painting_properties.pos.y = panel_painting_modes.pos.y + panel_painting_modes.scale.y + panel_smear_painting_properties.scale.y + 1;
    
    button_painting_filter_mode_filter.pos.x = panel_library.pos.x + panel_library.scale.x + button_painting_filter_mode_filter.scale.x + 1;
    button_painting_filter_mode_filter.pos.y = panel_painting_modes.pos.y + panel_painting_modes.scale.y + button_painting_filter_mode_filter.scale.y + 1;
    button_painting_filter_mode_filter.pos.z = panel_library.pos.z;
    
    panel_displaying_modes.pos.x = panel_layers.pos.x - panel_layers.scale.x - panel_displaying_modes.scale.x - 0.5f;
    panel_displaying_modes.pos.y = panel_navigation.pos.y + panel_navigation.scale.y + panel_displaying_modes.scale.y;
    
    checkBox_wrap_mode.pos = panel_displaying_modes.pos;
    checkBox_wrap_mode.pos.x = panel_layers.pos.x - panel_layers.scale.x - checkBox_wrap_mode.scale.x * 2.f;
    checkBox_wrap_mode.pos.y += panel_displaying_modes.scale.y + checkBox_wrap_mode.scale.y + 4.f;

    gizmo_scene.pos = checkBox_wrap_mode.pos;
    gizmo_scene.pos.x = panel_displaying_modes.pos.x;
    gizmo_scene.pos.y += checkBox_wrap_mode.scale.y + gizmo_scene.scale.y + 1.f;

    panel_selected_texture_display.sections[0].elements[0].scale.y = panel_selected_texture_display.scale.y;

    panel_twoD_painting.sections[0].elements[0].scale.y = panel_twoD_painting.scale.y;

}