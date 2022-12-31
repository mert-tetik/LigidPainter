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


UI UserInterface::getUiElements(Icons icons) {

    std::map<std::string, UIElement> UIElements;
	std::vector<std::string> uiIndex;

	ColorData colorData;


	//---------------------------MODEL PANEL---------------------------

	UIElement uploadingModelPathTextBox;
    uploadingModelPathTextBox.textBox.position_x = 0.0f;
    uploadingModelPathTextBox.textBox.position_y = 0.6f;
    uploadingModelPathTextBox.textBox.position_z = 0.9f;
    uploadingModelPathTextBox.textBox.width = 0.12;
    uploadingModelPathTextBox.textBox.height = 0.03;
    uploadingModelPathTextBox.textBox.text = "";
    uploadingModelPathTextBox.panel = 1;
    uploadingModelPathTextBox.attachedToMainPanel = true;
	uploadingModelPathTextBox.type = "textBox";
    UIElements["uploadingModelPathTextBox"] = uploadingModelPathTextBox;
    uiIndex.push_back("uploadingModelPathTextBox");//1

	UIElement uploadingModelPathText;
    uploadingModelPathText.text.positionX = -0.05f;
    uploadingModelPathText.text.positionY = 0.64f;
    uploadingModelPathText.text.text = "File Path";
    uploadingModelPathText.text.scale = 0.00022f;
    uploadingModelPathText.text.color = colorData.textBoxColor;
    uploadingModelPathText.panel = 1;
    uploadingModelPathText.attachedToMainPanel = true;
    uploadingModelPathText.type = "text";
	UIElements["uploadingModelPathText"] = uploadingModelPathText;
	uiIndex.push_back("uploadingModelPathText");//2

	UIElement loadModelButton;
    loadModelButton.button.positionX = 0.0f;
    loadModelButton.button.positionY = 0.45f;
    loadModelButton.button.positionZ = 0.9f;
    loadModelButton.button.width = 0.08f;
    loadModelButton.button.height = 0.04f;
    loadModelButton.button.color = colorData.buttonColor;
    loadModelButton.button.colorHover = colorData.buttonColorHover;
    loadModelButton.button.transitionMixVal = 0.0f;
    loadModelButton.button.buttonCurveReduce = 10.0f;
    loadModelButton.button.text = "Load";
    loadModelButton.button.textRatio = 0.022f;
    loadModelButton.panel = 1;
    loadModelButton.attachedToMainPanel = true;
	loadModelButton.type = "button";
    UIElements["loadModelButton"] = loadModelButton;
    uiIndex.push_back("loadModelButton");//3

	UIElement autoTriangulateCheckBox;
    autoTriangulateCheckBox.checkBox.positionX = -0.08f;
    autoTriangulateCheckBox.checkBox.positionY = 0.3f;
    autoTriangulateCheckBox.checkBox.text = "Auto triangulate";
    autoTriangulateCheckBox.panel = 1;
    autoTriangulateCheckBox.attachedToMainPanel = true;
	autoTriangulateCheckBox.type = "checkBox";
    UIElements["autoTriangulateCheckBox"] = autoTriangulateCheckBox;
    uiIndex.push_back("autoTriangulateCheckBox");//4
	

	UIElement backfaceCullingCheckBox;
    backfaceCullingCheckBox.checkBox.positionX = -0.08f;
    backfaceCullingCheckBox.checkBox.positionY = 0.2f;
    backfaceCullingCheckBox.checkBox.text = "Backface Culling";
    backfaceCullingCheckBox.panel = 1;
    backfaceCullingCheckBox.attachedToMainPanel = true;
	backfaceCullingCheckBox.type = "checkBox";
    UIElements["backfaceCullingCheckBox"] = backfaceCullingCheckBox;
    uiIndex.push_back("backfaceCullingCheckBox");//5


	UIElement modelPanel3DModelsText;
    modelPanel3DModelsText.text.positionX = -0.05f;
    modelPanel3DModelsText.text.positionY = 0.08f-0.1f;
    modelPanel3DModelsText.text.text = "3D Models";
    modelPanel3DModelsText.text.scale = 0.00022f;
    modelPanel3DModelsText.text.color = colorData.textBoxColor;
    modelPanel3DModelsText.panel = 1;
    modelPanel3DModelsText.attachedToMainPanel = true;
	modelPanel3DModelsText.type = "text";
    UIElements["modelPanel3DModelsText"] = modelPanel3DModelsText;
    uiIndex.push_back("modelPanel3DModelsText");//6

	//Load plane button
	UIElement loadPlaneModelButton;
    loadPlaneModelButton.button.positionX = 0.0f;
    loadPlaneModelButton.button.positionY = -0.1f;
    loadPlaneModelButton.button.positionZ = 0.9f;
    loadPlaneModelButton.button.width = 0.008f;
    loadPlaneModelButton.button.height = 0.045f;
    loadPlaneModelButton.button.color = colorData.buttonColor;
    loadPlaneModelButton.button.colorHover = colorData.buttonColorHover;
    loadPlaneModelButton.button.transitionMixVal = 0.0f;
    loadPlaneModelButton.button.buttonCurveReduce = 7.0f;
    loadPlaneModelButton.button.text = "";
    loadPlaneModelButton.button.textRatio = 0.022f;
    loadPlaneModelButton.panel = 1;
    loadPlaneModelButton.attachedToMainPanel = true;
	loadPlaneModelButton.type = "button";
    UIElements["loadPlaneModelButton"] = loadPlaneModelButton;
    uiIndex.push_back("loadPlaneModelButton");//7

	//Load sphere button 
	UIElement loadSphereModelButton;
    loadSphereModelButton.button.positionX = 0.0f;
    loadSphereModelButton.button.positionY = -0.2f;
    loadSphereModelButton.button.positionZ = 0.9f;
    loadSphereModelButton.button.width = 0.008f;
    loadSphereModelButton.button.height = 0.045f;
    loadSphereModelButton.button.color = colorData.buttonColor;
    loadSphereModelButton.button.colorHover = colorData.buttonColorHover;
    loadSphereModelButton.button.transitionMixVal = 0.0f;
    loadSphereModelButton.button.buttonCurveReduce = 7.0f;
    loadSphereModelButton.button.text = "";
    loadSphereModelButton.button.textRatio = 0.022f;
    loadSphereModelButton.panel = 1;
    loadSphereModelButton.attachedToMainPanel = true;
	loadSphereModelButton.type = "button";
    UIElements["loadSphereModelButton"] = loadSphereModelButton;
	uiIndex.push_back("loadSphereModelButton");//8


	UIElement loadCustomModelButton;
    loadCustomModelButton.button.positionX = 0.0f;
    loadCustomModelButton.button.positionY = -0.3f;
    loadCustomModelButton.button.positionZ = 0.9f;
    loadCustomModelButton.button.width = 0.06f;
    loadCustomModelButton.button.height = 0.04f;
    loadCustomModelButton.button.color = colorData.buttonColor;
    loadCustomModelButton.button.colorHover = colorData.buttonColorHover;
    loadCustomModelButton.button.transitionMixVal = 0.0f;
    loadCustomModelButton.button.buttonCurveReduce = 10.0f;
    loadCustomModelButton.button.text = "Custom";
    loadCustomModelButton.button.textRatio = 0.022f;
    loadCustomModelButton.panel = 1;
    loadCustomModelButton.attachedToMainPanel = true;
    loadCustomModelButton.type = "button";
    UIElements["loadCustomModelButton"] = loadCustomModelButton;
    uiIndex.push_back("loadCustomModelButton");//9


     //---------------------------PAINTING PANEL---------------------------

    UIElement addBrushMaskTextureButton;
    addBrushMaskTextureButton.button.positionX = 0.0f;
    addBrushMaskTextureButton.button.positionY = 0.9f;
    addBrushMaskTextureButton.button.positionZ = 0.9f;
    addBrushMaskTextureButton.button.width = 0.1f;
    addBrushMaskTextureButton.button.height = 0.04f;
    addBrushMaskTextureButton.button.color = colorData.buttonColor;
    addBrushMaskTextureButton.button.colorHover = colorData.buttonColorHover;
    addBrushMaskTextureButton.button.transitionMixVal = 0.0f;
    addBrushMaskTextureButton.button.buttonCurveReduce = 10.0f;
    addBrushMaskTextureButton.button.text = "Add Mask Texture";
    addBrushMaskTextureButton.button.textRatio = 0.075f;
    addBrushMaskTextureButton.panel = 3;
    addBrushMaskTextureButton.attachedToMainPanel = true;
    addBrushMaskTextureButton.type = "button";
    UIElements["addBrushMaskTextureButton"] = addBrushMaskTextureButton;
    uiIndex.push_back("addBrushMaskTextureButton");//10
		
    UIElement useNegativeCheckBox;
    useNegativeCheckBox.checkBox.positionX = -0.03f;
    useNegativeCheckBox.checkBox.positionY = 0.45f;
    useNegativeCheckBox.checkBox.text = "Use Negative";
    useNegativeCheckBox.panel = 3;
    useNegativeCheckBox.attachedToMainPanel = true;
    useNegativeCheckBox.type = "checkBox";
    UIElements["useNegativeCheckBox"] = useNegativeCheckBox;
	uiIndex.push_back("useNegativeCheckBox");//11

    UIElement maskTextureFileNameText;
    maskTextureFileNameText.text.positionX = -0.03f;
    maskTextureFileNameText.text.positionY = 0.37f;
    maskTextureFileNameText.text.text = "";
    maskTextureFileNameText.text.scale = 0.00022f;
    maskTextureFileNameText.text.color = colorData.textColor;
    maskTextureFileNameText.panel = 3;
    maskTextureFileNameText.attachedToMainPanel = true;
    maskTextureFileNameText.type = "text";
    UIElements["maskTextureFileNameText"] = maskTextureFileNameText;
    uiIndex.push_back("maskTextureFileNameText");//12
	 
	//Brush size range bar
    UIElement brushSizeText;
    brushSizeText.text.positionX = -0.11f;
    brushSizeText.text.positionY = 0.25f+0.06f-0.035f;
    brushSizeText.text.text = "Size";
    brushSizeText.text.scale = 0.00022f;
    brushSizeText.text.color = colorData.textBoxColor;
    brushSizeText.panel = 3;
    brushSizeText.attachedToMainPanel = true;
    brushSizeText.type = "text";
    UIElements["brushSizeText"] = brushSizeText;
    uiIndex.push_back("brushSizeText");//13

	UIElement brushSizeRangeBar;
	brushSizeRangeBar.rangeBar.positionX = 0.0f;
	brushSizeRangeBar.rangeBar.positionY = 0.22f+0.02f+0.02f;
	brushSizeRangeBar.rangeBar.value = 0.0f;
	brushSizeRangeBar.panel = 3;
	brushSizeRangeBar.attachedToMainPanel = true;
	brushSizeRangeBar.type = "rangeBar";
    UIElements["brushSizeRangeBar"] = brushSizeRangeBar;
    uiIndex.push_back("brushSizeRangeBar");//14

	//Brush blur range bar
    UIElement brushBlurText;
    brushBlurText.text.positionX = -0.11f;
    brushBlurText.text.positionY = +0.125f+0.08f-0.035f;
    brushBlurText.text.text = "Blur";
    brushBlurText.text.scale = 0.00022f;
    brushBlurText.text.color = colorData.textBoxColor;
    brushBlurText.panel = 3;
    brushBlurText.attachedToMainPanel = true;
    brushBlurText.type = "text";
    UIElements["brushBlurText"] = brushBlurText;
    uiIndex.push_back("brushBlurText");//15

	UIElement brushBlurRangeBar;
	brushBlurRangeBar.rangeBar.positionX = 0.0f;
	brushBlurRangeBar.rangeBar.positionY = 0.09f+0.04f+0.02f;
	brushBlurRangeBar.rangeBar.value = -0.11f;
	brushBlurRangeBar.panel = 3;
	brushBlurRangeBar.attachedToMainPanel = true;
	brushBlurRangeBar.type = "rangeBar";
    UIElements["brushBlurRangeBar"] = brushBlurRangeBar;
    uiIndex.push_back("brushBlurRangeBar");//16

	//Brush rotation range bar
    UIElement brushRotationText;
    brushRotationText.text.positionX = -0.11f;
    brushRotationText.text.positionY = -0.005f+0.1f-0.035f;
    brushRotationText.text.text = "Rotation";
    brushRotationText.text.scale = 0.00022f;
    brushRotationText.text.color = colorData.textBoxColor;
    brushRotationText.panel = 3;
    brushRotationText.attachedToMainPanel = true;
    brushRotationText.type = "text";
    UIElements["brushRotationText"] = brushRotationText;
    uiIndex.push_back("brushRotationText");//17

	UIElement brushRotationRangeBar;
	brushRotationRangeBar.rangeBar.positionX = 0.0f;
	brushRotationRangeBar.rangeBar.positionY = -0.04f+0.06f+0.02f;
	brushRotationRangeBar.rangeBar.value = -0.11f;
	brushRotationRangeBar.panel = 3;
	brushRotationRangeBar.attachedToMainPanel = true;
	brushRotationRangeBar.type = "rangeBar";
    UIElements["brushRotationRangeBar"] = brushRotationRangeBar;
    uiIndex.push_back("brushRotationRangeBar");//18

	//Brush opacity range bar
    UIElement brushOpacityText;
    brushOpacityText.text.positionX = -0.11f;
    brushOpacityText.text.positionY = -0.135f+0.12f-0.035f;
    brushOpacityText.text.text = "Opacity";
    brushOpacityText.text.scale = 0.00022f;
    brushOpacityText.text.color = colorData.textBoxColor;
    brushOpacityText.panel = 3;
    brushOpacityText.attachedToMainPanel = true;
    brushOpacityText.type = "text";
    UIElements["brushOpacityText"] = brushOpacityText;
    uiIndex.push_back("brushOpacityText");//19

	UIElement brushOpacityRangeBar;
	brushOpacityRangeBar.rangeBar.positionX = 0.0f;
	brushOpacityRangeBar.rangeBar.positionY = -0.17f+0.08f+0.02f;
	brushOpacityRangeBar.rangeBar.value = 0.11f;
	brushOpacityRangeBar.panel = 3;
	brushOpacityRangeBar.attachedToMainPanel = true;
	brushOpacityRangeBar.type = "rangeBar";
    UIElements["brushOpacityRangeBar"] = brushOpacityRangeBar;
    uiIndex.push_back("brushOpacityRangeBar");//20
    
	//Brush spacing range bar
    UIElement brushSpacingText;
    brushSpacingText.text.positionX = -0.11f;
    brushSpacingText.text.positionY = -0.265f+0.14f-0.035f;
    brushSpacingText.text.text = "Spacing";
    brushSpacingText.text.scale = 0.00022f;
    brushSpacingText.text.color = colorData.textBoxColor;
    brushSpacingText.panel = 3;
    brushSpacingText.attachedToMainPanel = true;
    brushSpacingText.type = "text";
    UIElements["brushSpacingText"] = brushSpacingText;
    uiIndex.push_back("brushSpacingText");//21

	UIElement brushSpacingRangeBar;
	brushSpacingRangeBar.rangeBar.positionX = 0.0f;
	brushSpacingRangeBar.rangeBar.positionY = -0.30f+0.1f+0.02f;
	brushSpacingRangeBar.rangeBar.value = -0.11f;
	brushSpacingRangeBar.panel = 3;
	brushSpacingRangeBar.attachedToMainPanel = true;
	brushSpacingRangeBar.type = "rangeBar";
    UIElements["brushSpacingRangeBar"] = brushSpacingRangeBar;
    uiIndex.push_back("brushSpacingRangeBar");//22

	//Brush spacing range bar
    UIElement brushBordersText;
    brushBordersText.text.positionX = -0.11f;
    brushBordersText.text.positionY = -0.285f + 0.02f;
    brushBordersText.text.text = "Borders";
    brushBordersText.text.scale = 0.00022f;
    brushBordersText.text.color = colorData.textBoxColor;
    brushBordersText.panel = 3;
    brushBordersText.attachedToMainPanel = true;
    brushBordersText.type = "text";
    UIElements["brushBordersText"] = brushBordersText;
    uiIndex.push_back("brushBordersText");//23

	UIElement brushBordersRangeBar;
	brushBordersRangeBar.rangeBar.positionX = 0.0f;
	brushBordersRangeBar.rangeBar.positionY = -0.31f+0.02f;
	brushBordersRangeBar.rangeBar.value = 0.06f;
	brushBordersRangeBar.panel = 3;
	brushBordersRangeBar.attachedToMainPanel = true;
	brushBordersRangeBar.type = "rangeBar";
    UIElements["brushBordersRangeBar"] = brushBordersRangeBar;
    uiIndex.push_back("brushBordersRangeBar");//24

	UIElement mirrorXCheckBox;
    mirrorXCheckBox.checkBox.positionX = -0.16f;
    mirrorXCheckBox.checkBox.positionY = 0.9f;
    mirrorXCheckBox.checkBox.text = "X";
    mirrorXCheckBox.panel = 3;
    mirrorXCheckBox.attachedToMainPanel = false;
    mirrorXCheckBox.type = "checkBox";
    UIElements["mirrorXCheckBox"] = mirrorXCheckBox;
    uiIndex.push_back("mirrorXCheckBox");//25

	UIElement mirrorYCheckBox;
    mirrorYCheckBox.checkBox.positionX = -0.10f;
    mirrorYCheckBox.checkBox.positionY = 0.9f;
    mirrorYCheckBox.checkBox.text = "Y";
    mirrorYCheckBox.panel = 3;
    mirrorYCheckBox.attachedToMainPanel = false;
    mirrorYCheckBox.type = "checkBox";
    UIElements["mirrorYCheckBox"] = mirrorYCheckBox;
    uiIndex.push_back("mirrorYCheckBox");//26

	UIElement mirrorZCheckBox;
    mirrorZCheckBox.checkBox.positionX = -0.04f;
    mirrorZCheckBox.checkBox.positionY = 0.9f;
    mirrorZCheckBox.checkBox.text = "Z";
    mirrorZCheckBox.panel = 3;
    mirrorZCheckBox.attachedToMainPanel = false;
    mirrorZCheckBox.type = "checkBox";
    UIElements["mirrorZCheckBox"] = mirrorZCheckBox;
    uiIndex.push_back("mirrorZCheckBox");//27
	
    //---------------------------EXPORT PANEL---------------------------

	
    UIElement exportingPathTextBox;
    exportingPathTextBox.textBox.position_x = 0.0f;
    exportingPathTextBox.textBox.position_y = 0.6f;
    exportingPathTextBox.textBox.position_z = 0.9f;
    exportingPathTextBox.textBox.width = 0.12;
    exportingPathTextBox.textBox.height = 0.03;
    exportingPathTextBox.textBox.text = "Choose Destination Path";
    exportingPathTextBox.panel = 4;
    exportingPathTextBox.attachedToMainPanel = true;
	exportingPathTextBox.type = "textBox";
    UIElements["exportingPathTextBox"] = exportingPathTextBox;
    uiIndex.push_back("exportingPathTextBox");//28

    UIElement exportingFolderNameTextBox;
    exportingFolderNameTextBox.textBox.position_x = 0.0f;
    exportingFolderNameTextBox.textBox.position_y = 0.5f;
    exportingFolderNameTextBox.textBox.position_z = 0.9f;
    exportingFolderNameTextBox.textBox.width = 0.12;
    exportingFolderNameTextBox.textBox.height = 0.03;
    exportingFolderNameTextBox.textBox.text = "LP_Export";
    exportingFolderNameTextBox.panel = 4;
    exportingFolderNameTextBox.attachedToMainPanel = true;
    exportingFolderNameTextBox.type = "textBox";
    UIElements["exportingFolderNameTextBox"] = exportingFolderNameTextBox;
    uiIndex.push_back("exportingFolderNameTextBox");//29

    UIElement jpgCheckBox;
    jpgCheckBox.checkBox.positionX = -0.11f;
    jpgCheckBox.checkBox.positionY = 0.4f;
    jpgCheckBox.checkBox.text = "";
    jpgCheckBox.panel = 4;
    jpgCheckBox.attachedToMainPanel = true;
    jpgCheckBox.type = "checkBox";
    UIElements["jpgCheckBox"] = jpgCheckBox;
    uiIndex.push_back("jpgCheckBox");//30

    UIElement pngCheckBox;
    pngCheckBox.checkBox.positionX = 0.05f;
    pngCheckBox.checkBox.positionY = 0.4f;
    pngCheckBox.checkBox.text = "";
    pngCheckBox.panel = 4;
    pngCheckBox.attachedToMainPanel = true;
    pngCheckBox.type = "checkBox";
    UIElements["pngCheckBox"] = pngCheckBox;
    uiIndex.push_back("pngCheckBox");//31

    UIElement downloadButton;
    downloadButton.button.positionX = 0.0f;
    downloadButton.button.positionY = 0.2f;
    downloadButton.button.positionZ = 0.9f;
    downloadButton.button.width = 0.1f;
    downloadButton.button.height = 0.04f;
    downloadButton.button.color = colorData.buttonColor;
    downloadButton.button.colorHover = colorData.buttonColorHover;
    downloadButton.button.transitionMixVal = 0.0f;
    downloadButton.button.buttonCurveReduce = 10.0f;
    downloadButton.button.text = "Download";
    downloadButton.button.textRatio = 0.045f;
    downloadButton.panel = 4;
    downloadButton.attachedToMainPanel = true;
    downloadButton.type = "button";
    UIElements["downloadButton"] = downloadButton;
    uiIndex.push_back("downloadButton");//32



	//-------------ICONS-------------


    UIElement jpgIcon;
    jpgIcon.icon.positionX = -0.06f;
    jpgIcon.icon.positionY = 0.4f;
    jpgIcon.icon.positionZ = 0.9f;
    jpgIcon.icon.width = 0.05f;
    jpgIcon.icon.height = 0.065f;
    jpgIcon.icon.color = colorData.iconColor;
    jpgIcon.icon.colorHover = colorData.iconColorHover;
    jpgIcon.icon.mixVal = 0.0f;
    jpgIcon.icon.icon = icons.JpgFile;
    jpgIcon.panel = 4;
    jpgIcon.attachedToMainPanel = true;
    jpgIcon.type = "icon";
    UIElements["jpgIcon"] = jpgIcon;
    uiIndex.push_back("jpgIcon");//33

    UIElement pngIcon;
    pngIcon.icon.positionX = +0.1f;
    pngIcon.icon.positionY = 0.4f;
    pngIcon.icon.positionZ = 0.9f;
    pngIcon.icon.width = 0.05f;
    pngIcon.icon.height = 0.065f;
    pngIcon.icon.color = colorData.iconColor;
    pngIcon.icon.colorHover = colorData.iconColorHover;
    pngIcon.icon.mixVal = 0.0f;
    pngIcon.icon.icon = icons.PngFile;
    pngIcon.panel = 4;
    pngIcon.attachedToMainPanel = true;
    pngIcon.type = "icon";
    UIElements["pngIcon"] = pngIcon;
    uiIndex.push_back("pngIcon");//34

	UIElement folderIcon;
    folderIcon.icon.positionX = 0.1f;
    folderIcon.icon.positionY = 0.6f;
    folderIcon.icon.positionZ = 0.99f;
    folderIcon.icon.width = 0.02f;
    folderIcon.icon.height = 0.04f;
    folderIcon.icon.color = colorData.iconColor;
    folderIcon.icon.colorHover = colorData.iconColorHover;
    folderIcon.icon.mixVal = 0.0f;
    folderIcon.icon.icon = icons.Folder;
    folderIcon.panel = 1;
    folderIcon.attachedToMainPanel = true;
    folderIcon.type = "icon";
    UIElements["folderIcon"] = folderIcon;
    uiIndex.push_back("folderIcon");//35

	UIElement planeIcon;
    planeIcon.icon.positionX = 0.0f;
    planeIcon.icon.positionY = -0.1f;
    planeIcon.icon.positionZ = 0.99f;
    planeIcon.icon.width = 0.025f;
    planeIcon.icon.height = 0.035f;
    planeIcon.icon.color = colorData.iconColor;
    planeIcon.icon.colorHover = colorData.iconColorHover;
    planeIcon.icon.mixVal = 0.0f;
    planeIcon.icon.icon = icons.Panel;
    planeIcon.panel = 1;
    planeIcon.attachedToMainPanel = true;
	planeIcon.type = "icon";
    UIElements["planeIcon"] = planeIcon;
    uiIndex.push_back("planeIcon");//36

    UIElement sphereIcon;
    sphereIcon.icon.positionX = -0.002f;
    sphereIcon.icon.positionY = -0.2f;
    sphereIcon.icon.positionZ = 0.99f;
    sphereIcon.icon.width = 0.03f;
    sphereIcon.icon.height = 0.04f;
    sphereIcon.icon.color = colorData.iconColor;
    sphereIcon.icon.colorHover = colorData.iconColorHover;
    sphereIcon.icon.mixVal = 0.0f;
    sphereIcon.icon.icon = icons.Sphere;
    sphereIcon.panel = 1;
    sphereIcon.attachedToMainPanel = true;
	sphereIcon.type = "icon";
    UIElements["sphereIcon"] = sphereIcon;
    uiIndex.push_back("sphereIcon");//37

	UI ui;
	ui.uiElements = UIElements;
	ui.uiIndex = uiIndex; 

	return ui;
}