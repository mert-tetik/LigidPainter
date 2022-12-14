#ifndef LGDUIACTS_H
#define LGDUIACTS_H
class UiActions {
public:
    bool updateRangeValues(GLFWwindow* window,double xOffset,double yOffset, int screenWidth, int screenHeight, TextureDisplayer &textureDisplayer,SndPanel &sndPanel);
    void isFirstClickDoneInside(GLFWwindow* window ,CallbckData callbackData,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,NodePanel &nodePanel,SndPanel &sndPanel);
    void uiActions(GLFWwindow* window ,CallbckData callbackData,std::vector<UIElement> &UIElements,ColorPicker &colorPicker,TextureDisplayer &textureDisplayer,NodePanel &nodePanel,SndPanel &sndPanel);
    void sendMaxWindowSize(int maxScreenWidth,int maxScreenHeight);
};
#endif