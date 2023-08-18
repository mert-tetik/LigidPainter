
#include "UTIL/Util.hpp"

namespace Settings{
    struct AppTextures{ 
        //--Icons
        Texture TDModelIcon; 
        Texture softenIcon;  
        Texture smearIcon;
        Texture modifierIcon;
        Texture environmentIcon;
        Texture dropperIcon;
        Texture colorIcon;
        Texture brushIcon;
        Texture arrowR;
        Texture arrowL;
        Texture threeDIcon;
        Texture twoDIcon;
        Texture exportIcon;
        Texture areaPaintingIcon;
        Texture gearIcon;
        Texture ligidPainterIcon;
        Texture nodeDisplayingMode;
        Texture paintingDisplayingMode;
        Texture solidPaintingDisplayingMode;
        Texture logButtonL;
        Texture logButtonR;
        Texture logButtonM;
        
        //--Textures
        Texture greetingDialogImage;  
        Texture noMaterialConnectedToTheMeshWarningImage;  
    };

    struct Properties{
        /*! @brief Set swapInterval(1) if true */
        bool VSync = false; 

        bool backfaceCulling = false;

        /*! @brief Textures will be generated with that resolution value */
        int textureRes = 1024; 
    };

    void loadAppTextures();

    AppTextures appTextures();

    Properties* properties();
};
