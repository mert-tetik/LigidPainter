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

/*! @brief 0 = LigidPainter logo, 1 = Project, 2 = Help*/
extern Panel panel_navigation;
void panel_navigation_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = 3D Painting, 1 = 2D Painting, 2 = Export, 3 = Displayer, 4 = Settings, 5 = Baking, 6 = Materials"*/
extern Panel panel_window;
void panel_window_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = Textures, 1 = Materials, 2 = Brushes, 3 = 3D Models, = 4 Fonts, 5 = Scripts, 6 = Filters, 7 = Layers, 8 = Texture Packs*/
extern Panel panel_library_modes;
void panel_library_modes_render(Timer& timer, bool doMouseTracking);

extern Panel panel_selected_texture_display;
void panel_selected_texture_display_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = Nodes, 1 = Painting, 2 = Solid Painting*/
extern Panel panel_displaying_modes;
void panel_displaying_modes_render(Timer& timer, bool doMouseTracking);

extern Panel panel_objects;
void panel_objects_render(Timer& timer, bool doMouseTracking);

extern Button button_texture_selected_objects;
void button_texture_selected_objects_render(Timer& timer, bool doMouseTracking);

extern Gizmo gizmo_scene;
void gizmo_scene_render(Timer& timer, bool doMouseTracking);

extern Panel panel_layers;
/*! @brief 0 = Texture layer, 1 = Painting layer, 2 = Material layer, 3 = Vectors layer */
extern Panel panel_add_layer;
extern Button button_mesh_selection;
extern ComboBox comboBox_layers_resolution;
void panel_layers_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = Draw, 1 = Soften, 2 = Smear, 3 = Normal, 4 = Filter, 5 = Vector, 6 = Bucket*/
extern Panel panel_painting_modes;
void panel_painting_modes_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = Material result displayer, 1 = enable albedo, 2 = albedo color, 3 = enable roughness, 4 = roughness value, 5 = enable metallic, 6 = metallic value, 7 = enable normal, 8 = normal value, 9 = enable heightmap, 10 = heightmap value, 11 = enable ao, 12 = ao value, 13 = use custom material, 14 = select custom material */
extern CheckComboList checkComboList_painting_color;
void checkComboList_painting_color_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = Mask to selected mesh, 1 = Mesh selection mode, 2 = Selecting Mode, 3 = Circle radius, 4 = Auto Select, 5 = Mask via texture, 6 = Cancel texture mask, 7 = Hide unselected mesh*/
extern CheckComboList checkComboList_painting_face_selection;
void checkComboList_painting_face_selection_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = x axis, 1 = x axis offset, 2 = x axis, 3 = x axis offset, 4 = x axis, 5 = x axis offset*/
extern CheckComboList checkComboList_painting_mirror;
void checkComboList_painting_mirror_render(Timer& timer, bool doMouseTracking);

/*! @brief 0 = Use Painting Over, 1 = Editor Mode, 2 = Add Texture From Library, 3 = Add Texture From Computer, 4 = Grayscale Masking */
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

/*! @brief 0 = Transform Strength, 1 = Bluring Strength */
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