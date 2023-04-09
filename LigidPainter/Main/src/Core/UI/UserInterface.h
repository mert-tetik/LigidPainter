#ifndef LGDUI_H
#define LGDUI_H

#include <map>
#include <vector>

#include "Core/LigidPainter.h"
#include "Core/Model/model.h"

#define UIUploadingModelPathTextBox 0
#define UIuploadingModelPathText 1
#define UIloadModelButton 2
#define UIautoTriangulateCheckBox 3
#define UIbackfaceCullingCheckBox 4
#define UImodelPanel3DModelsText 5
#define UIloadCustomModelButton 6
#define UIaddBrushMaskTextureButton 7
#define UIuseNegativeCheckBox 8
#define UImaskTextureFileNameText 9
#define UIbrushSizeText 10
#define UIbrushSizeRangeBar 11
#define UIbrushBlurText 12
#define UIbrushBlurRangeBar 13
#define UIbrushRotationText 14
#define UIbrushRotationRangeBar 15
#define UIbrushOpacityText 16
#define UIbrushOpacityRangeBar 17
#define UIbrushSpacingText 18
#define UIbrushSpacingRangeBar 19
#define UIbrushBordersText 20
#define UIbrushBordersRangeBar 21
#define UImirrorXCheckBox 22
#define UImirrorYCheckBox 23
#define UImirrorZCheckBox 24
#define UIexportingPathTextBox 25 
#define UIexportingFolderNameTextBox 26
#define UIjpgCheckBox 27
#define UIpngCheckBox 28
#define UIdownloadButton 29
#define UIskyboxCheckBox 30
#define UIskyBoxExposureRangeBar 31
#define UIskyBoxRotationRangeBar 32
#define UIrealtimeMaterialRenderingCheckBox 33
#define UIstabilizeFpsCheckBox 34
#define UIskyBoxExposureTextElement 35
#define UIskyBoxRotationTextElement 36
#define UIimageCheckBoxElement 37
#define UIviewportImageTextBoxElement 38
#define UInormalmapCheckBoxElement 39
#define UInormalStrengthTextElement 40
#define UInormalStrengthRangeBarElement 41
#define UInoiseCheckBoxElement 42
#define UInoiseStrengthTextElement 43
#define UInoiseStrengthRangeBarElement 44
#define UIgenerateTextureButtonElement 45
#define UIbakeButtonElement 46
#define UIpaintThroughCheckBoxElement 47
#define UIpaintOverCheckBoxElement 48
#define UIdisplayPaintOverTextureCheckBoxElement 49
#define UIselectPaintOverTextureIconElement 50
#define UIselectPaintOverTextureNameTextElement 51
#define UIdynamicPaintingCheckBoxElement 52
#define UIdynamicPaintingIconElement 53
#define UImaskPaintingCheckBoxElement 54
#define UImaskPaintingIconElement 55
#define UIcolorPaintingCheckBoxElement 56
#define UIcolorPaintingIconElement 57
#define UInormalmapPaintingCheckBoxElement 58
#define UInormalPaintingIconElement 59
#define UIsubSelectedImagePowerRangeBarElement 60
#define UIsubSelectedImagePowerRangeBarTextElement 61
#define UIfocusModeCheckBox 62
#define UImirrorXRangeBarElement 63
#define UImirrorYRangeBarElement 64
#define UImirrorZRangeBarElement 65
#define UIalbedoChannelCheckBoxElement 66
#define UIroughnessChannelCheckBoxElement 67
#define UImetallicChannelCheckBoxElement 68
#define UInormalChannelCheckBoxElement 69
#define UIjpgIcon 70
#define UIpngIcon 71
#define UIfolderIcon 72
#define UIloadPlaneModelButton 73
#define UIloadSphereModelButton 74
#define UIplaneIcon 75
#define UIsphereIcon 76
#define UIaddBrushMaskTextureIcon 77
#define UIselectBrushMaskTextureIcon 78

struct ColorData //LigidPainter color palette
{
	glm::vec3 LigidPainterThemeColor = glm::vec3(0.043f,0.635f,0.823f);

