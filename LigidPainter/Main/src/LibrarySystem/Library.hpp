/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LIBRARY_HPP
#define LIBRARY_HPP

//OpenGL & window 
#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map

/// @brief Forward declared Texture class 
class Texture;
/// @brief Forward declared Material class 
class Material;
/// @brief Forward declared Brush class 
class Brush;
/// @brief Forward declared Model class 
class Model;

/// @brief Responsible of holding textures, materials, 3D models, brushes etc. created/imported by the user
namespace Library{
    
    /// @brief Checks all the elements in the library and generates unique names for the doubled elements
    void uniqueNameControl();

    //----------- Add elements -----------
    /// @brief Pushes the element parameter back to the Textures array & generates unique ID for the texture
    void addTexture     (Texture texture);
    /// @brief Pushes the element parameter back to the Materials array & generates unique ID for the material
    void addMaterial    (Material material);
    /// @brief Pushes the element parameter back to the Brushes array 
    void addBrush       (Brush brush);
    /// @brief Pushes the element parameter back to the Models array 
    void addModel       (Model model);
    
    //----------- Get elements -----------
    /// @brief Returns the pointer of the element at the corresponding index from the Textures array 
    Texture* getTexture   (int index);
    /// @brief Returns the pointer of the element at the corresponding index from the Materials array
    Material* getMaterial (int index);
    /// @brief Returns the pointer of the element at the corresponding index from the Brushes array
    Brush* getBrush       (int index);
    /// @brief Returns the pointer of the element at the corresponding index from the Models array
    Model* getModel       (int index);

    //----------- Get element array sizes -----------
    /// @brief Returns the size of the Textures array
    int getTextureArraySize  ();
    /// @brief Returns the size of the Material array
    int getMaterialArraySize ();
    /// @brief Returns the size of the Brushes array
    int getBrushArraySize    ();
    /// @brief Returns the size of the Models array
    int getModelArraySize    ();

    //----------- Erase elements -------------
    /// @brief Erases the element at the corresponding index from the Textures array 
    void eraseTexture   (int index);
    /// @brief Erases the element at the corresponding index from the Materials array 
    void eraseMaterial  (int index);
    /// @brief Erases the element at the corresponding index from the Brushes array 
    void eraseBrush     (int index);
    /// @brief Erases the element at the corresponding index from the Models array 
    void eraseModel     (int index);
    
    //----------- Clear vectors -------------
    /// @brief Clears the Textures array 
    void clearTextures   ();
    /// @brief Clears the Materials array 
    void clearMaterials  ();
    /// @brief Clears the Brushes array 
    void clearBrushes    ();
    /// @brief Clears the TDModels array 
    void clearModels     ();

    //----------- Vector Pointers -------------
    std::vector<Texture>* getTextureVectorPointer();

    //----------- Selected Library Element -------------
    /*
        0 : Textures
        1 : Materials
        2 : Brushes
        3 : 3D Models
        4 : Fonts
        5 : Scripts
        6 : Filters
        7 : Layers
    */
    /// @brief Changes the selected element index variable to the newI param
    void changeSelectedElementIndex(int newI);
    /// @brief Returns the selected element index variable
    int getSelectedElementIndex();
    
    //----------- Library Changed -------------
    /// @brief Changes the library changed variable to the state param
    void setChanged(bool state);
    /// @brief Returns the library changed variable's value
    bool isChanged();

    /// @brief Generates a unique id for the corresponding element in the textures array (used in the @ref addTexture function)
    void textureGiveUniqueId(int index);
    /// @brief Generates a unique id for the corresponding element in the textures array (used in the @ref addMaterials function)
    void materialGiveUniqueId(int index);
};

#endif