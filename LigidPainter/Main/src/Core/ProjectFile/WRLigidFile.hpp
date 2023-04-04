#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

#include "../../thirdparty/include/glad/glad.h"

#include "Core/Model/model.h"
#include "Core/UI/UserInterface.h"
#include "Core/Texture/Texture.h"
#include "Core/gl.h"

//LIGID PAINTER PROJECT FILE GENERATOR
//This single header generates *.ligid file witch contains loaded 3d model, textures & materials

//Documentation

//Every LigidPainter project file shall start with 0x94DA0DB9 0xF53B4451 0xD0E355DE   

//3D Model code 0xB2911B6A 0x6825EA68 0x59EF9DF5

//Vertices code 0x1DCEC243 0xD25EF96B 0xF6A94C0C + size of the vertices 
//Example : 0x1DCEC243 0xD25EF96B 0xF6A94C0C 0x003D0900 (same goes for the indices)

//Indices code 0x3600353C 0xB3ACAD02 0x6C23DE2C + size of the indices

//Texture code 0xA7F8F642 0x99CAD11E 0x6F5E55A8 + size of the texture elements + texture resolution
//Example : 0xA7F8F642 0x99CAD11E 0x6F5E55A8 0x0000000a 0x00000400

//Texture element code 0xB7A58C82 0xF7F7B273 0x3C82A6FF + size of the elements

//Node Scene (Material code) 0x0A78DA67 0xF99C3CA7 0x36CB89A5 + size of the node scenes



//TODO : Multiple meshes

