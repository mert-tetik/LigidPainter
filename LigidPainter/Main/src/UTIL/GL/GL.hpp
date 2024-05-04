/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef UTIL_GL_HPP
#define UTIL_GL_HPP

namespace GL{

    /* --------------------- TEXTURES -------------------- */
    /*! 
        @param slot 0 for GL_TEXTURE0, 10 for GL_TEXTURE10 etc.
        @param target Specifies the target to which the texture is bound. Must be one of GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, 
                      GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY, 
                      GL_TEXTURE_BUFFER, GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D_MULTISAMPLE_ARRAY.
    */
    int bindTexture(unsigned int texture, unsigned int target, std::string location);
    /*! 
        @param slot 0 for GL_TEXTURE0, 10 for GL_TEXTURE10 etc.
    */
    int bindTexture_2D(unsigned int texture, std::string location);
    /*! 
        @param slot 0 for GL_TEXTURE0, 10 for GL_TEXTURE10 etc.
    */
    int bindTexture_CubeMap(unsigned int texture, std::string location);

    /*!
        @brief Unbinds the bound textures in the current context
    */
    void releaseBoundTextures(std::string location);
    
    void releaseTextureFromSlot(int slot, std::string location);

    /* --------------------- VERTEX -------------------- */
    bool makeDrawCall(GLenum mode, GLint first, GLsizei count, std::string debugTitle);
    
}

#endif