	glm::vec4 viewportBackColor = glm::vec4(0.23f, 0.23f, 0.23f,1.0f);

	glm::vec4 panelColor = glm::vec4(0.15f, 0.15f, 0.15f,0.5f);
	glm::vec4 panelColorSnd = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 panelHoldColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);

	glm::vec4 buttonColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 buttonColorHover = glm::vec4(0.0f, 0.0f, 0.0f,0.5f);

	glm::vec4 menuBarColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 textBoxColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 textBoxColorClicked = glm::vec4(LigidPainterThemeColor,0.5f);

	glm::vec4 checkBoxColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);
	glm::vec4 checkBoxHoverColor = glm::vec4(0.15f, 0.15f, 0.15f,0.5f);
	glm::vec4 checkBoxCheckedColor = glm::vec4(1.0f, 1.0f, 1.0f,0.5f);

	glm::vec4 rangeBarBack = glm::vec4(0.65f, 0.65f, 0.65f,0.5f);
	glm::vec4 rangeBarFront = glm::vec4(0.95f, 0.95f, 0.95f,0.5f);
	glm::vec4 rangeBarSlide = glm::vec4(0.95f, 0.95f, 0.95f,0.5f);

	glm::vec4 textColor = glm::vec4(1.0f, 1.0f, 1.0f,1.0f);

	glm::vec4 colorBoxIndicatorColor = glm::vec4(0.05f, 0.05f, 0.05f,0.5f);

	glm::vec4 iconColor = glm::vec4(1.0f,1.0f,1.0f,1.0f);
	glm::vec4 iconColorHover = glm::vec4(0.5f,0.5f,0.5f,1.0f);
	
	glm::vec4 numericModifierArrowColor = glm::vec4(0.6f, 0.6f, 0.6f,0.5f);
	glm::vec4 numericModifierArrowHoverColor = glm::vec4(0.95f, 0.95f, 0.95f,0.5f);

	glm::vec4 textureDisplayerButtonColor = glm::vec4(LigidPainterThemeColor,1.f);

	glm::vec4 buttonMaskTxtrPanelColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec3 chosenBrushMaskTextureColor = glm::vec3(LigidPainterThemeColor);
	glm::vec4 brushMaskIconColor = glm::vec4(1.0f,1.0f,1.0f,1.0f);
	glm::vec4 maskPanelSliderColor = glm::vec4(0.0f,0.0f,0.0f,0.5f);
	glm::vec4 maskPanelSliderBackgroundColor = glm::vec4(1.0f,1.0f,1.0f,0.5f);
	
	glm::vec4 messageBoxPanelColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 messageBoxButtonColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);

	glm::vec4 nodePanelColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 nodePanelColorSnd = glm::vec4(0.0f, 0.0f, 0.0f,0.7f);

	glm::vec4 nodePanelContextMenuPanelColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 nodePanelContextMenuButtonColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 nodePanelContextMenuButtonColorHover = glm::vec4(0.33f, 0.33f, 0.33f,0.5f);

	glm::vec4 floatNodeInputColor = glm::vec4(0.55f, 0.55f, 0.55f,1.0f);
	glm::vec4 vec2NodeInputColor = glm::vec4(0.33f, 0.93f, 0.33f,1.0f);
	glm::vec4 vec3NodeInputColor = glm::vec4(0.93f, 0.93f, 0.33f,1.0f);

	glm::vec4 textureSelectionPanelColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);

	glm::vec4 materialButtonColor = glm::vec4(0.13f, 0.13f, 0.13f,0.5f);
	glm::vec4 materialButtonColorHover = glm::vec4(0.0f, 0.0f, 0.0f,0.5f);
	glm::vec4 materialButtonColorActive = glm::vec4(LigidPainterThemeColor,0.5);
	glm::vec4 materialButtonColorActiveHover = glm::vec4(0.043f,0.435f,0.623f,0.5);

	glm::vec4 materialIconColor = glm::vec4(1.0f,1.0f,1.0f,1.0f);
	glm::vec4 materialIconColorHover = glm::vec4(0.5f,0.5f,0.5f,1.0f);
	glm::vec4 materialIconColorActive = glm::vec4(LigidPainterThemeColor,1.0);
	glm::vec4 materialIconColorActiveHover = glm::vec4(0.043f,0.435f,0.623f,1.0);
	glm::vec4 materialIconIndexTextColor = glm::vec4(0.1f,0.1f,0.1f,1.0);

	glm::vec3 alertColor = glm::vec3(0.76,0.17,0.17);

	glm::vec4 rangeBarTextColor = glm::vec4(0.02f,0.02f,0.02f,0.8f);
	
};

