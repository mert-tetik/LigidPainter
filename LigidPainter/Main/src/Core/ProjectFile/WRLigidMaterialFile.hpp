#ifndef LIGID_WRITE_READ_MATERIAL_FILE 
#define LIGID_WRITE_READ_MATERIAL_FILE

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include "../../thirdparty/include/glad/glad.h"

#include "Core/Model/model.h"
#include "Core/UI/UserInterface.h"
#include "Core/Texture/Texture.h"
#include "Core/gl.h"

//* V1.3 : 0x94BA0BBA; 

class MaterialFile{
public:
    void writeTheFile(const char* path,NodeScene material){
        std::ofstream wf = std::ofstream(path, std::ios::out | std::ios::binary);
        if(wf.fail())
            std::cout << "Error : Writing the material file : Can't create the file : " << path << std::endl;


            uint64_t h1 = 0x94BA0BBA; 
            wf.write(reinterpret_cast<char*>(&h1),sizeof(uint64_t));

            uint64_t nodeScI = material.index;
            uint64_t nodeScAI = material.arrayIndex;
            wf.write(reinterpret_cast<char*>(&nodeScI),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(&nodeScAI),sizeof(uint64_t));

            //Scene name
            uint64_t sceneNameSize = material.sceneName.size();
            wf.write(reinterpret_cast<char*>(&sceneNameSize),sizeof(uint64_t));
            for (size_t i = 0; i < material.sceneName.size(); i++)
            {
                wf.write(reinterpret_cast<char*>(&material.sceneName[i]),sizeof(char));
            }
            
            //Scene Nodes
            uint64_t nodeSize = material.nodes.size();
            wf.write(reinterpret_cast<char*>(&nodeSize),sizeof(uint64_t));
            for (size_t nodeI = 0; nodeI < material.nodes.size(); nodeI++)
            {
                //Node's itself
                sNode snode;
                snode.upBarColor = material.nodes[nodeI].upBarColor;
    	        snode.backColor = material.nodes[nodeI].backColor;
    	        snode.unZoomedPositionX = material.nodes[nodeI].unZoomedPositionX; 
    	        snode.unZoomedPositionY = material.nodes[nodeI].unZoomedPositionY; 
    	        snode.positionX = material.nodes[nodeI].positionX; 
    	        snode.positionY = material.nodes[nodeI].positionY;
    	        snode.width = material.nodes[nodeI].width;
    	        snode.height = material.nodes[nodeI].height;
    	        snode.barHover = material.nodes[nodeI].barHover;
    	        snode.barPressed = material.nodes[nodeI].barPressed;
    	        snode.panelHover = material.nodes[nodeI].panelHover;
    	        snode.rangeBarCount = material.nodes[nodeI].rangeBarCount;
    	        snode.program = material.nodes[nodeI].program;
    	        snode.isMainOut = material.nodes[nodeI].isMainOut;
    	        snode.renderingIndex = material.nodes[nodeI].renderingIndex;
    	        snode.useModel = material.nodes[nodeI].useModel;
    	        snode.marked = material.nodes[nodeI].marked;
    	        snode.active = material.nodes[nodeI].active;
    	        snode.stateChanged = material.nodes[nodeI].stateChanged;
    	        snode.dupI = material.nodes[nodeI].dupI;
    	        snode.doInvert = material.nodes[nodeI].doInvert;
                wf.write(reinterpret_cast<char*>(&snode),sizeof(sNode));

                uint64_t nodeFragSourceSize = material.nodes[nodeI].fragSource.size();
                wf.write(reinterpret_cast<char*>(&nodeFragSourceSize),sizeof(uint64_t));
                for (size_t frI = 0; frI < nodeFragSourceSize; frI++)
                {
                    wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].fragSource[frI]),sizeof(char));
                }

                //Inputs
                uint64_t nodeInputSize = material.nodes[nodeI].inputs.size();
                wf.write(reinterpret_cast<char*>(&nodeInputSize),sizeof(uint64_t));
                for (size_t inI = 0; inI < material.nodes[nodeI].inputs.size(); inI++)
                {
                    //Input's itself
                    sNodeInput sIn;
	                sIn.addTextureButtonHover = material.nodes[nodeI].inputs[inI].addTextureButtonHover;
	                sIn.removeTextureButtonHover = material.nodes[nodeI].inputs[inI].removeTextureButtonHover;
	                sIn.selectedTextureIndex = material.nodes[nodeI].inputs[inI].selectedTextureIndex;
	                sIn.selectedTexture = material.nodes[nodeI].inputs[inI].selectedTexture;
	                sIn.color = material.nodes[nodeI].inputs[inI].color;
	                sIn.coloringPanelActivated = material.nodes[nodeI].inputs[inI].coloringPanelActivated;
	                sIn.posX = material.nodes[nodeI].inputs[inI].posX;
	                sIn.posY = material.nodes[nodeI].inputs[inI].posY;
	                sIn.connectionHover = material.nodes[nodeI].inputs[inI].connectionHover;
	                sIn.pressed = material.nodes[nodeI].inputs[inI].pressed;
	                sIn.connectionPosX = material.nodes[nodeI].inputs[inI].connectionPosX;
	                sIn.connectionPosY = material.nodes[nodeI].inputs[inI].connectionPosY;
	                sIn.nodeConnectionIndex = material.nodes[nodeI].inputs[inI].nodeConnectionIndex;
	                sIn.inputConnectionIndex = material.nodes[nodeI].inputs[inI].inputConnectionIndex;
	                sIn.isConnectedToShaderInput = material.nodes[nodeI].inputs[inI].isConnectedToShaderInput;
	                sIn.textureSelectingState = material.nodes[nodeI].inputs[inI].textureSelectingState;
	                sIn.result = material.nodes[nodeI].inputs[inI].result;
	                sIn.removeTheResult = material.nodes[nodeI].inputs[inI].removeTheResult;
	                sIn.selectedRampIndex = material.nodes[nodeI].inputs[inI].selectedRampIndex;
	                sIn.value = material.nodes[nodeI].inputs[inI].value;

                    wf.write(reinterpret_cast<char*>(&sIn),sizeof(sNodeInput));

                    //inputs text
                    uint64_t nodeOutputTextSize = material.nodes[nodeI].inputs[inI].text.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].text.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].text[i]),sizeof(char));
                    }
                    //inputs type
                    uint64_t nodeOutputTypeSize = material.nodes[nodeI].inputs[inI].type.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].type.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].type[i]),sizeof(char));
                    }
                    //inputs element
                    uint64_t nodeOutputElementSize = material.nodes[nodeI].inputs[inI].element.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].element.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].element[i]),sizeof(char));
                    }
                    //inputs texture
                    uint64_t nodeOutputTextureSize = material.nodes[nodeI].inputs[inI].selectedTextureName.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].selectedTextureName.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].selectedTextureName[i]),sizeof(char));
                    }
                    //inputs Connections
                    uint64_t nodeOutputConnectionSize = material.nodes[nodeI].inputs[inI].connections.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].connections.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].connections[i]),sizeof(NodeConnection));
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize = material.nodes[nodeI].inputs[inI].rampPos.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].rampPos.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].rampPos[i]),sizeof(float));
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize = material.nodes[nodeI].inputs[inI].rampClr.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].rampClr.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].rampClr[i]),sizeof(glm::vec3));
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize = material.nodes[nodeI].inputs[inI].rampPress.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].inputs[inI].rampPress.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].inputs[inI].rampPress[i]),sizeof(bool));
                    }
 
                    //rangeBarsPointerPressedSize
                    uint64_t rangeBarsPointerPressedSize = material.nodes[nodeI].inputs[inI].rangeBarsPointerPressed.size();
                    wf.write(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                }

                //Outputs
                uint64_t nodeOutputSize = material.nodes[nodeI].outputs.size();
                wf.write(reinterpret_cast<char*>(&nodeOutputSize),sizeof(uint64_t));
                for (size_t outI = 0; outI < material.nodes[nodeI].outputs.size(); outI++)
                {
                    //Output's itself
                    sNodeInput sOut;
	                sOut.addTextureButtonHover = material.nodes[nodeI].outputs[outI].addTextureButtonHover;
	                sOut.removeTextureButtonHover = material.nodes[nodeI].outputs[outI].removeTextureButtonHover;
	                sOut.selectedTextureIndex = material.nodes[nodeI].outputs[outI].selectedTextureIndex;
	                sOut.selectedTexture = material.nodes[nodeI].outputs[outI].selectedTexture;
	                sOut.color = material.nodes[nodeI].outputs[outI].color;
	                sOut.coloringPanelActivated = material.nodes[nodeI].outputs[outI].coloringPanelActivated;
	                sOut.posX = material.nodes[nodeI].outputs[outI].posX;
	                sOut.posY = material.nodes[nodeI].outputs[outI].posY;
	                sOut.connectionHover = material.nodes[nodeI].outputs[outI].connectionHover;
	                sOut.pressed = material.nodes[nodeI].outputs[outI].pressed;
	                sOut.connectionPosX = material.nodes[nodeI].outputs[outI].connectionPosX;
	                sOut.connectionPosY = material.nodes[nodeI].outputs[outI].connectionPosY;
	                sOut.nodeConnectionIndex = material.nodes[nodeI].outputs[outI].nodeConnectionIndex;
	                sOut.inputConnectionIndex = material.nodes[nodeI].outputs[outI].inputConnectionIndex;
	                sOut.isConnectedToShaderInput = material.nodes[nodeI].outputs[outI].isConnectedToShaderInput;
	                sOut.textureSelectingState = material.nodes[nodeI].outputs[outI].textureSelectingState;
	                sOut.result = material.nodes[nodeI].outputs[outI].result;
	                sOut.removeTheResult = material.nodes[nodeI].outputs[outI].removeTheResult;
	                sOut.selectedRampIndex = material.nodes[nodeI].outputs[outI].selectedRampIndex;
	                sOut.value = material.nodes[nodeI].outputs[outI].value;

                    wf.write(reinterpret_cast<char*>(&sOut),sizeof(sNodeInput));
                    
                    //Output text
                    uint64_t nodeOutputTextSize = material.nodes[nodeI].outputs[outI].text.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].text.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].text[i]),sizeof(char));
                    }
                    //Output type
                    uint64_t nodeOutputTypeSize = material.nodes[nodeI].outputs[outI].type.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].type.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].type[i]),sizeof(char));
                    }
                    //Output element
                    uint64_t nodeOutputElementSize = material.nodes[nodeI].outputs[outI].element.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].element.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].element[i]),sizeof(char));
                    }
                    //Output texture
                    uint64_t nodeOutputTextureSize = material.nodes[nodeI].outputs[outI].selectedTextureName.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].selectedTextureName.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].selectedTextureName[i]),sizeof(char));
                    }
                    //Output Connections
                    uint64_t nodeOutputConnectionSize = material.nodes[nodeI].outputs[outI].connections.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].connections.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].connections[i]),sizeof(NodeConnection));
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize = material.nodes[nodeI].outputs[outI].rampPos.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].rampPos.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].rampPos[i]),sizeof(float));
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize = material.nodes[nodeI].outputs[outI].rampClr.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].rampClr.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].rampClr[i]),sizeof(glm::vec3));
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize = material.nodes[nodeI].outputs[outI].rampPress.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < material.nodes[nodeI].outputs[outI].rampPress.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].outputs[outI].rampPress[i]),sizeof(bool));
                    }
                    
                    //rangeBarsPointerPressedSize
                    uint64_t rangeBarsPointerPressedSize = material.nodes[nodeI].outputs[outI].rangeBarsPointerPressed.size();
                    wf.write(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                }

                //Listboxes
                uint64_t nodeListboxSize = material.nodes[nodeI].listBoxes.size();
                wf.write(reinterpret_cast<char*>(&nodeListboxSize),sizeof(uint64_t));
                for (size_t listI = 0; listI < material.nodes[nodeI].listBoxes.size(); listI++)
                {
                    wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].listBoxes[listI].active),sizeof(bool));
                    wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].listBoxes[listI].chosenIndex),sizeof(int));
                    
                    //Listbox elements
                    uint64_t nodeListboxElementSize = material.nodes[nodeI].listBoxes[listI].elements.size();
                    wf.write(reinterpret_cast<char*>(&nodeListboxElementSize),sizeof(uint64_t));
                    for (size_t elementI = 0; elementI < material.nodes[nodeI].listBoxes[listI].elements.size(); elementI++)
                    {
                        //List box element titles
                        uint64_t nodeListboxElementTextSize = material.nodes[nodeI].listBoxes[listI].elements[elementI].size();
                        wf.write(reinterpret_cast<char*>(&nodeListboxElementTextSize),sizeof(uint64_t));
                        for (size_t i = 0; i < material.nodes[nodeI].listBoxes[listI].elements[elementI].size(); i++)
                        {
                            wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].listBoxes[listI].elements[elementI][i]),sizeof(char));
                        }
                    }
                }

                //Node title size
                uint64_t nodeTitleSize = material.nodes[nodeI].title.size();
                wf.write(reinterpret_cast<char*>(&nodeTitleSize),sizeof(uint64_t));
                //Title
                for (size_t i = 0; i < material.nodes[nodeI].title.size(); i++)
                {
                    wf.write(reinterpret_cast<char*>(&material.nodes[nodeI].title[i]),sizeof(char));
                }
            }
    }
    NodeScene readTheFile(const char* path){
        ifstream rf;
        startReadingTheProjectFile(path,rf);

        NodeScene material;

            uint64_t h1 = 0x94BA0BBA; 
            uint64_t ch1; 
            
            rf.read(reinterpret_cast<char*>(&ch1),sizeof(uint64_t));
            if(h1 != ch1){
                UserInterface ui;
                ui.alert("This is not a material file. File description header is 0x94BA0BBA",200,false);
                return material;
            }
            
            rf.read(reinterpret_cast<char*>(&material.index),sizeof(uint64_t));
            rf.read(reinterpret_cast<char*>(&material.arrayIndex),sizeof(uint64_t));
        
            
            //Scene name
            uint64_t sceneNameSize;
            rf.read(reinterpret_cast<char*>(&sceneNameSize),sizeof(uint64_t));
            for (size_t i = 0; i < sceneNameSize; i++)
            {
                char ch;
                rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                material.sceneName.push_back(ch);
            }
            
            //Scene Nodes
            uint64_t nodeSize;
            rf.read(reinterpret_cast<char*>(&nodeSize),sizeof(uint64_t));
            for (size_t nodeI = 0; nodeI < nodeSize; nodeI++)
            {


                Node node;
                material.nodes.push_back(node);
                

                //Node's itself
                sNode snode;
                rf.read(reinterpret_cast<char*>(&snode),sizeof(sNode));
                material.nodes[nodeI].upBarColor = snode.upBarColor;
    	        material.nodes[nodeI].backColor = snode.backColor;
    	        material.nodes[nodeI].unZoomedPositionX = snode.unZoomedPositionX; 
    	        material.nodes[nodeI].unZoomedPositionY = snode.unZoomedPositionY; 
    	        material.nodes[nodeI].positionX = snode.positionX; 
    	        material.nodes[nodeI].positionY = snode.positionY;
    	        material.nodes[nodeI].width = snode.width;
    	        material.nodes[nodeI].height = snode.height;
    	        material.nodes[nodeI].barHover = snode.barHover;
    	        material.nodes[nodeI].barPressed = snode.barPressed;
    	        material.nodes[nodeI].panelHover = snode.panelHover;
    	        material.nodes[nodeI].rangeBarCount = snode.rangeBarCount;
    	        material.nodes[nodeI].program = snode.program;
    	        material.nodes[nodeI].isMainOut = snode.isMainOut;
    	        material.nodes[nodeI].renderingIndex = snode.renderingIndex;
    	        material.nodes[nodeI].useModel = snode.useModel;
    	        material.nodes[nodeI].marked = snode.marked;
    	        material.nodes[nodeI].active = snode.active;
    	        material.nodes[nodeI].stateChanged = snode.stateChanged;
    	        material.nodes[nodeI].dupI = snode.dupI;
    	        material.nodes[nodeI].doInvert = snode.doInvert;
                
                uint64_t nodeFragSourceSize;
                rf.read(reinterpret_cast<char*>(&nodeFragSourceSize),sizeof(uint64_t));
                for (size_t frI = 0; frI < nodeFragSourceSize; frI++)
                {
                    char c;
                    rf.read(reinterpret_cast<char*>(&c),sizeof(char));
                    material.nodes[nodeI].fragSource.push_back(c);
                }
                    const char* defaultVertexShader = 
			            "#version 330 core\n"
			            "layout(location = 0) in vec3 aPos;\n"
			            "layout(location = 1) in vec3 aNormal;\n"
			            "layout(location = 2) in vec2 aTexCoords;\n"
			            "layout(location = 3) in vec3 aTangent;\n"
			            "layout(location = 4) in vec3 aBitangent;\n"

			            "uniform mat4 view;\n"
			            "uniform mat4 projection;\n"
			            "uniform int is3D;\n"

			            "out vec2 tex_coords;\n"
			            "out vec3 normal;\n"
			            "out vec3 posModel;\n"
			            "out vec4 posScene;\n"
			            "out vec3 tangent;\n"
			            "out vec3 bitangent;\n"

			            "void main() {\n"
			            	"tangent = aTangent;\n"
			            	"bitangent = aBitangent;\n"
			                "posModel = aPos;\n"
			                "tex_coords = aTexCoords;\n"
			                "normal = aNormal;\n"
			            	"vec4 res;\n"
			            	"if(is3D == 1){\n"
			                	"res = projection * view * vec4(aPos, 0.5);\n" 
			            	"}\n"
			            	"else{\n"
			                	"res = projection * vec4(tex_coords,0, 1);\n" 
			            	"}\n"
			            	"posScene = projection * view * vec4(aPos, 0.5);\n"
			                "gl_Position = res;\n"
			            "}\0";

		            //Compile the fragment shader
		            const char* shaderSource = material.nodes[nodeI].fragSource.c_str();
		            unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		            glShaderSource(fragShader, 1, &shaderSource, NULL);
		            glCompileShader(fragShader);
		            //Test the shader
		            int success;
		            char infoLog[512];
		            glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
		            //Print the error if any occurs
		            if (!success){glGetShaderInfoLog(fragShader, 512, NULL, infoLog);std::cout << "ERROR::SHADER::COMPILATION_FAILED " << infoLog << std::endl;};

		            //Compile the vertex shader
		            unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
		            glShaderSource(vertShader, 1, &defaultVertexShader, NULL);
		            glCompileShader(vertShader);

		            //Test the shader
		            glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
		            //Print the error if any occurs
		            if (!success){glGetShaderInfoLog(vertShader, 512, NULL, infoLog);std::cout << "ERROR::SHADER::COMPILATION_FAILED " << infoLog << std::endl;};

		            material.nodes[nodeI].program = glCreateProgram();
		            glAttachShader(material.nodes[nodeI].program, vertShader);
		            glAttachShader(material.nodes[nodeI].program, fragShader);
		            glLinkProgram(material.nodes[nodeI].program);

		            glDeleteShader(vertShader);
		            glDeleteShader(fragShader);

                //Inputs
                uint64_t nodeInputSize;
                rf.read(reinterpret_cast<char*>(&nodeInputSize),sizeof(uint64_t));
                for (size_t inI = 0; inI < nodeInputSize; inI++)
                {
                    NodeInput nodeIn;
                    material.nodes[nodeI].inputs.push_back(nodeIn);


                    //Input's itself
                    sNodeInput sIn;
                    rf.read(reinterpret_cast<char*>(&sIn),sizeof(sNodeInput));

	                material.nodes[nodeI].inputs[inI].addTextureButtonHover = sIn.addTextureButtonHover;
	                material.nodes[nodeI].inputs[inI].removeTextureButtonHover = sIn.removeTextureButtonHover;
	                material.nodes[nodeI].inputs[inI].selectedTextureIndex = sIn.selectedTextureIndex;
	                material.nodes[nodeI].inputs[inI].selectedTexture = sIn.selectedTexture;
	                material.nodes[nodeI].inputs[inI].color = sIn.color;
	                material.nodes[nodeI].inputs[inI].coloringPanelActivated = sIn.coloringPanelActivated;
	                material.nodes[nodeI].inputs[inI].posX = sIn.posX;
	                material.nodes[nodeI].inputs[inI].posY = sIn.posY;
	                material.nodes[nodeI].inputs[inI].connectionHover = sIn.connectionHover;
	                material.nodes[nodeI].inputs[inI].pressed = sIn.pressed;
	                material.nodes[nodeI].inputs[inI].connectionPosX = sIn.connectionPosX;
	                material.nodes[nodeI].inputs[inI].connectionPosY = sIn.connectionPosY;
	                material.nodes[nodeI].inputs[inI].nodeConnectionIndex = sIn.nodeConnectionIndex;
	                material.nodes[nodeI].inputs[inI].inputConnectionIndex = sIn.inputConnectionIndex;
	                material.nodes[nodeI].inputs[inI].isConnectedToShaderInput = sIn.isConnectedToShaderInput;
	                material.nodes[nodeI].inputs[inI].textureSelectingState = sIn.textureSelectingState;
	                material.nodes[nodeI].inputs[inI].result = sIn.result;
	                material.nodes[nodeI].inputs[inI].removeTheResult = sIn.removeTheResult;
	                material.nodes[nodeI].inputs[inI].selectedRampIndex = sIn.selectedRampIndex;
	                material.nodes[nodeI].inputs[inI].value = sIn.value;

                    //inputs text
                    uint64_t nodeOutputTextSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].inputs[inI].text.push_back(ch);
                    }
                    //inputs type
                    uint64_t nodeOutputTypeSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTypeSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].inputs[inI].type.push_back(ch);
                    }
                    //inputs element
                    uint64_t nodeOutputElementSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputElementSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].inputs[inI].element.push_back(ch);
                    }
                    //inputs texture
                    uint64_t nodeOutputTextureSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextureSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].inputs[inI].selectedTextureName.push_back(ch);
                    }
                    //inputs Connections
                    uint64_t nodeOutputConnectionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputConnectionSize; i++)
                    {
                        NodeConnection nodeCn;
                        rf.read(reinterpret_cast<char*>(&nodeCn),sizeof(NodeConnection));
                        material.nodes[nodeI].inputs[inI].connections.push_back(nodeCn);
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPositionSize; i++)
                    {
                        float rampPos;
                        rf.read(reinterpret_cast<char*>(&rampPos),sizeof(float));
                        material.nodes[nodeI].inputs[inI].rampPos.push_back(rampPos);
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampColorSize; i++)
                    {
                        glm::vec3 color;
                        rf.read(reinterpret_cast<char*>(&color),sizeof(glm::vec3));
                        material.nodes[nodeI].inputs[inI].rampClr.push_back(color);
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPressedStateSize; i++)
                    {
                        bool b;
                        rf.read(reinterpret_cast<char*>(&b),sizeof(bool));
                        material.nodes[nodeI].inputs[inI].rampPress.push_back(b);
                    }
                    uint64_t rangeBarsPointerPressedSize;
                    rf.read(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                    for (size_t i = 0; i < rangeBarsPointerPressedSize; i++)
                    {
                        material.nodes[nodeI].inputs[inI].rangeBarsPointerPressed.push_back(false);                        
                    }
                }
                

                //Outputs
                uint64_t nodeOutputSize;
                rf.read(reinterpret_cast<char*>(&nodeOutputSize),sizeof(uint64_t));
                for (size_t outI = 0; outI < nodeOutputSize; outI++)
                {
                    NodeInput nodeOut;
                    material.nodes[nodeI].outputs.push_back(nodeOut);
                    //Output's itself
                    sNodeInput sOut;

                    rf.read(reinterpret_cast<char*>(&sOut),sizeof(sNodeInput));
	                material.nodes[nodeI].outputs[outI].addTextureButtonHover = sOut.addTextureButtonHover;
	                material.nodes[nodeI].outputs[outI].removeTextureButtonHover = sOut.removeTextureButtonHover;
	                material.nodes[nodeI].outputs[outI].selectedTextureIndex = sOut.selectedTextureIndex;
	                material.nodes[nodeI].outputs[outI].selectedTexture = sOut.selectedTexture;
	                material.nodes[nodeI].outputs[outI].color = sOut.color;
	                material.nodes[nodeI].outputs[outI].coloringPanelActivated = sOut.coloringPanelActivated;
	                material.nodes[nodeI].outputs[outI].posX = sOut.posX;
	                material.nodes[nodeI].outputs[outI].posY = sOut.posY;
	                material.nodes[nodeI].outputs[outI].connectionHover = sOut.connectionHover;
	                material.nodes[nodeI].outputs[outI].pressed = sOut.pressed;
	                material.nodes[nodeI].outputs[outI].connectionPosX = sOut.connectionPosX;
	                material.nodes[nodeI].outputs[outI].connectionPosY = sOut.connectionPosY;
	                material.nodes[nodeI].outputs[outI].nodeConnectionIndex = sOut.nodeConnectionIndex;
	                material.nodes[nodeI].outputs[outI].inputConnectionIndex = sOut.inputConnectionIndex;
	                material.nodes[nodeI].outputs[outI].isConnectedToShaderInput = sOut.isConnectedToShaderInput;
	                material.nodes[nodeI].outputs[outI].textureSelectingState = sOut.textureSelectingState;
	                material.nodes[nodeI].outputs[outI].result = sOut.result;
	                material.nodes[nodeI].outputs[outI].removeTheResult = sOut.removeTheResult;
	                material.nodes[nodeI].outputs[outI].selectedRampIndex = sOut.selectedRampIndex;
	                material.nodes[nodeI].outputs[outI].value = sOut.value;
                    
                    //Output text
                    uint64_t nodeOutputTextSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].outputs[outI].text.push_back(ch);
                    }
                    //Output type
                    uint64_t nodeOutputTypeSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTypeSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].outputs[outI].type.push_back(ch);
                    }
                    //Output element
                    uint64_t nodeOutputElementSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputElementSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].outputs[outI].element.push_back(ch);
                    }
                    //Output texture
                    uint64_t nodeOutputTextureSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextureSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        material.nodes[nodeI].outputs[outI].selectedTextureName.push_back(ch);
                    }
                    //Output Connections
                    uint64_t nodeOutputConnectionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputConnectionSize; i++)
                    {
                        NodeConnection nc;
                        rf.read(reinterpret_cast<char*>(&nc),sizeof(NodeConnection));
                        material.nodes[nodeI].outputs[outI].connections.push_back(nc);
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPositionSize; i++)
                    {
                        //TODO : FIX RAMP NODE
                        float rampPos;
                        rf.read(reinterpret_cast<char*>(&rampPos),sizeof(float));
                        material.nodes[nodeI].outputs[outI].rampPos.push_back(rampPos);                        
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampColorSize; i++)
                    {
                        glm::vec3 clr;
                        rf.read(reinterpret_cast<char*>(&clr),sizeof(glm::vec3));
                        material.nodes[nodeI].outputs[outI].rampClr.push_back(clr);                        
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPressedStateSize; i++)
                    {
                        bool b;
                        rf.read(reinterpret_cast<char*>(&b),sizeof(bool));
                        material.nodes[nodeI].outputs[outI].rampPress.push_back(b);                        
                    }

                    //rangeBarsPointerPressedSize
                    uint64_t rangeBarsPointerPressedSize;
                    rf.read(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                    for (size_t i = 0; i < rangeBarsPointerPressedSize; i++)
                    {
                        material.nodes[nodeI].outputs[outI].rangeBarsPointerPressed.push_back(false);                        
                    }
                    
                }
                

                //Listboxes
                uint64_t nodeListboxSize;
                rf.read(reinterpret_cast<char*>(&nodeListboxSize),sizeof(uint64_t));
                for (size_t listI = 0; listI < nodeListboxSize; listI++)
                {
                    ListBox lbox;
                    material.nodes[nodeI].listBoxes.push_back(lbox);                        

                    bool active;
                    int chosenIndex;
                    rf.read(reinterpret_cast<char*>(&active),sizeof(bool));
                    rf.read(reinterpret_cast<char*>(&chosenIndex),sizeof(int));
                    material.nodes[nodeI].listBoxes[listI].active = active;
                    material.nodes[nodeI].listBoxes[listI].chosenIndex = chosenIndex;
                    
                    //Listbox elements
                    uint64_t nodeListboxElementSize;
                    rf.read(reinterpret_cast<char*>(&nodeListboxElementSize),sizeof(uint64_t));
                    for (size_t elementI = 0; elementI < nodeListboxElementSize; elementI++)
                    {
                        std::string listelement;
                        material.nodes[nodeI].listBoxes[listI].elements.push_back(listelement);

                        //List box element titles
                        uint64_t nodeListboxElementTextSize;
                        rf.read(reinterpret_cast<char*>(&nodeListboxElementTextSize),sizeof(uint64_t));
                        for (size_t i = 0; i < nodeListboxElementTextSize; i++)
                        {
                            char ch;
                            rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                            material.nodes[nodeI].listBoxes[listI].elements[elementI].push_back(ch);
                        }
                    }
                }
                


                //Node title size
                uint64_t nodeTitleSize;
                rf.read(reinterpret_cast<char*>(&nodeTitleSize),sizeof(uint64_t));
                //Title
                for (size_t i = 0; i < nodeTitleSize; i++)
                {
                    char ch;
                    rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                    material.nodes[nodeI].title.push_back(ch);
                }

            }
            
        return material;
    }
