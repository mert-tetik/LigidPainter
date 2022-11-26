struct UiActionsData{
    bool textureDemonstratorButtonPressed;
    bool textureDemonstratorBoundariesPressed;
};
class UiActions {
public:
    bool updateRangeValues(GLFWwindow* window,float xOffset,float yOffset,unsigned int screenWidth,unsigned int screenHeight);
    void isFirstClickDoneInside(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover);
    UiActionsData uiActions(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover);
};