struct ColorData2{
	glm::vec3 LigidPainterThemeColor = glm::vec3(0.043f,0.635f,0.823f);
	glm::vec4 textboxCursorColor = glm::vec4(1.f);
	glm::vec4 nodeOutlineColor = glm::vec4(LigidPainterThemeColor,1.f);
	glm::vec4 selectionBoxColor = glm::vec4(LigidPainterThemeColor,0.2f);
};

struct Audios{
	unsigned int MessageBox;
	unsigned int Login;
	unsigned int ButtonEnter;
	unsigned int Alert;
};

struct Objects{
	unsigned int VBO;
	unsigned int VAO;
	unsigned int sqrVBO;
	unsigned int sqrVAO;
};

struct Container{
	float width;
	float height;
	float positionX;
	float positionY;
	float positionZ;
	glm::vec4 color;
};

struct aTexture{
	unsigned int id;
	std::string name;
	std::vector<unsigned int> undoList;
	bool nameTextActive = false;
	bool isTexture = true;
	bool isTrashFolder = false;
	bool rightClicked = false;
	int folderIndex = 10000;
};

struct Button{
	float width;
	float height;
	float positionX;
	float positionY;
	float positionZ;
	std::string text;
	float textRatio;
	glm::vec4 color;
	glm::vec4 colorHover;
	float buttonCurveReduce;
	float transitionMixVal;
	bool hover = false;
	bool hoverAnimationActive; 
};

struct RangeBar{
	float positionX;
	float positionY;
	float value;
	bool hover = false;
	float widthDivider = 1;
	bool isConstant = false;
};

struct Icon{
	float width;
	float height;
	float positionX;
	float positionY;
	float positionZ;
	unsigned int icon;
	glm::vec4 color;
	glm::vec4 colorHover;
	float mixVal = 0;
	bool hover = false;
	bool clickable = false;
};

struct Text{
	std::string text;
	glm::vec4 color;
	float positionX;
	float positionY;
	float scale;
};

struct CheckBox{
	float positionX;
	float positionY;
	std::string text;
	bool mouseHover = false;
	bool checked = false;
};
struct TextBox{
	float width;
	float height;
	float position_x;
	float position_y;
	float position_z;
	bool hover = false;
	float transitionMixVal = 0.0f;
	bool clicked = false;
	std::string text;
};

struct Box{
	float width;
	float height;
	float position_x;
	float position_y;
	float position_z;
	float buttonCurveReduce;
	glm::vec4 color;
	glm::vec4 colorTransitionColor; 
	float mixVal;
};

struct UIElement{
	Container container;
	Button button;
	RangeBar rangeBar;
	Icon icon;
	Text text;
	CheckBox checkBox;
	TextBox textBox;
	Box box;
	const char* type;
	bool attachedToMainPanel;
	int panel; //0 = Independent from panels , 1 = 3D model panel , 2 = Materials panel , 3 = Painting Panel , 4 = Export Panel
	bool focusMode = false;
};


struct ColorPicker{
	float saturationValuePosX;
	float saturationValuePosY;
	float hueValue;

	bool saturationValueBoxHover;
	bool saturationValuePointerHover;
	bool hueValueBarHover;
	bool hueValuePointerHover;

	bool hexValTextBoxEnter;
	bool hexValTextBoxActive;
	std::string hexValTextBoxVal = "#408181";

