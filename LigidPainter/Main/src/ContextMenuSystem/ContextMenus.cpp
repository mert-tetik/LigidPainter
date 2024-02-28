/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include "GUI/GUI.hpp"

namespace ContextMenus{
    ContextMenu texture = ContextMenu({"Rename", "Duplicate", "Copy Path", "Edit", "Delete"}); 
    ContextMenu material = ContextMenu({"Edit", "Rename", "Duplicate", "Copy Path", "Delete", "Export"});    
    ContextMenu brush = ContextMenu({"Edit Brush", "Rename", "Duplicate", "Copy Path", "Delete"});    
    ContextMenu model = ContextMenu({"Model Info", "Use This Model"});    
    ContextMenu menuBarProject = ContextMenu({"Save / CTRL+S", "Save as / CTRL+SHIFT+S", "Create new", "Load new", "Copy Path", "File Explorer", "Recover"}); 
    ContextMenu menuBarHelp = ContextMenu({"Website", "YouTube"}); 
    ContextMenu materialModifier = ContextMenu({"Delete", "Move To Top", "Move To Bottom", "Change Mask"});
    ContextMenu node = ContextMenu({"Delete"}); 
    ContextMenu addMaterialModifier = ContextMenu({"Texture Modifier", "Dust Modifier", "Asphalt Modifier", "Liquid Modifier", "Moss Modifier", "Rust Modifier", "Skin Modifier", "Solid Modifier", "Wooden Modifier", "Math Modifier"});
    ContextMenu nodeScenePanel = ContextMenu({"Add Material ID Node", "Add Material Mask Node"});
};