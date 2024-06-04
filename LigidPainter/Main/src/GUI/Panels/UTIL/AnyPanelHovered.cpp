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

#include "UTIL/Project/Project.hpp"

#include <string>
#include <vector>

extern bool _any_panels_hovered;
extern bool _last_any_panels_hovered;

bool panels_any_hovered(){
    return _any_panels_hovered || _last_any_panels_hovered ||
            gizmo_scene.hover ||
            button_mesh_selection.hover ||
            comboBox_layers_resolution.hover[0] ||
            comboBox_PBR_displaying_mode.hover[0] ||
            checkComboList_painting_color.hover ||
            checkComboList_mesh_face_selection.hover ||
            checkComboList_painting_mirror.hover ||
            checkComboList_painting_over.hover ||
            button_painting_brush.hover ||
            button_painting_filter_mode_filter.hover ||
            getScene()->get_selected_mesh()->layerScene.any_dialog_hovered() ||
            dialog_log.isHovered() ||
            dialog_greeting.dialogControl.isActive() ||
            dialog_loadProject.dialogControl.isActive() ||
            dialog_newProject.dialogControl.isActive() ||
            checkBox_wrap_mode.hover;
}