	bool hexValTextBoxGotInput;
	bool saturationValueValChanged;

	bool saturationValuePointerChanging;
	bool huePointerChanging;

	bool updateHueVal = true;
	bool updatePickerVal = true;

	bool saturationValueBoxClicked;
	bool hueBarClicked;

	bool dropperEnter;
	bool dropperActive;

	glm::vec3 hueColorValue = glm::vec3(0, 238.f/255.f, 254.f/255.f);
	glm::vec3 pickerValue = glm::vec3(68.0f/255.0f, 127.0f/255.0f, 135.0f/255.0f);
};

struct TextureDisplayer{
	float buttonPosX;
	float buttonPosY;

	float width = 0.4f;
	float height = 0.8f;

	float ndWidth = 0.4f;
	float ndHeight = 0.8f;

	bool cornerHover;
	bool cornerPressed;
	
	bool positionChanged;

	bool buttonHover;
	bool buttonPressed;
	bool buttonClicked;

	bool changeState;

	int buttonPressedCounter = 0;
};

struct NodePanel{
	float heigth = 0.5f;

	bool panelHover;
	bool boundariesHover;
	bool boundariesPressed;

	float zoomVal = 0.4f;
	bool zoomValChanged = false;

	float panelPositionX = 0.0;
	float panelPositionY = 0.0;

	bool pointerCursor = false;
};

struct NodeConnection{
	float connectionPosX = 0;
	float connectionPosY = 0;
	int nodeConnectionIndex = 10000;
	int inputConnectionIndex = 10000;

	bool connectionHover;
	bool connectionPressed;
};

struct NodeInput{
	std::string text;
	std::string type;
	std::string element;
	std::string selectedTextureName = "none";
	std::vector<NodeConnection> connections; //Output
	std::vector<float> rampPos = {0.02,0.98};
	std::vector<glm::vec3> rampClr = {glm::vec3(0),glm::vec3(255.)};
	std::vector<float> rampPress = {0,0};

	glm::vec4 value = glm::vec4(0.001);
	
	//Range element
	std::vector<bool> rangeBarsPointerPressed;

	//Texture element
	bool addTextureButtonHover;
	bool removeTextureButtonHover;
	int selectedTextureIndex = 10000;
	unsigned int selectedTexture = 0;

	//Color element
	glm::vec3 color = glm::vec3(0);
	bool coloringPanelActivated;
	

	float posX;
	float posY;

	bool connectionHover = false;
	bool pressed;

	//Input
	float connectionPosX = 0;
	float connectionPosY = 0;
	int nodeConnectionIndex = 10000;
	int inputConnectionIndex = 10000;

	bool isConnectedToShaderInput = false;
	
	bool textureSelectingState = false;

	unsigned int result;

	bool removeTheResult = true;

	int selectedRampIndex = 0;

};

struct ListBox{
	std::vector<std::string> elements;
	int chosenIndex = 0;
	bool active = false;
};

struct Node{
	std::string title;

	glm::vec4 upBarColor;
	glm::vec4 backColor;
	std::vector<NodeInput> inputs;
	std::vector<NodeInput> outputs;
	std::vector<ListBox> listBoxes;
	
	float unZoomedPositionX = 0.f; 
	float unZoomedPositionY = 0.f; 

	float positionX = 0.f; 
	float positionY = 0.f;

	float width;
	float height;

	bool barHover;
	bool barPressed;

	bool panelHover;

	int rangeBarCount = 0;

	unsigned int program;

	bool isMainOut = false;

	int renderingIndex = 10000;

	bool useModel = false;

	bool marked = false;

	bool active = false;

	bool stateChanged = false;

	int dupI = 0;

	bool doInvert = true;

	bool hide = false;
	bool hideDone = false;
};

struct ContextMenu {
	float positionX;
	float positionY;
	float positionZ;

	std::string searchText = "search";
	bool searchTextboxActive = false;

	float width;
	float height;

	bool stateChanged;
	bool active = false;

	int scroll = 0;

	bool hover;

	int selectedButtonIndex = 1;
	std::vector<Button> buttons;
};

