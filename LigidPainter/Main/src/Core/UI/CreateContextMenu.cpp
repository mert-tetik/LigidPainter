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
#include "Core/gl.h"
#include "Core/Render/Render.h"
#include "Core/Model/model.h"
#include "Core/Texture/Texture.h"
#include "Core/UI/UiActions.h"
#include "Core/Texture Generator/TextureGenerator.h"

ContextMenu UserInterface::createContextMenus(){
    ColorData colorData;
    ContextMenu addNodeContextMenu; 
    addNodeContextMenu.width = 0.07f;
	addNodeContextMenu.height = 0.08f;
	addNodeContextMenu.positionZ = 0.99f;

    Button title;
    title.width = addNodeContextMenu.width+0.02f;
    title.height = 0.025f;
    title.buttonCurveReduce = 10000;
    title.color = colorData.nodePanelContextMenuButtonColor; 
    title.colorHover = colorData.nodePanelContextMenuButtonColorHover;
    title.positionX = 0;
    title.positionY = 0.1;
    title.positionZ = 0.99f;
    title.text = "Add Nodes";
    title.textRatio = 0.052f;
    title.hoverAnimationActive = false;
    addNodeContextMenu.buttons.push_back(title);

    Button imageTextureNode;
    imageTextureNode.width = addNodeContextMenu.width+0.02f;
    imageTextureNode.height = 0.025f;
    imageTextureNode.buttonCurveReduce = 10000;
    imageTextureNode.color = colorData.nodePanelContextMenuButtonColor; 
    imageTextureNode.colorHover = colorData.nodePanelContextMenuButtonColorHover;
    imageTextureNode.positionX = 0;
    imageTextureNode.positionY = 0.025;
    imageTextureNode.positionZ = 0.99f;
    imageTextureNode.text = "Image Texture";
    imageTextureNode.textRatio = 0.052f;
    imageTextureNode.hoverAnimationActive = true;
    addNodeContextMenu.buttons.push_back(imageTextureNode);

    Button pbrOutputNode;
    pbrOutputNode.width = addNodeContextMenu.width+0.02f;
    pbrOutputNode.height = 0.025f;
    pbrOutputNode.buttonCurveReduce = 10000;
    pbrOutputNode.color = colorData.nodePanelContextMenuButtonColor; 
    pbrOutputNode.colorHover = colorData.nodePanelContextMenuButtonColorHover;
    pbrOutputNode.positionX = 0;
    pbrOutputNode.positionY = -0.025f;
    pbrOutputNode.positionZ = 0.99f;
    pbrOutputNode.text = "PBR Output";
    pbrOutputNode.textRatio = 0.052f;
    pbrOutputNode.hoverAnimationActive = true;
    addNodeContextMenu.buttons.push_back(pbrOutputNode);

    return addNodeContextMenu;
}