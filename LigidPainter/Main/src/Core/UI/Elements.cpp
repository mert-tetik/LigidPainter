#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/LigidPainter.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"

#include "tinyfiledialogs.h"




std::vector<UIElement> UserInterface::getUiElements(Icons icons) {

	std::vector<UIElement> UIElements;

	ColorData colorData;


	//---------------------------MODEL PANEL---------------------------

	UIElement uploadingModelPathTextBoxElement;
    uploadingModelPathTextBoxElement.textBox.position_x = 0.0f;
    uploadingModelPathTextBoxElement.textBox.position_y = 0.6f;
    uploadingModelPathTextBoxElement.textBox.position_z = 0.9f;
    uploadingModelPathTextBoxElement.textBox.width = 0.12;
    uploadingModelPathTextBoxElement.textBox.height = 0.03;
    uploadingModelPathTextBoxElement.textBox.text = "";
    uploadingModelPathTextBoxElement.panel = 1;
    uploadingModelPathTextBoxElement.attachedToMainPanel = true;
	uploadingModelPathTextBoxElement.type = "textBox";
    UIElements.push_back(uploadingModelPathTextBoxElement);//1

	UIElement uploadingModelPathTextElement;
    uploadingModelPathTextElement.text.positionX = -0.05f;
    uploadingModelPathTextElement.text.positionY = 0.64f;
    uploadingModelPathTextElement.text.text = "File Path";
    uploadingModelPathTextElement.text.scale = 0.00022f;
    uploadingModelPathTextElement.text.color = colorData.textBoxColor;
    uploadingModelPathTextElement.panel = 1;
    uploadingModelPathTextElement.attachedToMainPanel = true;
    uploadingModelPathTextElement.type = "text";
	UIElements.push_back(uploadingModelPathTextElement);//2

	UIElement loadModelButtonElement;
    loadModelButtonElement.button.positionX = 0.0f;
    loadModelButtonElement.button.positionY = 0.45f;
    loadModelButtonElement.button.positionZ = 0.9f;
    loadModelButtonElement.button.width = 0.08f;
    loadModelButtonElement.button.height = 0.04f;
    loadModelButtonElement.button.color = colorData.buttonColor;
    loadModelButtonElement.button.colorHover = colorData.buttonColorHover;
    loadModelButtonElement.button.transitionMixVal = 0.0f;
    loadModelButtonElement.button.buttonCurveReduce = 10.0f;
    loadModelButtonElement.button.text = "Load";
    loadModelButtonElement.button.textRatio = 0.022f;
    loadModelButtonElement.panel = 1;
    loadModelButtonElement.attachedToMainPanel = true;
	loadModelButtonElement.type = "button";
    UIElements.push_back(loadModelButtonElement);//3

	UIElement autoTriangulateCheckBoxElement;
    autoTriangulateCheckBoxElement.checkBox.positionX = -0.08f;
    autoTriangulateCheckBoxElement.checkBox.positionY = 0.3f;
    autoTriangulateCheckBoxElement.checkBox.text = "Auto triangulate";
    autoTriangulateCheckBoxElement.panel = 1;
    autoTriangulateCheckBoxElement.attachedToMainPanel = true;
	autoTriangulateCheckBoxElement.type = "checkBox";
    UIElements.push_back(autoTriangulateCheckBoxElement);//4
	

	UIElement backfaceCullingCheckBoxElement;
    backfaceCullingCheckBoxElement.checkBox.positionX = -0.08f;
    backfaceCullingCheckBoxElement.checkBox.positionY = 0.2f;
    backfaceCullingCheckBoxElement.checkBox.text = "Backface Culling";
    backfaceCullingCheckBoxElement.panel = 1;
    backfaceCullingCheckBoxElement.attachedToMainPanel = true;
	backfaceCullingCheckBoxElement.type = "checkBox";
    UIElements.push_back(backfaceCullingCheckBoxElement);//5


	UIElement modelPanel3DModelsTextElement;
    modelPanel3DModelsTextElement.text.positionX = -0.05f;
    modelPanel3DModelsTextElement.text.positionY = 0.08f-0.1f;
    modelPanel3DModelsTextElement.text.text = "3D Models";
    modelPanel3DModelsTextElement.text.scale = 0.00022f;
    modelPanel3DModelsTextElement.text.color = colorData.textBoxColor;
    modelPanel3DModelsTextElement.panel = 1;
    modelPanel3DModelsTextElement.attachedToMainPanel = true;
	modelPanel3DModelsTextElement.type = "text";
    UIElements.push_back(modelPanel3DModelsTextElement);//6

	//Load plane button
	UIElement loadPlaneModelButtonElement;
    loadPlaneModelButtonElement.button.positionX = 0.0f;
    loadPlaneModelButtonElement.button.positionY = -0.1f;
    loadPlaneModelButtonElement.button.positionZ = 0.9f;
    loadPlaneModelButtonElement.button.width = 0.008f;
    loadPlaneModelButtonElement.button.height = 0.045f;
    loadPlaneModelButtonElement.button.color = colorData.buttonColor;
    loadPlaneModelButtonElement.button.colorHover = colorData.buttonColorHover;
    loadPlaneModelButtonElement.button.transitionMixVal = 0.0f;
    loadPlaneModelButtonElement.button.buttonCurveReduce = 7.0f;
    loadPlaneModelButtonElement.button.text = "";
    loadPlaneModelButtonElement.button.textRatio = 0.022f;
    loadPlaneModelButtonElement.panel = 1;
    loadPlaneModelButtonElement.attachedToMainPanel = true;
	loadPlaneModelButtonElement.type = "button";
    UIElements.push_back(loadPlaneModelButtonElement);//7

	//Load sphere button 
	UIElement loadSphereModelButtonElement;
    loadSphereModelButtonElement.button.positionX = 0.0f;
    loadSphereModelButtonElement.button.positionY = -0.2f;
    loadSphereModelButtonElement.button.positionZ = 0.9f;
    loadSphereModelButtonElement.button.width = 0.008f;
    loadSphereModelButtonElement.button.height = 0.045f;
    loadSphereModelButtonElement.button.color = colorData.buttonColor;
    loadSphereModelButtonElement.button.colorHover = colorData.buttonColorHover;
    loadSphereModelButtonElement.button.transitionMixVal = 0.0f;
    loadSphereModelButtonElement.button.buttonCurveReduce = 7.0f;
    loadSphereModelButtonElement.button.text = "";
    loadSphereModelButtonElement.button.textRatio = 0.022f;
    loadSphereModelButtonElement.panel = 1;
    loadSphereModelButtonElement.attachedToMainPanel = true;
	loadSphereModelButtonElement.type = "button";
	UIElements.push_back(loadSphereModelButtonElement);//8


	UIElement loadCustomModelButtonElement;
    loadCustomModelButtonElement.button.positionX = 0.0f;
    loadCustomModelButtonElement.button.positionY = -0.3f;
    loadCustomModelButtonElement.button.positionZ = 0.9f;
    loadCustomModelButtonElement.button.width = 0.06f;
    loadCustomModelButtonElement.button.height = 0.04f;
    loadCustomModelButtonElement.button.color = colorData.buttonColor;
    loadCustomModelButtonElement.button.colorHover = colorData.buttonColorHover;
    loadCustomModelButtonElement.button.transitionMixVal = 0.0f;
    loadCustomModelButtonElement.button.buttonCurveReduce = 10.0f;
    loadCustomModelButtonElement.button.text = "Custom";
    loadCustomModelButtonElement.button.textRatio = 0.022f;
    loadCustomModelButtonElement.panel = 1;
    loadCustomModelButtonElement.attachedToMainPanel = true;
    loadCustomModelButtonElement.type = "button";
    UIElements.push_back(loadCustomModelButtonElement);//9


     //---------------------------PAINTING PANEL---------------------------

    UIElement addBrushMaskTextureButtonElement;
    addBrushMaskTextureButtonElement.button.positionX = 0.0f;
    addBrushMaskTextureButtonElement.button.positionY = 0.85f;
    addBrushMaskTextureButtonElement.button.positionZ = 0.9f;
    addBrushMaskTextureButtonElement.button.width = 0.1f;
    addBrushMaskTextureButtonElement.button.height = 0.04f;
    addBrushMaskTextureButtonElement.button.color = colorData.buttonColor;
    addBrushMaskTextureButtonElement.button.colorHover = colorData.buttonColorHover;
    addBrushMaskTextureButtonElement.button.transitionMixVal = 0.0f;
    addBrushMaskTextureButtonElement.button.buttonCurveReduce = 10.0f;
    addBrushMaskTextureButtonElement.button.text = "Add Mask Texture";
    addBrushMaskTextureButtonElement.button.textRatio = 0.075f;
    addBrushMaskTextureButtonElement.panel = 3;
    addBrushMaskTextureButtonElement.attachedToMainPanel = true;
    addBrushMaskTextureButtonElement.type = "button";
    UIElements.push_back(addBrushMaskTextureButtonElement);//10
		
    UIElement useNegativeCheckBoxElement;
    useNegativeCheckBoxElement.checkBox.positionX = -0.03f;
    useNegativeCheckBoxElement.checkBox.positionY = 0.45f;
    useNegativeCheckBoxElement.checkBox.text = "Use Negative";
    useNegativeCheckBoxElement.panel = 3;
    useNegativeCheckBoxElement.attachedToMainPanel = true;
    useNegativeCheckBoxElement.type = "checkBox";
	UIElements.push_back(useNegativeCheckBoxElement);//11

    UIElement maskTextureFileNameTextElement;
    maskTextureFileNameTextElement.text.positionX = -0.03f;
    maskTextureFileNameTextElement.text.positionY = 0.37f;
    maskTextureFileNameTextElement.text.text = "";
    maskTextureFileNameTextElement.text.scale = 0.00022f;
    maskTextureFileNameTextElement.text.color = colorData.textColor;
    maskTextureFileNameTextElement.panel = 3;
    maskTextureFileNameTextElement.attachedToMainPanel = true;
    maskTextureFileNameTextElement.type = "text";
    UIElements.push_back(maskTextureFileNameTextElement);//12
	 
	//Brush size range bar
    UIElement brushSizeTextElement;
    brushSizeTextElement.text.positionX = -0.11f;
    brushSizeTextElement.text.positionY = 0.25f+0.06f-0.035f;
    brushSizeTextElement.text.text = "Size";
    brushSizeTextElement.text.scale = 0.00022f;
    brushSizeTextElement.text.color = colorData.textBoxColor;
    brushSizeTextElement.panel = 3;
    brushSizeTextElement.attachedToMainPanel = true;
    brushSizeTextElement.type = "text";
    UIElements.push_back(brushSizeTextElement);//13

	UIElement brushSizeRangeBarElement;
	brushSizeRangeBarElement.rangeBar.positionX = 0.0f;
	brushSizeRangeBarElement.rangeBar.positionY = 0.22f+0.02f+0.02f;
	brushSizeRangeBarElement.rangeBar.value = 0.0f;
	brushSizeRangeBarElement.panel = 3;
	brushSizeRangeBarElement.attachedToMainPanel = true;
	brushSizeRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushSizeRangeBarElement);//14

	//Brush blur range bar
    UIElement brushBlurTextElement;
    brushBlurTextElement.text.positionX = -0.11f;
    brushBlurTextElement.text.positionY = +0.125f+0.08f-0.035f;
    brushBlurTextElement.text.text = "Blur";
    brushBlurTextElement.text.scale = 0.00022f;
    brushBlurTextElement.text.color = colorData.textBoxColor;
    brushBlurTextElement.panel = 3;
    brushBlurTextElement.attachedToMainPanel = true;
    brushBlurTextElement.type = "text";
    UIElements.push_back(brushBlurTextElement);//15

	UIElement brushBlurRangeBarElement;
	brushBlurRangeBarElement.rangeBar.positionX = 0.0f;
	brushBlurRangeBarElement.rangeBar.positionY = 0.09f+0.04f+0.02f;
	brushBlurRangeBarElement.rangeBar.value = -0.11f;
	brushBlurRangeBarElement.panel = 3;
	brushBlurRangeBarElement.attachedToMainPanel = true;
	brushBlurRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushBlurRangeBarElement);//16

	//Brush rotation range bar
    UIElement brushRotationTextElement;
    brushRotationTextElement.text.positionX = -0.11f;
    brushRotationTextElement.text.positionY = -0.005f+0.1f-0.035f;
    brushRotationTextElement.text.text = "Rotation";
    brushRotationTextElement.text.scale = 0.00022f;
    brushRotationTextElement.text.color = colorData.textBoxColor;
    brushRotationTextElement.panel = 3;
    brushRotationTextElement.attachedToMainPanel = true;
    brushRotationTextElement.type = "text";
    UIElements.push_back(brushRotationTextElement);//17

	UIElement brushRotationRangeBarElement;
	brushRotationRangeBarElement.rangeBar.positionX = 0.0f;
	brushRotationRangeBarElement.rangeBar.positionY = -0.04f+0.06f+0.02f;
	brushRotationRangeBarElement.rangeBar.value = -0.11f;
	brushRotationRangeBarElement.panel = 3;
	brushRotationRangeBarElement.attachedToMainPanel = true;
	brushRotationRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushRotationRangeBarElement);//18

	//Brush opacity range bar
    UIElement brushOpacityTextElement;
    brushOpacityTextElement.text.positionX = -0.11f;
    brushOpacityTextElement.text.positionY = -0.135f+0.12f-0.035f;
    brushOpacityTextElement.text.text = "Opacity";
    brushOpacityTextElement.text.scale = 0.00022f;
    brushOpacityTextElement.text.color = colorData.textBoxColor;
    brushOpacityTextElement.panel = 3;
    brushOpacityTextElement.attachedToMainPanel = true;
    brushOpacityTextElement.type = "text";
    UIElements.push_back(brushOpacityTextElement);//19

	UIElement brushOpacityRangeBarElement;
	brushOpacityRangeBarElement.rangeBar.positionX = 0.0f;
	brushOpacityRangeBarElement.rangeBar.positionY = -0.17f+0.08f+0.02f;
	brushOpacityRangeBarElement.rangeBar.value = 0.11f;
	brushOpacityRangeBarElement.panel = 3;
	brushOpacityRangeBarElement.attachedToMainPanel = true;
	brushOpacityRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushOpacityRangeBarElement);//20
    
	//Brush spacing range bar
    UIElement brushSpacingTextElement;
    brushSpacingTextElement.text.positionX = -0.11f;
    brushSpacingTextElement.text.positionY = -0.265f+0.14f-0.035f;
    brushSpacingTextElement.text.text = "Spacing";
    brushSpacingTextElement.text.scale = 0.00022f;
    brushSpacingTextElement.text.color = colorData.textBoxColor;
    brushSpacingTextElement.panel = 3;
    brushSpacingTextElement.attachedToMainPanel = true;
    brushSpacingTextElement.type = "text";
    UIElements.push_back(brushSpacingTextElement);//21

	UIElement brushSpacingRangeBarElement;
	brushSpacingRangeBarElement.rangeBar.positionX = 0.0f;
	brushSpacingRangeBarElement.rangeBar.positionY = -0.30f+0.1f+0.02f;
	brushSpacingRangeBarElement.rangeBar.value = -0.11f;
	brushSpacingRangeBarElement.panel = 3;
	brushSpacingRangeBarElement.attachedToMainPanel = true;
	brushSpacingRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushSpacingRangeBarElement);//22

	//Brush spacing range bar
    UIElement brushBordersTextElement;
    brushBordersTextElement.text.positionX = -0.11f;
    brushBordersTextElement.text.positionY = -0.285f + 0.02f;
    brushBordersTextElement.text.text = "Borders";
    brushBordersTextElement.text.scale = 0.00022f;
    brushBordersTextElement.text.color = colorData.textBoxColor;
    brushBordersTextElement.panel = 3;
    brushBordersTextElement.attachedToMainPanel = true;
    brushBordersTextElement.type = "text";
    UIElements.push_back(brushBordersTextElement);//23

	UIElement brushBordersRangeBarElement;
	brushBordersRangeBarElement.rangeBar.positionX = 0.0f;
	brushBordersRangeBarElement.rangeBar.positionY = -0.31f+0.02f;
	brushBordersRangeBarElement.rangeBar.value = 0.06f;
	brushBordersRangeBarElement.panel = 3;
	brushBordersRangeBarElement.attachedToMainPanel = true;
	brushBordersRangeBarElement.type = "rangeBar";
    UIElements.push_back(brushBordersRangeBarElement);//24

	UIElement mirrorXCheckBoxElement;
    mirrorXCheckBoxElement.checkBox.positionX = -0.16f;
    mirrorXCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorXCheckBoxElement.checkBox.text = "X";
    mirrorXCheckBoxElement.panel = 3;
    mirrorXCheckBoxElement.attachedToMainPanel = false;
    mirrorXCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorXCheckBoxElement);//25

	UIElement mirrorYCheckBoxElement;
    mirrorYCheckBoxElement.checkBox.positionX = -0.10f;
    mirrorYCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorYCheckBoxElement.checkBox.text = "Y";
    mirrorYCheckBoxElement.panel = 3;
    mirrorYCheckBoxElement.attachedToMainPanel = false;
    mirrorYCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorYCheckBoxElement);//26

	UIElement mirrorZCheckBoxElement;
    mirrorZCheckBoxElement.checkBox.positionX = -0.04f;
    mirrorZCheckBoxElement.checkBox.positionY = 0.9f;
    mirrorZCheckBoxElement.checkBox.text = "Z";
    mirrorZCheckBoxElement.panel = 3;
    mirrorZCheckBoxElement.attachedToMainPanel = false;
    mirrorZCheckBoxElement.type = "checkBox";
    UIElements.push_back(mirrorZCheckBoxElement);//27
	
    //---------------------------EXPORT PANEL---------------------------

	
    UIElement exportingPathTextBoxElement;
    exportingPathTextBoxElement.textBox.position_x = 0.0f;
    exportingPathTextBoxElement.textBox.position_y = 0.6f;
    exportingPathTextBoxElement.textBox.position_z = 0.9f;
    exportingPathTextBoxElement.textBox.width = 0.12;
    exportingPathTextBoxElement.textBox.height = 0.03;
    exportingPathTextBoxElement.textBox.text = "Choose Destination Path";
    exportingPathTextBoxElement.panel = 4;
    exportingPathTextBoxElement.attachedToMainPanel = true;
	exportingPathTextBoxElement.type = "textBox";
    UIElements.push_back(exportingPathTextBoxElement);//28

    UIElement exportingFolderNameTextBoxElement;
    exportingFolderNameTextBoxElement.textBox.position_x = 0.0f;
    exportingFolderNameTextBoxElement.textBox.position_y = 0.5f;
    exportingFolderNameTextBoxElement.textBox.position_z = 0.9f;
    exportingFolderNameTextBoxElement.textBox.width = 0.12;
    exportingFolderNameTextBoxElement.textBox.height = 0.03;
    exportingFolderNameTextBoxElement.textBox.text = "LP_Export";
    exportingFolderNameTextBoxElement.panel = 4;
    exportingFolderNameTextBoxElement.attachedToMainPanel = true;
    exportingFolderNameTextBoxElement.type = "textBox";
    UIElements.push_back(exportingFolderNameTextBoxElement);//29

    UIElement jpgCheckBoxElement;
    jpgCheckBoxElement.checkBox.positionX = -0.11f;
    jpgCheckBoxElement.checkBox.positionY = 0.4f;
    jpgCheckBoxElement.checkBox.text = "";
    jpgCheckBoxElement.panel = 4;
    jpgCheckBoxElement.attachedToMainPanel = true;
    jpgCheckBoxElement.type = "checkBox";
    UIElements.push_back(jpgCheckBoxElement);//30

    UIElement pngCheckBoxElement;
    pngCheckBoxElement.checkBox.positionX = 0.05f;
    pngCheckBoxElement.checkBox.positionY = 0.4f;
    pngCheckBoxElement.checkBox.text = "";
    pngCheckBoxElement.panel = 4;
    pngCheckBoxElement.attachedToMainPanel = true;
    pngCheckBoxElement.type = "checkBox";
    UIElements.push_back(pngCheckBoxElement);//31

    UIElement downloadButtonElement;
    downloadButtonElement.button.positionX = 0.0f;
    downloadButtonElement.button.positionY = 0.2f;
    downloadButtonElement.button.positionZ = 0.9f;
    downloadButtonElement.button.width = 0.1f;
    downloadButtonElement.button.height = 0.04f;
    downloadButtonElement.button.color = colorData.buttonColor;
    downloadButtonElement.button.colorHover = colorData.buttonColorHover;
    downloadButtonElement.button.transitionMixVal = 0.0f;
    downloadButtonElement.button.buttonCurveReduce = 10.0f;
    downloadButtonElement.button.text = "Download";
    downloadButtonElement.button.textRatio = 0.045f;
    downloadButtonElement.panel = 4;
    downloadButtonElement.attachedToMainPanel = true;
    downloadButtonElement.type = "button";
    UIElements.push_back(downloadButtonElement);//32



	//-------------ICONS-------------


    UIElement jpgIconElement;
    jpgIconElement.icon.positionX = -0.06f;
    jpgIconElement.icon.positionY = 0.4f;
    jpgIconElement.icon.positionZ = 0.9f;
    jpgIconElement.icon.width = 0.05f;
    jpgIconElement.icon.height = 0.065f;
    jpgIconElement.icon.color = colorData.iconColor;
    jpgIconElement.icon.colorHover = colorData.iconColorHover;
    jpgIconElement.icon.mixVal = 0.0f;
    jpgIconElement.icon.icon = icons.JpgFile;
    jpgIconElement.panel = 4;
    jpgIconElement.attachedToMainPanel = true;
    jpgIconElement.type = "icon";
    UIElements.push_back(jpgIconElement);//33

    UIElement pngIconElement;
    pngIconElement.icon.positionX = +0.1f;
    pngIconElement.icon.positionY = 0.4f;
    pngIconElement.icon.positionZ = 0.9f;
    pngIconElement.icon.width = 0.05f;
    pngIconElement.icon.height = 0.065f;
    pngIconElement.icon.color = colorData.iconColor;
    pngIconElement.icon.colorHover = colorData.iconColorHover;
    pngIconElement.icon.mixVal = 0.0f;
    pngIconElement.icon.icon = icons.PngFile;
    pngIconElement.panel = 4;
    pngIconElement.attachedToMainPanel = true;
    pngIconElement.type = "icon";
    UIElements.push_back(pngIconElement);//34

	UIElement folderIconElement;
    folderIconElement.icon.positionX = 0.1f;
    folderIconElement.icon.positionY = 0.6f;
    folderIconElement.icon.positionZ = 0.99f;
    folderIconElement.icon.width = 0.02f;
    folderIconElement.icon.height = 0.04f;
    folderIconElement.icon.color = colorData.iconColor;
    folderIconElement.icon.colorHover = colorData.iconColorHover;
    folderIconElement.icon.mixVal = 0.0f;
    folderIconElement.icon.icon = icons.Folder;
    folderIconElement.panel = 1;
    folderIconElement.attachedToMainPanel = true;
    folderIconElement.type = "icon";
    UIElements.push_back(folderIconElement);//35

	UIElement planeIconElement;
    planeIconElement.icon.positionX = 0.0f;
    planeIconElement.icon.positionY = -0.1f;
    planeIconElement.icon.positionZ = 0.99f;
    planeIconElement.icon.width = 0.025f;
    planeIconElement.icon.height = 0.035f;
    planeIconElement.icon.color = colorData.iconColor;
    planeIconElement.icon.colorHover = colorData.iconColorHover;
    planeIconElement.icon.mixVal = 0.0f;
    planeIconElement.icon.icon = icons.Panel;
    planeIconElement.panel = 1;
    planeIconElement.attachedToMainPanel = true;
	planeIconElement.type = "icon";
    UIElements.push_back(planeIconElement);//36

    UIElement sphereIconElement;
    sphereIconElement.icon.positionX = -0.002f;
    sphereIconElement.icon.positionY = -0.2f;
    sphereIconElement.icon.positionZ = 0.99f;
    sphereIconElement.icon.width = 0.03f;
    sphereIconElement.icon.height = 0.04f;
    sphereIconElement.icon.color = colorData.iconColor;
    sphereIconElement.icon.colorHover = colorData.iconColorHover;
    sphereIconElement.icon.mixVal = 0.0f;
    sphereIconElement.icon.icon = icons.Sphere;
    sphereIconElement.panel = 1;
    sphereIconElement.attachedToMainPanel = true;
	sphereIconElement.type = "icon";
    UIElements.push_back(sphereIconElement);//37

	return UIElements;
}