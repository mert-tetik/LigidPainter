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

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_navigation;

void panel_navigation_render(Timer& timer, Project& project ,bool doMouseTracking)
{
    panel_navigation.render(timer, doMouseTracking);
    if(panel_navigation.resizingDone){
        panels_transform(); 
    }

    // If shortcuts were used
    if(shortcuts_CTRL_S())
        project.updateProject(true, false);

    if(shortcuts_CTRL_SHIFT_S())
        project.saveAs("");

    //If pressed to "Project" button
    if(panel_navigation.sections[0].elements[1].button.clicked){ 
        
        // Show the context menu
        int res = show_context_menu(timer, {"Save", "Save as", "Create new", "Load new", "Copy path", "Open in file explorer", "Recover"});
        
        //Save
        if(res == 0){
            project.updateProject(true, false);
        }
        
        //Save as
        else if(res == 1){
        }
        
        //Create new
        else if(res == 2){
            dialog_newProject.dialogControl.activate();
        }
        
        //Load new
        else if(res == 3){
            dialog_loadProject.dialogControl.activate();
        }
        
        //Copy path
        else if(res == 4){
            project.copyTheProjectPathToTheClipboard();
        }

        //Open in file explorer
        else if(res == 5){
            UTIL::openInFileExplorer(project.absoluteProjectPath().c_str());
        }

        //Recover
        else if(res == 6){
            dialog_projectRecover.show(timer, project);
        }
    }
    
    //If pressed to "Help" button
    if(panel_navigation.sections[0].elements[2].button.clicked){ 
        int res = show_context_menu(timer, {"LigidTools website", "YouTube tutorials"});
        
        if(res == 0){
            Website("https://ligidtools.com/").open();
        }
        if(res == 1){
            Website("https://www.youtube.com/channel/UCMVLfsYsd5WAKEWsgM7fjtA").open();
        }
    }
}