struct SndPanel{
	float position = -0.6f;
	
	bool panelHover = false;

	bool boundariesHover = false;
	bool boundariesPressed = false;

	bool plusSignHover = false;
	bool minusSignHover = false;
	bool downSignHover = false;
	bool folderSignHover = false;
	bool backSignHover = false;
	bool duplicateSignHover = false;

	float plusSignMixVal = false;
	float minusSignMixVal = false;
	float downSignMixVal = false;
	float folderSignMixVal = false;
	float backSignMixVal = false;
	float duplicateSignMixVal = false;


	bool texturePanelButtonHover;
	float texturePanelButtonMixval = 0.f;
	bool materialPanelButtonHover;
	float materialPanelButtonMixval = 0.f;

	float texturePanelSlideVal = 0.f;
	float materialPanelSlideVal = 0.f;

	int state; //0 = textures , 1 = materials

	int activeFolderIndex = 10000;
};

struct NodeScene{
	int arrayIndex = 0;

	std::string sceneName;
	std::vector<Node> nodes;

	std::vector<Node> renderingPipeline;

	int index;
	bool stateChanged = true;

	unsigned int renderedTexture = 0; //Material display
	unsigned int outTexture = 0; //Material texture result
};

struct TextureSelectionPanel{
	float posX;
	float posY;
	float posZ;

	bool active;

	int selectedIndex = 10000;
	std::string selectedTextureName = "";

	bool panelHover = false;

	bool textureClicked = false;
};

struct ColoringPanel{
	float panelPosX = 0.f;
	float panelPosY = 0.f;
	
	bool panelHover = false;

	bool enteredOnce = false;

	float saturationValueBoxPosX = 0.f;
	float saturationValueBoxPosY = 0.f;
	bool saturationValueBoxPointerHover = false;
	bool saturationValueBoxPointerPressed = false;

	float hueBarPosX = 0.f;
	bool hueBarPointerHover = false;
	bool hueBarPointerPressed = false;

	bool pickerValueChanged = false;
	
	bool active;

	std::string hexVal = "#408080";
	bool hexValTextboxActive = false;
	bool hexValTextboxHover = false;
	bool newHexValTextboxEntry = false;

	bool dropperHover = false;
	bool dropperActive = false;

	glm::vec3 result;

	bool colorBoxHover;
	bool hueBarHover;

	bool brushSizeRangeBarPressed = false;
	bool brushBlurRangeBarPressed = false;
};

struct TextureCreatingPanel{
	float panelPosX;
	float panelPosY;
	
	bool panelHover = false;

	bool active; 

	std::string textBoxVal = "texture";
	bool textBoxActive = false;
	bool textBoxHover = false;
	bool newTextboxEntry = false;

	bool colorBarHover = false;
	bool coloringPanelActive = false;

	bool createButtonHover = false;

	glm::vec3 color = glm::vec3(0);
};
struct RenderOutData{
	glm::vec3 mouseHoverPixel;
	bool maskPanelMaskHover;
	bool maskPanelMaskClicked;
	unsigned int currentBrushMaskTxtr;

	bool texturePanelButtonHover;
	bool texturePanelButtonClicked;
	unsigned int chosenMaterialIndex;
};

struct BrushTexture{
	std::vector<aTexture> maskTextures;
	std::vector<aTexture> colorTextures;
	std::vector<aTexture> normalTextures;
};

class UserInterface {
public:
	//Load
	std::vector<UIElement> getUiElements(Icons icons);
	ContextMenu createContextMenus(std::vector<Node> appNodes);



