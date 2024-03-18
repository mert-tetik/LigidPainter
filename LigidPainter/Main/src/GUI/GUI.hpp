/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LIGID_GUI_HPP
#define LIGID_GUI_HPP

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "GUI/Dialogs/Dialogs.hpp"
#include "ContextMenuSystem/ContextMenus.hpp"




class UI{
private:
    bool prevStraightLinePaintingCondition = false;
    glm::vec2 straightLinePaintingStartPos = glm::vec2(50.f);
    glm::vec2 straightLinePaintingDirectionPos = glm::vec2(50.f);

    void renderPaintingOverTextureFields(Timer& timer, Painter& painter);

public:
    /* -- PANELS -- */
    
    int selectedPaintingPanelMode = 0;

    bool anyPanelHover = false;

    std::vector<TextureField> paintingOverTextureFields;

    int frameCounter = 0; 

    bool anyDialogActive = false;

    /*! @brief Default constructor */
    UI();

    /*! @brief Renders all the GUI */
    void render(Timer &timer,Project &project, Painter &painter, Skybox &skybox);

private: 

    void renderPanels(Timer &timer, Painter &painter, Project& project);

    void renderRenamingTextbox(Timer &timer, Painter &painter);

    void renderDialogs(Timer &timer, Project &project, Skybox &skybox, Painter& painter);
};

#endif //LIGID_GUI_HPP