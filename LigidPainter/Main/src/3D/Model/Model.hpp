/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef THREED_MODEL_HPP
#define THREED_MODEL_HPP

#include "UTIL/Util.hpp"
#include "3D/Mesh/Mesh.hpp"

class Model{
public:
    /*! @brief Meshes of the model */
    std::vector<Mesh>    meshes;

    /*! @brief Title of the model*/    
    std::string title;
    
    /*! @brief Path of the model*/    
    std::string path;

    /*! 
        @brief Is true in the same frame a new model is imported.
                And is set to false back in the renderer.render  
    */
    bool newModelAdded = false;

    Texture displayingTxtr; 
    
    /*! @brief 3D Model rendered with different color values for each object
            Created in the updateObjectIDsTexture function 
            Params : GL_NEAREST, GL_RED, GL_R32F
    */
    Texture objectIDs;
    
    void Draw();
    void exportModel(std::string path);
    bool loadModel(std::string const &path, bool triangulate, bool initTxtrs);
    void updateObjectIDsTexture();
    void generateDisplayingTexture();

    /*! 
        @brief Call this function as the mouse is clicked
               Detects the selected object (based on the cursor position)
               and applies it's index to the @ref selectedObjectIndices array
    */
    void selectObject();
};

#endif