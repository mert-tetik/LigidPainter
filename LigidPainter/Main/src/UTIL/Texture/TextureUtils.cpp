/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/GL/GL.hpp"
#include "UTIL/Framebuffers/Framebuffer_Pool.hpp"
void Texture::copyDataToTheCopyContext(){
    glm::ivec2 res = this->getResolution();
    char* pxs = new char[res.x * res.y * 4]; 
    this->getData(pxs);

    if(getCopyContext()->window.makeContextCurrent()){
        mainThreadUsingCopyContext = true;

        glActiveTexture(GL_TEXTURE0);
        if(this->copyContextID == 0)
            glGenTextures(1,&this->copyContextID);

        //!LEAK
        Texture copyContextIDOBJ = Texture(copyContextID);
        copyContextIDOBJ.update(pxs, res.x, res.y);
        
        mainThreadUsingCopyContext = false;
        delete[] pxs;
    }
    
    getContext()->window.makeContextCurrent();
    
}