private:
    //--------------------READ THE FILE--------------------
    struct sNode{
    	glm::vec4 upBarColor;
    	glm::vec4 backColor;
    	float unZoomedPositionX; 
    	float unZoomedPositionY; 
    	float positionX; 
    	float positionY;
    	float width;
    	float height;
    	bool barHover;
    	bool barPressed;
    	bool panelHover;
    	int rangeBarCount = 0;
    	unsigned int program;
    	bool isMainOut = false;
    	int renderingIndex = 10000;
    	bool useModel = false;
    	bool marked = false;
    	bool active = false;
    	bool stateChanged = false;
    	int dupI = 0;
    	bool doInvert = true;
    };
    struct sNodeInput{
	    //Texture element
    	glm::vec4 value;
	    bool addTextureButtonHover;
	    bool removeTextureButtonHover;
	    int selectedTextureIndex = 10000;
	    unsigned int selectedTexture = 0;
	    //Color element
	    glm::vec3 color = glm::vec3(0);
	    bool coloringPanelActivated;
	    float posX;
	    float posY;
	    bool connectionHover = false;
	    bool pressed;
	    //Input
	    float connectionPosX = 0;
	    float connectionPosY = 0;
	    int nodeConnectionIndex = 10000;
	    int inputConnectionIndex = 10000;
	    bool isConnectedToShaderInput = false;
	    bool textureSelectingState = false;
	    unsigned int result;
	    bool removeTheResult = true;
	    int selectedRampIndex = 0;
    };

    bool startReadingTheProjectFile(const char* path,std::ifstream &rf){
        rf = std::ifstream(path, std::ios::out | std::ios::binary);
        if(!rf.is_open()){
            std::cout << "ERROR WHILE READING THE LIGID MATERIAL FILE! Cannot open file : " << path << std::endl;
            return false;
        }
    }
};

#endif //LIGID_WRITE_READ_MATERIAL_FILE