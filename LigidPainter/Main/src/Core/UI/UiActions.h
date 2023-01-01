#ifndef LGDUIACTS_H
#define LGDUIACTS_H
struct UiActionsData{
    bool textureDemonstratorButtonPressed;
    bool textureDemonstratorBoundariesPressed;
};
class UiActions {
public:
    bool updateRangeValues(GLFWwindow* window,double xOffset,double yOffset, int screenWidth, int screenHeight);
    void isFirstClickDoneInside(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover);
    UiActionsData uiActions(GLFWwindow* window ,CallbckData callbackData,bool textureDemonstratorBoundariesHover,std::vector<UIElement> &UIElements);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
};
#endif