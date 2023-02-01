#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/Callback.h"
#include "Core/Gl.h"
#include "Core/Render/Render.h"
#include "Core/Model/Model.h"
#include "Core/Texture/Texture.h"
#include "Core/UI/UiActions.h"
#include "Core/TextureGenerator/TextureGenerator.h"

ContextMenu UserInterface::createContextMenus(std::vector<Node> appNodes){
    ColorData colorData;
    ContextMenu addNodeContextMenu; 
    addNodeContextMenu.width = 0.07f;
	addNodeContextMenu.positionZ = 0.99f;

    float panelHeight = 0.f;

    Button title;
    title.width = addNodeContextMenu.width+0.02f;
    title.height = 0.025f;
    title.buttonCurveReduce = 10000;
    title.color = colorData.nodePanelContextMenuButtonColor; 
    title.colorHover = colorData.nodePanelContextMenuButtonColorHover;
    title.positionX = 0;
    title.positionY = 0.04f*appNodes.size();
    title.positionZ = 0.99f;
    title.text = "Add Nodes";
    title.textRatio = 0.052f;
    title.hoverAnimationActive = false;
    addNodeContextMenu.buttons.push_back(title);

    for (size_t i = 0; i < appNodes.size(); i++)
    {
        Button button;
        button.width = addNodeContextMenu.width+0.02f;
        button.height = 0.025f;
        button.buttonCurveReduce = 10000;
        button.color = colorData.nodePanelContextMenuButtonColor; 
        button.colorHover = colorData.nodePanelContextMenuButtonColorHover;
        button.positionX = 0;
        button.positionY = 0.02 - (i*0.05);
        button.positionZ = 0.99f;
        button.text = appNodes[i].title;
        button.textRatio = 0.052f;
        button.hoverAnimationActive = true;
        addNodeContextMenu.buttons.push_back(button);
        panelHeight += 0.04f;
    }
	addNodeContextMenu.height = panelHeight;
    


    return addNodeContextMenu;
}