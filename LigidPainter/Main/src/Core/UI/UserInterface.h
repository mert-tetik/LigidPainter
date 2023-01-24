#ifndef LGDUI_H
#define LGDUI_H

#include <map>

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
#define UIjpgIcon 30
#define UIpngIcon 31
#define UIfolderIcon 32
#define UIloadPlaneModelButton 33
#define UIloadSphereModelButton 34
#define UIplaneIcon 35
#define UIsphereIcon 36

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

	glm::vec4 textureDisplayerButtonColor = glm::vec4(LigidPainterThemeColor,0.5f);

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
};

//Width will be added to the midPanelPos if attachedToMainPanel is true
struct Container{
	float width;
	float height;
	float positionX;
	float positionY;
	float positionZ;
	glm::vec4 color;
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
	bool mouseHover;
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

	float zoomVal = 1.0f;
	bool zoomValChanged = false;

	float panelPositionX = 0.0;
	float panelPositionY = 0.0;
};

struct NodeInput{
	std::string text;
	std::string type;
	
	std::string element;

	glm::vec4 value = glm::vec4(0.001);
	
	//Range element
	std::vector<bool> rangeBarsPointerPressed;

	//Texture element
	bool addTextureButtonHover;
	bool removeTextureButtonHover;
	int selectedTextureIndex = 10000;
	unsigned int selectedTexture = 0;
	

	float connectionPosX = 0;
	float connectionPosY = 0;

	float posX;
	float posY;

	bool connectionHover = false;
	bool pressed;

	int nodeConnectionIndex = 10000;
	int inputConnectionIndex = 10000;


	bool isConnectedToShaderInput = false;
	
	bool textureSelectingState = false;

	unsigned int result;
};



struct Node{
	std::string title;

	glm::vec4 upBarColor;
	glm::vec4 backColor;
	std::vector<NodeInput> inputs;
	std::vector<NodeInput> outputs;
	
	float unZoomedPositionX; 
	float unZoomedPositionY; 
	float positionX; 
	float positionY;

	float width;
	float height;

	bool barHover;
	bool barPressed;

	int rangeBarCount = 0;

	unsigned int program;

	bool isMainOut = false;

	int renderingIndex = 10000;
};

struct ContextMenu {
	float positionX;
	float positionY;
	float positionZ;

	float width;
	float height;

	bool stateChanged;
	bool active = false;

	bool hover;

	std::vector<Button> buttons;
};

struct SndPanel{
	float position = -0.6f;
	bool boundariesHover = false;
	bool boundariesPressed = false;

	bool plusSignHover = false;
	bool minusSignHover = false;
	bool downSignHover = false;

	bool texturePanelButtonHover;
	bool materialPanelButtonHover;

	int state; //0 = textures , 1 = materials
};

struct NodeScene{
	std::string sceneName;
	std::vector<Node> nodes;

	int index;
	bool stateChanged = true;
};

struct TextureSelectionPanel{
	float posX;
	float posY;
	float posZ;

	bool active;

	int selectedIndex = 10000;

	bool panelHover = false;

	bool textureClicked = false;
};

class UserInterface {
public:
	//Load
	std::vector<UIElement> UserInterface::getUiElements(Icons icons);
	ContextMenu createContextMenus(std::vector<Node> appNodes);



	//Visuality
	void panel(float panelLoc, Icons icons);
	void sndPanel(int state,float panelLoc , Programs programs , Icons icons , std::vector<unsigned int> &albedoTextures,GLFWwindow* window,double mouseXpos,double mouseYpos,float screenGapX,float maxScreenWidth,int& selectedAlbedoTextureIndex,std::vector<NodeScene>& nodeScenes,int &selectedNodeScene,bool& newModelAdded);
	void nodePanel(float mainPanelLoc,float sndPanel, float height,Programs programs,Icons icons,std::vector<NodeScene> nodeScenes,int selectedNodeScene);
	void nodePanelBarriers(float mainPanelLoc,float sndPanel, float height);

	void textureDisplayer(float width,float height, float position_x,float position_y,float z);
	void box(float width, float height, float position_x, float position_y, std::string text, glm::vec4 color, float textRatio, bool isTextBox, bool isMaskImageBox, float buttonCurveReduce,float z,glm::vec4 colorTransitionColor , float mixVal);
	void iconBox(float width, float height, float position_x, float position_y,float z, unsigned int icon,float mixVal,glm::vec4 color,glm::vec4 colorHover);
	void setViewportBgColor();
	void renderText(unsigned int program, std::string text, float x, float y, float scale,glm::vec4 color,float z);
	void renderMenubar(GLFWwindow* window);
	void checkBox(float position_x, float position_y, std::string text, bool mouseHover, bool checked,unsigned int circleTxtr);
	void panelChangeButton(float position_x, float position_y);
	void rangeBar(float position_x, float position_y, float value);
	void colorBox(float position_x, float position_y, float valueX, float valueY);
	glm::vec3 hueBar(float position_x, float position_y,float value, unsigned int FBO,GLFWwindow* window,glm::mat4 projection,bool updateHueVal);
	void decorationSquare(float position_x, float position_y);
	void numericModifier(float position_x,float position_y,unsigned int leftArrow,unsigned int rightArrow,float z,int value,float mixValP,float mixValN);
	void verticalRangeBar(float positionX,float positionY,float height,float value);
	void container(float positionX,float positionY,float positionZ,float width, float height,glm::vec4 color, Programs &programs,unsigned int circleTexture);
	void circle(float positionX,float positionY,float positionZ,float width, float height, unsigned int circleTexture,glm::vec4 color);
	void drawLine(float posX,float posY,float posZ,float toPosX,float toPosY,float width,glm::vec4 color);
	void textureSelectionPanel(TextureSelectionPanel &textureSelectionPanel,std::vector<unsigned int> &albedoTextures,Programs programs,GLFWwindow* window,double mouseXpos,double mouseYpos,float screenGapX,int maxScreenWidth,unsigned int circleTexture);

	
	void node(Node &node,Programs programs,Icons icons,GLFWwindow* window,double mouseX,double mouseY,double xOffset,double yOffset,float maxScreenWidth,
	float maxScreenHeight,NodeScene &material,NodePanel &nodePanel,TextureSelectionPanel &textureSelectionPanel,int currentNodeIndex,std::vector<unsigned int> albedoTextures);


	
	//Calculations and More
	bool isMouseOnPanelChangeButton(GLFWwindow* window, float position_x, float position_y, double mouseXpos, double mouseYpos);
	bool isMouseOnButton(GLFWwindow* window, float width, float height, float position_x, float position_y, double mouseXpos, double mouseYpos, bool isPanelMoving);
	bool isMouseOnCoords(GLFWwindow*window,double mouseXpos, double mouseYpos,std::vector<float> buttonCoor,bool isPanelMoving);
	bool textInput(int key, int action,bool caps,std::string &text,int threshold);
	bool textInputHex(int key, int action, std::string &text, int &activeChar);
	bool isMouseOnNodePanel(GLFWwindow* window,float mainPanelLoc,float height,double mouseXpos,double mouseYpos,bool calculateBoundaries);


	void sendProgramsToUserInterface(Programs appPrograms);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
	void sendMaxWindowSizeToCalculationsAndMore(int maxScreenWidth,int maxScreenHeight);
};
#endif 