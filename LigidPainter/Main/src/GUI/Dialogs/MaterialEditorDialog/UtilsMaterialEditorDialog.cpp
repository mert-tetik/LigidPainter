/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

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

#include <string>
#include <iostream>
#include <vector>

void MaterialEditorDialog::activate(){
    selectedMaterialModifierIndex = 0;
    dialogControl.activate();
}

void MaterialEditorDialog::deactivate(TextureSelectionDialog &textureSelectionDialog){
    selectedMaterialModifierIndex = 0;
    dialogControl.unActivate();
    textureSelectionDialog.dialogControl.unActivate();
}