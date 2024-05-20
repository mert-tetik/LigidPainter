/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/Panels.hpp"

#include <string>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

extern bool _any_panels_hovered;
extern bool _last_any_panels_hovered;


void panels_render(Timer& timer)
{
    _last_any_panels_hovered = _any_panels_hovered; 
    _any_panels_hovered = false;

    ShaderSystem::buttonShader().use();
    ShaderSystem::buttonShader().setMat4("projection", getContext()->ortho_projection); 
    ShaderSystem::buttonShader().setVec2("properties.txtrScale", glm::vec2(1.f));
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    glDepthFunc(GL_LEQUAL);

    Debugger::block("GUI : panelPositioning"); // Start
    panels_transform();
    Debugger::block("GUI : panelPositioning"); // End

    Debugger::block("GUI : Panels : Navigation panel"); // Start
    panel_navigation_render(timer, true);
    Debugger::block("GUI : Panels : Navigation panel"); // End

    Debugger::block("GUI : Panels : Window Panel"); // Start
    panel_window_render(timer, true);
    Debugger::block("GUI : Panels : Window Panel"); // End

    Debugger::block("GUI : Panels : Library Modes Panel"); // Start
    panel_library_modes_render(timer, true);
    Debugger::block("GUI : Panels : Library Modes Panel"); // End

    Debugger::block("GUI : Panels : Library Panel"); // End
    panel_library_render(timer, true);
    Debugger::block("GUI : Panels : Library Panel"); // End
    
    Debugger::block("GUI : Panels : Selected Texture Displayer Panel"); // Start
    panel_selected_texture_display_render(timer, true);
    Debugger::block("GUI : Panels : Selected Texture Displayer Panel"); // End
    
    Debugger::block("GUI : Panels : Scene Gizmo"); // Start
    gizmo_scene_render(timer, true);
    Debugger::block("GUI : Panels : Scene Gizmo"); // End

    Debugger::block("GUI : Panels : Wrap Mode Checkbox"); // Start
    checkBox_wrap_mode_render(timer, !twoD_painting_mode);
    Debugger::block("GUI : Panels : Wrap Mode Checkbox"); // End
    
    Debugger::block("GUI : Panels : Current mode hint displayer"); // Start
    current_mode_hint_displayer_render(timer);
    Debugger::block("GUI : Panels : Current mode hint displayer"); // End

    Debugger::block("GUI : Panels : Layers Panel"); // Start
    panel_layers_render(timer, !twoD_painting_mode);
    Debugger::block("GUI : Panels : Layers Panel"); // End

    Debugger::block("GUI : Panels : Displaying modes panel"); // Start
    panel_displaying_modes_render(timer, !twoD_painting_mode);
    Debugger::block("GUI : Panels : Displaying modes panel"); // End
    
    if(twoD_painting_mode){
        Debugger::block("GUI : Panels : 2D painting panel"); // Start
        panel_twoD_painting_render(timer, true);
        Debugger::block("GUI : Panels : 2D painting panel"); // End

        checkBox_wrap_mode.clickState1 = false;
        checkComboList_painting_mirror.panel.sections[0].elements[0].checkBox.clickState1 = false;
        checkComboList_painting_mirror.panel.sections[0].elements[2].checkBox.clickState1 = false;
        checkComboList_painting_mirror.panel.sections[0].elements[4].checkBox.clickState1 = false;
        checkComboList_mesh_face_selection.panel.sections[0].elements[0].checkBox.clickState1 = false;
        checkComboList_mesh_face_selection.panel.sections[0].elements[1].checkBox.clickState1 = false;
    }

    if(panel_displaying_modes.selectedElement == 1 || panel_displaying_modes.selectedElement == 2){
        Debugger::block("GUI : Panels : Painting modes panel"); // Start
        panel_painting_modes_render(timer, true);
        Debugger::block("GUI : Panels : Painting modes panel"); // End

        Debugger::block("GUI : Panels : Painting color check combo list"); // Start
        checkComboList_painting_color_render(timer, true || checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1);
        Debugger::block("GUI : Panels : Painting color check combo list"); // End        
        
        Debugger::block("GUI : Panels : Painting mirror check combo list"); // Start
        checkComboList_painting_mirror_render(timer, true || checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1);
        Debugger::block("GUI : Panels : Painting mirror check combo list"); // End
        
        Debugger::block("GUI : Panels : Painting over check combo list"); // Start
        checkComboList_painting_over_render(timer, true || checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1);
        Debugger::block("GUI : Panels : Painting over check combo list"); // End
        
        Debugger::block("GUI : Panels : Painting brush button"); // Start
        button_painting_brush_render(timer, true);
        Debugger::block("GUI : Panels : Painting brush button"); // End

        if(panel_painting_modes.selectedElement == 2){
            Debugger::block("GUI : Panels : Smear painting properties panel"); // Start
            panel_smear_painting_properties_render(timer, true);
            Debugger::block("GUI : Panels : Smear painting properties panel"); // End
        }
        if(panel_painting_modes.selectedElement == 4){
            Debugger::block("GUI : Panels : Painting filter mode filter selection button"); // Start
            button_painting_filter_mode_filter_render(timer, true);
            Debugger::block("GUI : Panels : Painting filter mode filter selection button"); // End
        }
    }
    else{
        Debugger::block("GUI : Panels : Texture selected objects button"); // Start
        button_texture_selected_objects_render(timer, true);
        Debugger::block("GUI : Panels : Texture selected objects button"); // End
    }

    Debugger::block("GUI : Panels : Painting face selection check combo list"); // Start
    checkComboList_mesh_face_selection_render(timer, true || checkComboList_painting_over.panel.sections[0].elements[1].checkBox.clickState1);
    Debugger::block("GUI : Panels : Painting face selection check combo list"); // End
    
    if(dialog_greeting.dialogControl.isActive()){
        dialog_greeting.show(timer);
    }
    if(dialog_newProject.dialogControl.isActive()){
        dialog_newProject.show(timer);
    }
    if(dialog_loadProject.dialogControl.isActive()){
        dialog_loadProject.show(timer);
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if(!Settings::properties()->cat_hide){
        Debugger::block("GUI : Log Dialog"); // Start
        dialog_log.render(timer);
        Debugger::block("GUI : Log Dialog"); // End
    }

    updateThePreRenderedPanels = false;
}