	//Visuality
	void panel(float panelLoc, Icons icons,PanelData &panelData);
	void sndPanel(int state,float panelLoc , Programs programs , Icons icons , std::vector<aTexture> &albedoTextures,GLFWwindow* window,double mouseXpos,double mouseYpos,float screenGapX,float maxScreenWidth,int& selectedAlbedoTextureIndex,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,bool& newModelAdded,float &txtrSlideVal,float &materialSlideVal,bool &firstClick,ColoringPanel &clringPanel,TextureCreatingPanel &txtrCreatingPanel,bool& anyTextureNameActive,std::string &textureText,int& folderIndex,NodePanel &nodePanel,std::vector<Node> appNodes,SndPanel &sndpnl,BrushTexture &brushMaskTextures,bool maskPanelEnter,float yOffset,std::vector<NodeScene> &nodeScenesHistory,int brushMaskTexturesState,int chosenTextureResIndex);
	void nodePanel(float mainPanelLoc,float sndPanel, float height,Programs programs,Icons icons,std::vector<NodeScene> nodeScenes,int selectedNodeScene);
	void nodePanelBarriers(float mainPanelLoc,float sndPanel, float height);

	void textureDisplayer(float width,float height, float position_x,float position_y,float z);
	void box(float width, float height, float position_x, float position_y, std::string text, glm::vec4 color, float textRatio, bool isTextBox, bool isMaskImageBox, float buttonCurveReduce,float z,glm::vec4 colorTransitionColor , float mixVal);
	void iconBox(float width, float height, float position_x, float position_y,float z, unsigned int icon,float mixVal,glm::vec4 color,glm::vec4 colorHover);
	void renderText(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z,bool active);
	void renderMenubar(GLFWwindow* window);
	void checkBox(float position_x, float position_y, std::string text, bool mouseHover, bool checked,unsigned int circleTxtr);
	void panelChangeButton(float position_x, float position_y);
	void rangeBar(float position_x, float position_y, float value,float divideWidth);
	void constRangeBar(float position_x, float position_y,float value,Icons icons);
	void colorBox(float position_x, float position_y, float valueX, float valueY,Icons icons);
	glm::vec3 hueBar(float position_x, float position_y,float value, unsigned int FBO,GLFWwindow* window,glm::mat4 projection,bool updateHueVal);
	void decorationSquare(float position_x, float position_y);
	void numericModifier(float position_x,float position_y,unsigned int leftArrow,unsigned int rightArrow,float z,int value,float mixValP,float mixValN);
	bool verticalRangeBar(float positionX,float positionY,float height,float &orgvalue,float value,GLFWwindow* window,float mouseX,float mouseY,float yOffset,bool &firstClick,int textureSize,float screenGapX);
	void container(float positionX,float positionY,float positionZ,float width, float height,glm::vec4 color, Programs &programs,unsigned int circleTexture,glm::vec4 transitionColor, float mixVal);
	void circle(float positionX,float positionY,float positionZ,float width, float height, unsigned int circleTexture,glm::vec4 color,glm::vec4 transitionColor, float mixVal);
	void drawLine(float posX,float posY,float posZ,float toPosX,float toPosY,float width,glm::vec4 color,bool isLine);
	void textureSelectionPanel(TextureSelectionPanel &textureSelectionPanel,std::vector<aTexture> &albedoTextures,Programs programs,GLFWwindow* window,double mouseXpos,double mouseYpos,float screenGapX,int maxScreenWidth,unsigned int circleTexture);
	void renderAlert(std::string message,int duration,unsigned int uiProgram,int state);
	void alert(std::string message,int duration);
	bool coloringPanel(ColoringPanel &coloringPanel,Programs programs,Icons icons,GLFWwindow* window,SaturationValShaderData saturationValShaderData,glm::mat4 orthoProjection,double mouseXpos,double mouseYpos,bool &firstClick,float xOffset,float yOffset,
	unsigned int FBOscreen,ColorPicker &colorPicker,float screenGapX,glm::vec3 screenHoverPixel,std::vector<UIElement>&UIElements,bool modifyBrush);
	void textureCreatingPanel(TextureCreatingPanel &txtrCreatingPanel,Icons icons,Programs programs,GLFWwindow* window,double mouseXpos,double mouseYpos,bool &firstClick,ColoringPanel &coloringPanel,float screenGapX,std::vector<aTexture> &albedoTextures,int& activeFolderIndex);
	void modelMaterialPanel(Model &model,Programs programs,RenderData renderData,float screenGapX,float materialsPanelSlideValue,double mouseXpos,double mouseYpos,RenderOutData& uiOut,int& currentMaterialIndex,bool &firstClick,bool& newModelAdded, int &selectedNodeScene,Icons icons,std::vector<NodeScene> nodeScenes,std::vector<aTexture> &albedoTextures,TextureSelectionPanel &textureSelectionPanel);
	void brushMaskTexturePanel(Programs programs,BrushTexture &maskTextures,float centerCoords, float screenGapX,float &maskPanelSliderValue,unsigned int &currentBrushMaskTexture,bool &firstClick,GLFWwindow* window,double mouseXpos,double mouseYpos,unsigned int FBOScreen,PanelData &panelData,int screenSizeX,int screenSizeY,RenderOutData& uiOut,std::vector<UIElement> &UIElements,float brushBlurVal, OutShaderData outShaderData, float posY,int state);
	bool listBox(float posX,float posY,float posZ,const char* title,float width, Icons icons,std::vector<const char*> list,bool active,GLFWwindow* window, float mouseXpos,float mouseYpos,bool &firstClick,int &chosenIndex,float screenGapX);
	std::vector<float> selectionBox(bool active,float sPX,float sPY,float dPX,float dPY,float depth);
	void showMessageBox(std::string message);
	
