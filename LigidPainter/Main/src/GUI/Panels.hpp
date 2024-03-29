/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LIGID_PANELS_HPP
#define LIGID_PANELS_HPP

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"

extern Panel panel_library;
extern Texture panel_library_selected_texture;
void panel_library_render(Timer& timer, bool doMouseTracking);

extern Panel panel_navigation;
void panel_navigation_render(Timer& timer, bool doMouseTracking);

extern Panel panel_window;
void panel_window_render(Timer& timer, bool doMouseTracking);

extern Panel panel_library_modes;
void panel_library_modes_render(Timer& timer, bool doMouseTracking);

extern Panel panel_selected_texture_display;
void panel_selected_texture_display_render(Timer& timer, bool doMouseTracking);

extern Panel panel_displaying_modes;
void panel_displaying_modes_render(Timer& timer, bool doMouseTracking);

extern Panel panel_objects;
void panel_objects_render(Timer& timer, bool doMouseTracking);

extern Button button_texture_selected_objects;
void button_texture_selected_objects_render(Timer& timer, bool doMouseTracking);

extern Gizmo gizmo_scene;
void gizmo_scene_render(Timer& timer, bool doMouseTracking);

extern Panel panel_layers;
extern Panel panel_add_layer;
extern Button button_mesh_selection;
extern ComboBox comboBox_layers_resolution;
void panel_layers_render(Timer& timer, bool doMouseTracking);

extern Panel panel_painting_modes;
void panel_painting_modes_render(Timer& timer, bool doMouseTracking);

extern CheckComboList checkComboList_painting_color;
void checkComboList_painting_color_render(Timer& timer, bool doMouseTracking);

extern CheckComboList checkComboList_painting_face_selection;
void checkComboList_painting_face_selection_render(Timer& timer, bool doMouseTracking);

extern CheckComboList checkComboList_painting_mirror;
void checkComboList_painting_mirror_render(Timer& timer, bool doMouseTracking);

extern CheckComboList checkComboList_painting_over;
void checkComboList_painting_over_render(Timer& timer, bool doMouseTracking);

extern Panel panel_twoD_painting;
extern Box twoD_painting_box;
extern bool twoD_painting_mode;
void panel_twoD_painting_render(Timer& timer, bool doMouseTracking);

extern Button button_painting_brush;
void button_painting_brush_render(Timer& timer, bool doMouseTracking);

extern Button button_painting_filter_mode_filter;
void button_painting_filter_mode_filter_render(Timer& timer, bool doMouseTracking);

extern Panel panel_smear_painting_properties;
void panel_smear_painting_properties_render(Timer& timer, bool doMouseTracking);

extern CheckBox checkBox_wrap_mode;
void checkBox_wrap_mode_render(Timer& timer, bool doMouseTracking);

void current_mode_hint_displayer_render(Timer& timer);



/*! @brief Positions & scales the panels according to each other*/
void panels_transform();

/*! @brief Creates panels (calls structures for each panel)*/
void panels_init();

/*! @brief Returns true if any panel was hovered by the cursor*/
bool panels_any_hovered();

/*! @brief Renders all the panels (positioning done inside of it via 'panels_transform' function) */
void panels_render(Timer& timer);

#endif