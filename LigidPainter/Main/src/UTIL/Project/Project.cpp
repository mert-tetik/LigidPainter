/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"

#include "3D/ThreeD.hpp"

#include "UTIL/Util.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Project/Project.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

static std::string path = "";
std::mutex project_mutex;
bool project_discard_update_flag = false;

std::string project_path(){
    if(path == "")
        return "";
        
    std::string res = std::filesystem::absolute(path).string();
    UTIL::correctFolderDistinguishers(res);
    return res;
}
std::string project_title(){
    return UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher());
}

void projectUTIL_set_path(std::string dest_path){
    //Make sure folder path doesn't have seperator at the end (./myProject/ -> ./myProject)
    if(dest_path[dest_path.size()-1] == '/' || dest_path[dest_path.size()-1] == '\\') 
        dest_path.pop_back();
    
    path = dest_path;
}