class LigidFile{
public:
    Model writeTheFile(const char* path,Model model,std::vector<aTexture> textures,std::vector<NodeScene> nodeScenes,int textureResIndex){

        std::ofstream wf = createTheProjectFile(path);

        writeModelData(wf,model);
        writeTextures(wf,textures,textureResIndex);
        drawTextureElements(wf,textures);
        writeNodeScenes(wf,nodeScenes);

        wf.close();

        return model;
    }
    void readTheFile(const char* path,Model &model,std::vector<aTexture> &textures, std::vector<NodeScene> &nodeScenes){
        ifstream rf;
        std::cout << "READING";

        if(startReadingTheProjectFile(path,rf)){
            //3D Model
            textures.clear();
            model.meshes.clear();
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            
            uint64_t c;
            int modelKeysCounter = 0;
            int verticeKeysCounter = 0;
            int indiceKeysCounter = 0;
            int textureKeysCounter = 0;
            int textureElementKeysCounter = 0;
            int nodeScenesKeyCounter = 0;

            int counter = 0;
            while(rf.read(reinterpret_cast<char*>(&c),sizeof(uint64_t))){
                
                updateCounters(c,modelKeysCounter,verticeKeysCounter,indiceKeysCounter,textureKeysCounter,textureElementKeysCounter,nodeScenesKeyCounter);
                
                //3D Model (Is not necessary)
                if(modelKeysCounter == 3){
                    modelKeysCounter = 0;
                }

                //Vertices
                if(verticeKeysCounter == 3){
                    verticeKeysCounter = 0;
                    readVertices(rf,vertices);
                }

                //Indices
                if(indiceKeysCounter == 3){
                    indiceKeysCounter = 0;
                    readIndices(rf,indices);
                }

                //Textures
                if(textureKeysCounter == 3){
                    textureKeysCounter = 0;
                    readTexture(rf,textures);
                }
                
                //Textures
                if(textureElementKeysCounter == 3){
                    textureElementKeysCounter = 0;
                    readTextureElements(rf,textures);
                }
                
                //Node Scenes
                if(nodeScenesKeyCounter == 3){
                    nodeScenesKeyCounter = 0;
                    readNodeScenes(rf,nodeScenes);
                }

                counter++;
            }
            model.meshes.push_back(Mesh(vertices, indices, {},"",0));
            finishTheModel(model);
        }
        std::cout << "\n READING ENDED";
        rf.close();
    }
private:
    //--------------------READ THE FILE--------------------
    struct sTexture{
	    unsigned int id;
	    bool nameTextActive = false;
	    bool isTexture = true;
	    bool isTrashFolder = false;
	    bool rightClicked = false;
	    int folderIndex = 10000;
    };
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
            std::cout << "ERROR WHILE READING THE LIGID PROJECT FILE! Cannot open file : " << path << std::endl;
            return false;
        }
        uint64_t keys[3] = {0x94DA0DB9 ,0xF53B4451 ,0xD0E355DE};
        for (size_t i = 0; i < 3; i++)
        {
            uint64_t c;
            rf.read(reinterpret_cast<char*>(&c),sizeof(uint64_t));
            if(c != keys[i]){
                std::cout << "ERROR WHILE READING THE LIGID PROJECT FILE! This file is not associated with LigidPainter : " << path << std::endl;
                return false;
            }
        }
        return true;
    }
    
    void updateCounters(uint64_t c,int &modelKeysCounter,int &verticeKeysCounter,int &indiceKeysCounter,int &textureKeysCounter, int &textureElementKeysCounter,int &nodeScenesKeyCounter){
        uint64_t modelKeys[3] =  {0xB2911B6A, 0x6825EA68, 0x59EF9DF5};
        uint64_t verticeKeys[3] =  {0x1DCEC243, 0xD25EF96B, 0xF6A94C0C};
        uint64_t indiceKeys[3] =  {0x3600353C, 0xB3ACAD02, 0x6C23DE2C};
        uint64_t textureKeys[3] =  {0xA7F8F642, 0x99CAD11E, 0x6F5E55A8};
        uint64_t textureElementKeys[3] =  {0xB7A58C82 ,0xF7F7B273 ,0x3C82A6FF};
        uint64_t nodeSceneKeys[3] =  {0x0A78DA67, 0xF99C3CA7, 0x36CB89A5};

        if(c == modelKeys[modelKeysCounter])
            modelKeysCounter++;
        else{
            modelKeysCounter = 0;
        }
        if(c == verticeKeys[verticeKeysCounter])
            verticeKeysCounter++;
        else{
            verticeKeysCounter = 0;
        }
        if(c == indiceKeys[indiceKeysCounter])
            indiceKeysCounter++;
        else{
            indiceKeysCounter = 0;
        }
        if(c == textureKeys[textureKeysCounter])
            textureKeysCounter++;
        else{
            textureKeysCounter = 0;
        }
        if(c == textureElementKeys[textureElementKeysCounter])
            textureElementKeysCounter++;
        else{
            textureElementKeysCounter = 0;
        }
        if(c == nodeSceneKeys[nodeScenesKeyCounter])
            nodeScenesKeyCounter++;
        else{
            nodeScenesKeyCounter = 0;
        }
    }

    void readVertices(std::ifstream &rf,std::vector<Vertex> &vertices){
        uint64_t verticeCount = 0;
        rf.read(reinterpret_cast<char*>(&verticeCount),sizeof(uint64_t));
        
        for (size_t i = 0; i < verticeCount; i++)
        {
            Vertex vertex;
            rf.read(reinterpret_cast<char*>(&vertex),sizeof(Vertex));
            vertices.push_back(vertex);
        }
    }
    void readIndices(std::ifstream &rf,std::vector<unsigned int> &indices){
        uint64_t indiceCount = 0;
        rf.read(reinterpret_cast<char*>(&indiceCount),sizeof(uint64_t));
        
        
        for (size_t i = 0; i < indiceCount; i++)
        {
            unsigned int index;
            rf.read(reinterpret_cast<char*>(&index),sizeof(unsigned int));
            indices.push_back(index);
        }
    }
    void readTexture(std::ifstream &rf,std::vector<aTexture>&textures){
        uint64_t textureCount = 0;
        rf.read(reinterpret_cast<char*>(&textureCount),sizeof(uint64_t));
        uint64_t resolution = 0;
        rf.read(reinterpret_cast<char*>(&resolution),sizeof(uint64_t));
        
        uint64_t pixelCount = resolution*resolution;

        for (size_t txtrI = 0; txtrI < textureCount; txtrI++)
        {
            GLubyte* textureData = new GLubyte[pixelCount*4];
            aTexture txtr;
            for (size_t pxI = 0; pxI < pixelCount*4; pxI++)
            {
                GLubyte data;
                rf.read(reinterpret_cast<char*>(&data),sizeof(GLubyte));
                textureData[pxI] = data;
            }
            glActiveTexture(GL_TEXTURE28);
            unsigned int textureId;
            glGenTextures(1,&textureId);
            glBindTexture(GL_TEXTURE_2D,textureId);
            GlSet gls;
            gls.texImage(textureData,resolution,resolution,GL_RGBA);
            gls.generateMipmap();
            delete[] textureData;

            txtr.id = textureId;
            txtr.folderIndex = 10000;
            txtr.isTexture = true;
            txtr.name = "importedTexture";
            textures.push_back(txtr);
        }        
    }
    void readTextureElements(std::ifstream &rf,std::vector<aTexture>&textures){
        std::vector<aTexture> generatedTextureArray;
        
        uint64_t textureElementCount = 0;
        rf.read(reinterpret_cast<char*>(&textureElementCount),sizeof(uint64_t));

        int orgTxtrI = 0;
        for (size_t i = 0; i < textureElementCount; i++)
        {
            sTexture element;
            rf.read(reinterpret_cast<char*>(&element),sizeof(sTexture));

            if(element.isTexture){
                element.id = textures[orgTxtrI].id; 
                orgTxtrI++;
            }
            aTexture atxtr;
            atxtr.id = element.id;
            atxtr.folderIndex = element.folderIndex;
            atxtr.isTexture = element.isTexture;
            atxtr.isTrashFolder = element.isTrashFolder;
            atxtr.nameTextActive = false;

            uint64_t txtrNameSize = 0;
            rf.read(reinterpret_cast<char*>(&txtrNameSize),sizeof(uint64_t));
            for (size_t cI = 0; cI < txtrNameSize; cI++)
            {
                char txtrNameC;
                rf.read(reinterpret_cast<char*>(&txtrNameC),sizeof(char));
                atxtr.name.push_back(txtrNameC);
            }

            generatedTextureArray.push_back(atxtr);
        }
        textures = generatedTextureArray;
    }

    void finishTheModel(Model &model){
        model.meshes[0].materialIndex = 0;
        model.meshes[0].materialName = "loadedMaterial";

        model.uploadModel(model);
    }
    int ccc = 0;
    void ff(){
        std::cout << "HERE :" << ccc << ' ';
        ccc++;
    }

    void readNodeScenes(std::ifstream &rf,std::vector<NodeScene> &nodeScenes){
        
        nodeScenes.clear();

        uint64_t nodeSceneSize;
        rf.read(reinterpret_cast<char*>(&nodeSceneSize),sizeof(uint64_t));
        //Node Scene
        for (size_t sceneI = 0; sceneI < nodeSceneSize; sceneI++)
        {
            NodeScene scene;
            nodeScenes.push_back(scene);
        
            //Scene name
            uint64_t sceneNameSize;
            rf.read(reinterpret_cast<char*>(&sceneNameSize),sizeof(uint64_t));
            for (size_t i = 0; i < sceneNameSize; i++)
            {
                char ch;
                rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                nodeScenes[sceneI].sceneName.push_back(ch);
            }
            
            //Scene Nodes
            uint64_t nodeSize;
            rf.read(reinterpret_cast<char*>(&nodeSize),sizeof(uint64_t));
            for (size_t nodeI = 0; nodeI < nodeSize; nodeI++)
            {


                Node node;
                nodeScenes[sceneI].nodes.push_back(node);
                ff();

                //Node's itself
                sNode snode;
                rf.read(reinterpret_cast<char*>(&snode),sizeof(sNode));
                nodeScenes[sceneI].nodes[nodeI].upBarColor = snode.upBarColor;
    	        nodeScenes[sceneI].nodes[nodeI].backColor = snode.backColor;
    	        nodeScenes[sceneI].nodes[nodeI].unZoomedPositionX = snode.unZoomedPositionX; 
    	        nodeScenes[sceneI].nodes[nodeI].unZoomedPositionY = snode.unZoomedPositionY; 
    	        nodeScenes[sceneI].nodes[nodeI].positionX = snode.positionX; 
    	        nodeScenes[sceneI].nodes[nodeI].positionY = snode.positionY;
    	        nodeScenes[sceneI].nodes[nodeI].width = snode.width;
    	        nodeScenes[sceneI].nodes[nodeI].height = snode.height;
    	        nodeScenes[sceneI].nodes[nodeI].barHover = snode.barHover;
    	        nodeScenes[sceneI].nodes[nodeI].barPressed = snode.barPressed;
    	        nodeScenes[sceneI].nodes[nodeI].panelHover = snode.panelHover;
    	        nodeScenes[sceneI].nodes[nodeI].rangeBarCount = snode.rangeBarCount;
    	        nodeScenes[sceneI].nodes[nodeI].program = snode.program;
    	        nodeScenes[sceneI].nodes[nodeI].isMainOut = snode.isMainOut;
    	        nodeScenes[sceneI].nodes[nodeI].renderingIndex = snode.renderingIndex;
    	        nodeScenes[sceneI].nodes[nodeI].useModel = snode.useModel;
    	        nodeScenes[sceneI].nodes[nodeI].marked = snode.marked;
    	        nodeScenes[sceneI].nodes[nodeI].active = snode.active;
    	        nodeScenes[sceneI].nodes[nodeI].stateChanged = snode.stateChanged;
    	        nodeScenes[sceneI].nodes[nodeI].dupI = snode.dupI;
    	        nodeScenes[sceneI].nodes[nodeI].doInvert = snode.doInvert;
                ff();

                //Inputs
                uint64_t nodeInputSize;
                rf.read(reinterpret_cast<char*>(&nodeInputSize),sizeof(uint64_t));
                for (size_t inI = 0; inI < nodeInputSize; inI++)
                {
                    NodeInput nodeIn;
                    nodeScenes[sceneI].nodes[nodeI].inputs.push_back(nodeIn);


                    //Input's itself
                    sNodeInput sIn;
                    rf.read(reinterpret_cast<char*>(&sIn),sizeof(sNodeInput));

	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].addTextureButtonHover = sIn.addTextureButtonHover;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].removeTextureButtonHover = sIn.removeTextureButtonHover;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTextureIndex = sIn.selectedTextureIndex;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTexture = sIn.selectedTexture;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].color = sIn.color;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].coloringPanelActivated = sIn.coloringPanelActivated;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].posX = sIn.posX;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].posY = sIn.posY;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].connectionHover = sIn.connectionHover;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].pressed = sIn.pressed;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].connectionPosX = sIn.connectionPosX;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].connectionPosY = sIn.connectionPosY;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].nodeConnectionIndex = sIn.nodeConnectionIndex;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].inputConnectionIndex = sIn.inputConnectionIndex;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].isConnectedToShaderInput = sIn.isConnectedToShaderInput;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].textureSelectingState = sIn.textureSelectingState;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].result = sIn.result;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].removeTheResult = sIn.removeTheResult;
	                nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedRampIndex = sIn.selectedRampIndex;

                    //inputs text
                    uint64_t nodeOutputTextSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].text.push_back(ch);
                    }
                    //inputs type
                    uint64_t nodeOutputTypeSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTypeSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].type.push_back(ch);
                    }
                    //inputs element
                    uint64_t nodeOutputElementSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputElementSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].element.push_back(ch);
                    }
                    //inputs texture
                    uint64_t nodeOutputTextureSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextureSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTextureName.push_back(ch);
                    }
                    //inputs Connections
                    uint64_t nodeOutputConnectionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputConnectionSize; i++)
                    {
                        NodeConnection nodeCn;
                        rf.read(reinterpret_cast<char*>(&nodeCn),sizeof(NodeConnection));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].connections.push_back(nodeCn);
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPositionSize; i++)
                    {
                        float rampPos;
                        rf.read(reinterpret_cast<char*>(&rampPos),sizeof(float));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPos.push_back(rampPos);
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampColorSize; i++)
                    {
                        glm::vec3 color;
                        rf.read(reinterpret_cast<char*>(&color),sizeof(glm::vec3));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampClr.push_back(color);
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPressedStateSize; i++)
                    {
                        bool b;
                        rf.read(reinterpret_cast<char*>(&b),sizeof(bool));
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPress.push_back(b);
                    }
                    uint64_t rangeBarsPointerPressedSize;
                    rf.read(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                    for (size_t i = 0; i < rangeBarsPointerPressedSize; i++)
                    {
                        nodeScenes[sceneI].nodes[nodeI].inputs[inI].rangeBarsPointerPressed.push_back(false);                        
                    }
                }
                ff();

                //Outputs
                uint64_t nodeOutputSize;
                rf.read(reinterpret_cast<char*>(&nodeOutputSize),sizeof(uint64_t));
                for (size_t outI = 0; outI < nodeOutputSize; outI++)
                {
                    NodeInput nodeOut;
                    nodeScenes[sceneI].nodes[nodeI].outputs.push_back(nodeOut);
                    //Output's itself
                    sNodeInput sOut;

                    rf.read(reinterpret_cast<char*>(&sOut),sizeof(sNodeInput));
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].addTextureButtonHover = sOut.addTextureButtonHover;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].removeTextureButtonHover = sOut.removeTextureButtonHover;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTextureIndex = sOut.selectedTextureIndex;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTexture = sOut.selectedTexture;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].color = sOut.color;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].coloringPanelActivated = sOut.coloringPanelActivated;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].posX = sOut.posX;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].posY = sOut.posY;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].connectionHover = sOut.connectionHover;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].pressed = sOut.pressed;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].connectionPosX = sOut.connectionPosX;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].connectionPosY = sOut.connectionPosY;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].nodeConnectionIndex = sOut.nodeConnectionIndex;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].inputConnectionIndex = sOut.inputConnectionIndex;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].isConnectedToShaderInput = sOut.isConnectedToShaderInput;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].textureSelectingState = sOut.textureSelectingState;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].result = sOut.result;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].removeTheResult = sOut.removeTheResult;
	                nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedRampIndex = sOut.selectedRampIndex;
                    
                    //Output text
                    uint64_t nodeOutputTextSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].text.push_back(ch);
                    }
                    //Output type
                    uint64_t nodeOutputTypeSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTypeSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].type.push_back(ch);
                    }
                    //Output element
                    uint64_t nodeOutputElementSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputElementSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].element.push_back(ch);
                    }
                    //Output texture
                    uint64_t nodeOutputTextureSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputTextureSize; i++)
                    {
                        char ch;
                        rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTextureName.push_back(ch);
                    }
                    //Output Connections
                    uint64_t nodeOutputConnectionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputConnectionSize; i++)
                    {
                        NodeConnection nc;
                        rf.read(reinterpret_cast<char*>(&nc),sizeof(NodeConnection));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].connections.push_back(nc);
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPositionSize; i++)
                    {
                        //TODO : FIX RAMP NODE
                        float rampPos;
                        rf.read(reinterpret_cast<char*>(&rampPos),sizeof(float));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPos.push_back(rampPos);                        
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampColorSize; i++)
                    {
                        glm::vec3 clr;
                        rf.read(reinterpret_cast<char*>(&clr),sizeof(glm::vec3));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampClr.push_back(clr);                        
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize;
                    rf.read(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeOutputRampPressedStateSize; i++)
                    {
                        bool b;
                        rf.read(reinterpret_cast<char*>(&b),sizeof(bool));
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPress.push_back(b);                        
                    }

                    //rangeBarsPointerPressedSize
                    uint64_t rangeBarsPointerPressedSize;
                    rf.read(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                    for (size_t i = 0; i < rangeBarsPointerPressedSize; i++)
                    {
                        nodeScenes[sceneI].nodes[nodeI].outputs[outI].rangeBarsPointerPressed.push_back(false);                        
                    }
                    
                }
                ff();

                //Listboxes
                uint64_t nodeListboxSize;
                rf.read(reinterpret_cast<char*>(&nodeListboxSize),sizeof(uint64_t));
                for (size_t listI = 0; listI < nodeListboxSize; listI++)
                {
                    ListBox lbox;
                    nodeScenes[sceneI].nodes[nodeI].listBoxes.push_back(lbox);                        

                    bool active;
                    int chosenIndex;
                    rf.read(reinterpret_cast<char*>(&active),sizeof(bool));
                    rf.read(reinterpret_cast<char*>(&chosenIndex),sizeof(int));
                    nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].active = active;
                    nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].chosenIndex = chosenIndex;
                    
                    //Listbox elements
                    uint64_t nodeListboxElementSize;
                    rf.read(reinterpret_cast<char*>(&nodeListboxElementSize),sizeof(uint64_t));
                    for (size_t elementI = 0; elementI < nodeListboxElementSize; elementI++)
                    {
                        std::string listelement;
                        nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].elements.push_back(listelement);

                        //List box element titles
                        uint64_t nodeListboxElementTextSize;
                        rf.read(reinterpret_cast<char*>(&nodeListboxElementTextSize),sizeof(uint64_t));
                        for (size_t i = 0; i < nodeListboxElementTextSize; i++)
                        {
                            char ch;
                            rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                            nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].elements[elementI].push_back(ch);
                        }
                    }
                }
                ff();


                //Node title size
                uint64_t nodeTitleSize;
                rf.read(reinterpret_cast<char*>(&nodeTitleSize),sizeof(uint64_t));
                //Title
                for (size_t i = 0; i < nodeTitleSize; i++)
                {
                    char ch;
                    rf.read(reinterpret_cast<char*>(&ch),sizeof(char));
                    nodeScenes[sceneI].nodes[nodeI].title.push_back(ch);
                }

            }
        }   
    }

    //--------------------CREATE THE FILE--------------------

    std::ofstream createTheProjectFile(const char* path){
    //Create the ofstream & write the ligidpainter header code
        uint64_t h1 = 0x94DA0DB9; 
        uint64_t h2 = 0xF53B4451; 
        uint64_t h3 = 0xD0E355DE; 

        std::ofstream wf(path, std::ios::out | std::ios::binary);
        if(!wf) {
            std::cout << "ERROR WHILE CREATING LIGID PROJECT FILE! Cannot open file : " << path << std::endl;
        }

        wf.write(reinterpret_cast<char*>(&h1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&h2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&h3),sizeof(uint64_t));

        return wf;
    }
    void writeModelData(std::ofstream &wf,Model model){
        //Model
        uint64_t model1 = 0xB2911B6A;  
        uint64_t model2 = 0x6825EA68;  
        uint64_t model3 = 0x59EF9DF5; 

        uint64_t vert1 = 0x1DCEC243;    
        uint64_t vert2 = 0xD25EF96B;   
        uint64_t vert3 = 0xF6A94C0C;  
        
        uint64_t indice1 = 0x3600353C; 
        uint64_t indice2 = 0xB3ACAD02; 
        uint64_t indice3 = 0x6C23DE2C;
        
        wf.write(reinterpret_cast<char*>(&model1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&model2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&model3),sizeof(uint64_t));

        
        for (size_t meshI = 0; meshI < model.meshes.size(); meshI++)
        {
            //--------Vertices
            wf.write(reinterpret_cast<char*>(&vert1),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(&vert2),sizeof(uint64_t));
            wf.write(reinterpret_cast<char*>(&vert3),sizeof(uint64_t));
            //Write the size
            uint64_t vertSize = model.meshes[meshI].vertices.size();
            wf.write(reinterpret_cast<char*>(&vertSize),sizeof(uint64_t));

            for (size_t verticeI = 0; verticeI < model.meshes[meshI].vertices.size(); verticeI++)
            {
                wf.write(reinterpret_cast<char*>(&model.meshes[meshI].vertices[verticeI]),sizeof(Vertex));
            }
            
            //--------Indices
             wf.write(reinterpret_cast<char*>(&indice1),sizeof(uint64_t));
             wf.write(reinterpret_cast<char*>(&indice2),sizeof(uint64_t));
             wf.write(reinterpret_cast<char*>(&indice3),sizeof(uint64_t));
            //Write the size
            uint64_t indiceSize = model.meshes[meshI].indices.size();
            wf.write(reinterpret_cast<char*>(&indiceSize),sizeof(uint64_t));
            
            for (size_t indiceI = 0; indiceI < model.meshes[meshI].indices.size(); indiceI++)
            {
                wf.write(reinterpret_cast<char*>(&model.meshes[meshI].indices[indiceI]),sizeof(unsigned int));
            }
        }
    }

    void writeTextures(std::ofstream &wf,std::vector<aTexture> textures,int textureResIndex){
        int txtrRes = 256;
	    for (size_t i = 0; i < chosenTextureResIndex; i++)
	    {
	    	txtrRes*=2;
	    }

        uint64_t txtr1 = 0xA7F8F642; 
        uint64_t txtr2 = 0x99CAD11E; 
        uint64_t txtr3 = 0x6F5E55A8;
        wf.write(reinterpret_cast<char*>(&txtr1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtr2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtr3),sizeof(uint64_t));
        
        int texturesSize = 0;
        for (size_t i = 0; i < textures.size(); i++)
        {
            if(textures[i].isTexture)
                texturesSize++;
        }
        

        uint64_t textureElementSize = texturesSize;
        wf.write(reinterpret_cast<char*>(&textureElementSize),sizeof(uint64_t));
        uint64_t textureResolutionSize = txtrRes;
        wf.write(reinterpret_cast<char*>(&textureResolutionSize),sizeof(uint64_t));
        
        //----Get the texture data
        //TODO : Get the width & height
        Texture txtr;
        for (size_t i = 0; i < textures.size(); i++)
        {
            if(textures[i].isTexture){
                glActiveTexture(GL_TEXTURE28);
                glBindTexture(GL_TEXTURE_2D,textures[i].id);
                GLubyte* data = txtr.getTextureFromProgram(GL_TEXTURE28,txtrRes,txtrRes,4);
                for (size_t wh = 0; wh < txtrRes*txtrRes*4; wh++)
                {
                    wf.write(reinterpret_cast<char*>(&data[wh]),sizeof(GLubyte));
                }
            }
        }
        //----Write the texture data
    }

    void drawTextureElements(std::ofstream &wf,std::vector<aTexture> textures){
        uint64_t txtrE1 = 0xB7A58C82;
        uint64_t txtrE2 = 0xF7F7B273;
        uint64_t txtrE3 = 0x3C82A6FF;
        wf.write(reinterpret_cast<char*>(&txtrE1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtrE2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&txtrE3),sizeof(uint64_t));

        uint64_t textureElementSize = textures.size();
        wf.write(reinterpret_cast<char*>(&textureElementSize),sizeof(uint64_t));

        for (size_t i = 0; i < textures.size(); i++)
        {
            sTexture stexture;
            stexture.id = textures[i].id;
            stexture.isTexture = textures[i].isTexture;
            stexture.isTrashFolder = textures[i].isTrashFolder;
            stexture.folderIndex = textures[i].folderIndex;

            wf.write(reinterpret_cast<char*>(&stexture),sizeof(sTexture));
            
            uint64_t txtrNameSize = textures[i].name.size();
            wf.write(reinterpret_cast<char*>(&txtrNameSize),sizeof(uint64_t));

            for (size_t cI = 0; cI < textures[i].name.size(); cI++)
            {
                wf.write(reinterpret_cast<char*>(&textures[i].name[cI]),sizeof(char));
            }
        }
    }

    //Nodescene key -> nodescene count
    //Rendering pipeline is ignored
    void writeNodeScenes(std::ofstream &wf,std::vector<NodeScene> nodeScenes){
        
        
        uint64_t nodeSC1 = 0x0A78DA67; 
        uint64_t nodeSC2 = 0xF99C3CA7; 
        uint64_t nodeSC3 = 0x36CB89A5;
        wf.write(reinterpret_cast<char*>(&nodeSC1),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&nodeSC2),sizeof(uint64_t));
        wf.write(reinterpret_cast<char*>(&nodeSC3),sizeof(uint64_t));

        uint64_t nodeSceneSize = nodeScenes.size();
        wf.write(reinterpret_cast<char*>(&nodeSceneSize),sizeof(uint64_t));

        //Node Scene
        for (size_t sceneI = 0; sceneI < nodeScenes.size(); sceneI++)
        {

            //Scene name
            uint64_t sceneNameSize = nodeScenes[sceneI].sceneName.size();
            wf.write(reinterpret_cast<char*>(&sceneNameSize),sizeof(uint64_t));
            for (size_t i = 0; i < nodeScenes[sceneI].sceneName.size(); i++)
            {
                wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].sceneName[i]),sizeof(char));
            }
            
            //Scene Nodes
            uint64_t nodeSize = nodeScenes[sceneI].nodes.size();
            wf.write(reinterpret_cast<char*>(&nodeSize),sizeof(uint64_t));
            for (size_t nodeI = 0; nodeI < nodeScenes[sceneI].nodes.size(); nodeI++)
            {
                //Node's itself
                sNode snode;
                snode.upBarColor = nodeScenes[sceneI].nodes[nodeI].upBarColor;
    	        snode.backColor = nodeScenes[sceneI].nodes[nodeI].backColor;
    	        snode.unZoomedPositionX = nodeScenes[sceneI].nodes[nodeI].unZoomedPositionX; 
    	        snode.unZoomedPositionY = nodeScenes[sceneI].nodes[nodeI].unZoomedPositionY; 
    	        snode.positionX = nodeScenes[sceneI].nodes[nodeI].positionX; 
    	        snode.positionY = nodeScenes[sceneI].nodes[nodeI].positionY;
    	        snode.width = nodeScenes[sceneI].nodes[nodeI].width;
    	        snode.height = nodeScenes[sceneI].nodes[nodeI].height;
    	        snode.barHover = nodeScenes[sceneI].nodes[nodeI].barHover;
    	        snode.barPressed = nodeScenes[sceneI].nodes[nodeI].barPressed;
    	        snode.panelHover = nodeScenes[sceneI].nodes[nodeI].panelHover;
    	        snode.rangeBarCount = nodeScenes[sceneI].nodes[nodeI].rangeBarCount;
    	        snode.program = nodeScenes[sceneI].nodes[nodeI].program;
    	        snode.isMainOut = nodeScenes[sceneI].nodes[nodeI].isMainOut;
    	        snode.renderingIndex = nodeScenes[sceneI].nodes[nodeI].renderingIndex;
    	        snode.useModel = nodeScenes[sceneI].nodes[nodeI].useModel;
    	        snode.marked = nodeScenes[sceneI].nodes[nodeI].marked;
    	        snode.active = nodeScenes[sceneI].nodes[nodeI].active;
    	        snode.stateChanged = nodeScenes[sceneI].nodes[nodeI].stateChanged;
    	        snode.dupI = nodeScenes[sceneI].nodes[nodeI].dupI;
    	        snode.doInvert = nodeScenes[sceneI].nodes[nodeI].doInvert;
                wf.write(reinterpret_cast<char*>(&snode),sizeof(sNode));

                //Inputs
                uint64_t nodeInputSize = nodeScenes[sceneI].nodes[nodeI].inputs.size();
                wf.write(reinterpret_cast<char*>(&nodeInputSize),sizeof(uint64_t));
                for (size_t inI = 0; inI < nodeScenes[sceneI].nodes[nodeI].inputs.size(); inI++)
                {
                    //Input's itself
                    sNodeInput sIn;
	                sIn.addTextureButtonHover = nodeScenes[sceneI].nodes[nodeI].inputs[inI].addTextureButtonHover;
	                sIn.removeTextureButtonHover = nodeScenes[sceneI].nodes[nodeI].inputs[inI].removeTextureButtonHover;
	                sIn.selectedTextureIndex = nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTextureIndex;
	                sIn.selectedTexture = nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTexture;
	                sIn.color = nodeScenes[sceneI].nodes[nodeI].inputs[inI].color;
	                sIn.coloringPanelActivated = nodeScenes[sceneI].nodes[nodeI].inputs[inI].coloringPanelActivated;
	                sIn.posX = nodeScenes[sceneI].nodes[nodeI].inputs[inI].posX;
	                sIn.posY = nodeScenes[sceneI].nodes[nodeI].inputs[inI].posY;
	                sIn.connectionHover = nodeScenes[sceneI].nodes[nodeI].inputs[inI].connectionHover;
	                sIn.pressed = nodeScenes[sceneI].nodes[nodeI].inputs[inI].pressed;
	                sIn.connectionPosX = nodeScenes[sceneI].nodes[nodeI].inputs[inI].connectionPosX;
	                sIn.connectionPosY = nodeScenes[sceneI].nodes[nodeI].inputs[inI].connectionPosY;
	                sIn.nodeConnectionIndex = nodeScenes[sceneI].nodes[nodeI].inputs[inI].nodeConnectionIndex;
	                sIn.inputConnectionIndex = nodeScenes[sceneI].nodes[nodeI].inputs[inI].inputConnectionIndex;
	                sIn.isConnectedToShaderInput = nodeScenes[sceneI].nodes[nodeI].inputs[inI].isConnectedToShaderInput;
	                sIn.textureSelectingState = nodeScenes[sceneI].nodes[nodeI].inputs[inI].textureSelectingState;
	                sIn.result = nodeScenes[sceneI].nodes[nodeI].inputs[inI].result;
	                sIn.removeTheResult = nodeScenes[sceneI].nodes[nodeI].inputs[inI].removeTheResult;
	                sIn.selectedRampIndex = nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedRampIndex;

                    wf.write(reinterpret_cast<char*>(&sIn),sizeof(sNodeInput));

                    //inputs text
                    uint64_t nodeOutputTextSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].text.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].text.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].text[i]),sizeof(char));
                    }
                    //inputs type
                    uint64_t nodeOutputTypeSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].type.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].type.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].type[i]),sizeof(char));
                    }
                    //inputs element
                    uint64_t nodeOutputElementSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].element.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].element.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].element[i]),sizeof(char));
                    }
                    //inputs texture
                    uint64_t nodeOutputTextureSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTextureName.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTextureName.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].selectedTextureName[i]),sizeof(char));
                    }
                    //inputs Connections
                    uint64_t nodeOutputConnectionSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].connections.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].connections.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].connections[i]),sizeof(NodeConnection));
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPos.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPos.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPos[i]),sizeof(float));
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampClr.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampClr.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampClr[i]),sizeof(glm::vec3));
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPress.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPress.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].inputs[inI].rampPress[i]),sizeof(bool));
                    }
 
                    //rangeBarsPointerPressedSize
                    uint64_t rangeBarsPointerPressedSize = nodeScenes[sceneI].nodes[nodeI].inputs[inI].rangeBarsPointerPressed.size();
                    wf.write(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                }

                //Outputs
                uint64_t nodeOutputSize = nodeScenes[sceneI].nodes[nodeI].outputs.size();
                wf.write(reinterpret_cast<char*>(&nodeOutputSize),sizeof(uint64_t));
                for (size_t outI = 0; outI < nodeScenes[sceneI].nodes[nodeI].outputs.size(); outI++)
                {
                    //Output's itself
                    sNodeInput sOut;
	                sOut.addTextureButtonHover = nodeScenes[sceneI].nodes[nodeI].outputs[outI].addTextureButtonHover;
	                sOut.removeTextureButtonHover = nodeScenes[sceneI].nodes[nodeI].outputs[outI].removeTextureButtonHover;
	                sOut.selectedTextureIndex = nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTextureIndex;
	                sOut.selectedTexture = nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTexture;
	                sOut.color = nodeScenes[sceneI].nodes[nodeI].outputs[outI].color;
	                sOut.coloringPanelActivated = nodeScenes[sceneI].nodes[nodeI].outputs[outI].coloringPanelActivated;
	                sOut.posX = nodeScenes[sceneI].nodes[nodeI].outputs[outI].posX;
	                sOut.posY = nodeScenes[sceneI].nodes[nodeI].outputs[outI].posY;
	                sOut.connectionHover = nodeScenes[sceneI].nodes[nodeI].outputs[outI].connectionHover;
	                sOut.pressed = nodeScenes[sceneI].nodes[nodeI].outputs[outI].pressed;
	                sOut.connectionPosX = nodeScenes[sceneI].nodes[nodeI].outputs[outI].connectionPosX;
	                sOut.connectionPosY = nodeScenes[sceneI].nodes[nodeI].outputs[outI].connectionPosY;
	                sOut.nodeConnectionIndex = nodeScenes[sceneI].nodes[nodeI].outputs[outI].nodeConnectionIndex;
	                sOut.inputConnectionIndex = nodeScenes[sceneI].nodes[nodeI].outputs[outI].inputConnectionIndex;
	                sOut.isConnectedToShaderInput = nodeScenes[sceneI].nodes[nodeI].outputs[outI].isConnectedToShaderInput;
	                sOut.textureSelectingState = nodeScenes[sceneI].nodes[nodeI].outputs[outI].textureSelectingState;
	                sOut.result = nodeScenes[sceneI].nodes[nodeI].outputs[outI].result;
	                sOut.removeTheResult = nodeScenes[sceneI].nodes[nodeI].outputs[outI].removeTheResult;
	                sOut.selectedRampIndex = nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedRampIndex;

                    wf.write(reinterpret_cast<char*>(&sOut),sizeof(sNodeInput));
                    
                    //Output text
                    uint64_t nodeOutputTextSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].text.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].text.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].text[i]),sizeof(char));
                    }
                    //Output type
                    uint64_t nodeOutputTypeSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].type.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTypeSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].type.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].type[i]),sizeof(char));
                    }
                    //Output element
                    uint64_t nodeOutputElementSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].element.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputElementSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].element.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].element[i]),sizeof(char));
                    }
                    //Output texture
                    uint64_t nodeOutputTextureSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTextureName.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputTextureSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTextureName.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].selectedTextureName[i]),sizeof(char));
                    }
                    //Output Connections
                    uint64_t nodeOutputConnectionSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].connections.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputConnectionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].connections.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].connections[i]),sizeof(NodeConnection));
                    }
                    //Ramp node positions
                    uint64_t nodeOutputRampPositionSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPos.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPositionSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPos.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPos[i]),sizeof(float));
                    }
                    //Ramp node colors
                    uint64_t nodeOutputRampColorSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampClr.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampColorSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampClr.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampClr[i]),sizeof(glm::vec3));
                    }
                    //Ramp node pressing states
                    uint64_t nodeOutputRampPressedStateSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPress.size();
                    wf.write(reinterpret_cast<char*>(&nodeOutputRampPressedStateSize),sizeof(uint64_t));
                    for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPress.size(); i++)
                    {
                        wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].outputs[outI].rampPress[i]),sizeof(bool));
                    }
                    
                    //rangeBarsPointerPressedSize
                    uint64_t rangeBarsPointerPressedSize = nodeScenes[sceneI].nodes[nodeI].outputs[outI].rangeBarsPointerPressed.size();
                    wf.write(reinterpret_cast<char*>(&rangeBarsPointerPressedSize),sizeof(uint64_t));
                }

                //Listboxes
                uint64_t nodeListboxSize = nodeScenes[sceneI].nodes[nodeI].listBoxes.size();
                wf.write(reinterpret_cast<char*>(&nodeListboxSize),sizeof(uint64_t));
                for (size_t listI = 0; listI < nodeScenes[sceneI].nodes[nodeI].listBoxes.size(); listI++)
                {
                    wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].active),sizeof(bool));
                    wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].chosenIndex),sizeof(int));
                    
                    //Listbox elements
                    uint64_t nodeListboxElementSize = nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].elements.size();
                    wf.write(reinterpret_cast<char*>(&nodeListboxElementSize),sizeof(uint64_t));
                    for (size_t elementI = 0; elementI < nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].elements.size(); elementI++)
                    {
                        //List box element titles
                        uint64_t nodeListboxElementTextSize = nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].elements[elementI].size();
                        wf.write(reinterpret_cast<char*>(&nodeListboxElementTextSize),sizeof(uint64_t));
                        for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].elements[elementI].size(); i++)
                        {
                            wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].listBoxes[listI].elements[elementI][i]),sizeof(char));
                        }
                    }
                }

                //Node title size
                uint64_t nodeTitleSize = nodeScenes[sceneI].nodes[nodeI].title.size();
                wf.write(reinterpret_cast<char*>(&nodeTitleSize),sizeof(uint64_t));
                //Title
                for (size_t i = 0; i < nodeScenes[sceneI].nodes[nodeI].title.size(); i++)
                {
                    wf.write(reinterpret_cast<char*>(&nodeScenes[sceneI].nodes[nodeI].title[i]),sizeof(char));
                }
            }
        }   
    }
};