	bool node(Node &node,Programs programs,Icons icons,GLFWwindow* window,double mouseX,double mouseY,double xOffset,double yOffset,float maxScreenWidth,
	float maxScreenHeight,NodeScene &material,NodePanel &nodePanel,TextureSelectionPanel &textureSelectionPanel,int currentNodeIndex,std::vector<aTexture> albedoTextures,
	float screenGapX,bool &firstClick,ColoringPanel &coloringPanel,bool &duplicateNodeCall,std::vector<Node> &duplicatedNodes);


	
	//Calculations and More
	bool isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, double mouseXpos, double mouseYpos);
	bool isMouseOnButton(GLFWwindow* window, float width, float height, float position_x, float position_y, double mouseXpos, double mouseYpos, bool isPanelMoving);
	bool isMouseOnCoords(GLFWwindow*window,double mouseXpos, double mouseYpos,std::vector<float> buttonCoor,bool isPanelMoving);
	bool textInput(int key, int action,bool caps,std::string &text,int threshold,GLFWwindow* window,int activeChar);
	bool textInputHex(int key, int action, std::string &text, int &activeChar);
	bool isMouseOnNodePanel(GLFWwindow* window,float mainPanelLoc,float height,double mouseXpos,double mouseYpos,bool calculateBoundaries,float sndPanel);


	void sendProgramsToUserInterface(Programs appPrograms);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void sendMaxWindowSizeToCalculationsAndMore(int maxScreenWidth,int maxScreenHeight);
	void sendTextBoxActiveCharToUI(int textBoxActiveChar);
	void sendObjectsToUI(Objects aobjects,unsigned int acircleIcon);
};
class MainLoop{
public:
	void updateCameraPosChanging(glm::vec3 cameraPos,bool &cameraPosChanging);
	void detectClick(GLFWwindow* window,bool &mousePress,bool &firstClick);
	void updateRenderTheScene(GLFWwindow* window,int &renderTheSceneCounter,bool &renderTheScene);
	void setContextPanelsStates(GLFWwindow* window,ColoringPanel &coloringPanel, ContextMenu &addNodeContextMenu, TextureSelectionPanel &textureSelectionPanel);
	void releaseTextBoxes(GLFWwindow* window,std::vector<UIElement> &UIElements,std::string &exportFileName,int& textBoxActiveChar,ColoringPanel &coloringPanel,TextureCreatingPanel& txtrCreatingPanel,ColorPicker &colorPicker);
	void changeTextureDisplayersState(GLFWwindow* window,TextureDisplayer &textureDisplayer);
	void changeColorPickersValue(GLFWwindow* window,ColorPicker& colorPicker,ColoringPanel &coloringPanel,glm::vec3 mouseHoverPixel,bool &firstClick);
};
#endif 