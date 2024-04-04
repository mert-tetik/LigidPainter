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

bool panels_any_hovered(){
    return  panel_library.hover ||
            panel_navigation.hover ||
            panel_window.hover ||
            panel_library_modes.hover ||
            panel_selected_texture_display.hover ||
            panel_displaying_modes.hover ||
            button_texture_selected_objects.hover ||
            gizmo_scene.hover ||
            panel_layers.hover ||
            panel_add_layer.hover ||
            button_mesh_selection.hover ||
            comboBox_layers_resolution.hover[0] ||
            panel_painting_modes.hover ||
            checkComboList_painting_color.hover ||
            checkComboList_painting_face_selection.hover ||
            checkComboList_painting_mirror.hover ||
            checkComboList_painting_over.hover ||
            panel_twoD_painting.hover ||
            button_painting_brush.hover ||
            button_painting_filter_mode_filter.hover ||
            panel_smear_painting_properties.hover ||
            checkBox_wrap_mode.hover;
}