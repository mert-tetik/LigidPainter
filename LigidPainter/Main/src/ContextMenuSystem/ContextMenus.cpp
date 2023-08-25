/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https:github.com/mert-tetik/LigidPainter
Official Web Page : https:ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include "GUI/GUI.hpp"

namespace ContextMenus{
    ContextMenu texture = ContextMenu({"Rename", "Duplicate", "Copy Path", "Edit", "Delete"}); 
    ContextMenu material = ContextMenu({"Edit", "Add To Scene", "Rename", "Duplicate", "Copy Path", "Delete", "Export"});    
    ContextMenu brush = ContextMenu({"Use", "Apply Current", "Rename", "Duplicate", "Copy Path","Delete"});    
    ContextMenu menuBarProject = ContextMenu({"Save", "Save as", "Create new", "Load new", "Copy Path", "File Explorer"}); 
    ContextMenu menuBarPainting = ContextMenu({"Undo", "Redo"}); 
    ContextMenu menuBarHelp = ContextMenu({"Website", "YouTube"}); 
    ContextMenu materialModifier = ContextMenu({"Delete", "Move To Top", "Move To Bottom", "Change Mask"});
    ContextMenu node = ContextMenu({"Delete"}); 
    ContextMenu addMaterialModifier = ContextMenu({"Texture Modifier", "Dust Modifier", "Asphalt Modifier", "Fabric Modifier", "Moss Modifier", "Rust Modifier", "Skin Modifier", "Solid Modifier", "Wooden Modifier"});
    ContextMenu nodeScenePanel = ContextMenu({"Add Material ID Node", "Add Material Mask Node"});
};