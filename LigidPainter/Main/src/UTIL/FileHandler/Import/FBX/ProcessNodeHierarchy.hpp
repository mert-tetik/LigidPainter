/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Manage websites & open a website with default browser

*/

void ProcessNodeHierarchy( 
                            std::vector<FbxNode>& nodes, 
                            std::vector<std::vector<glm::vec3>>& vertPositions, 
                            std::vector<std::vector<glm::vec2>>& vertUVs, 
                            std::vector<std::vector<glm::vec3>>& vertNormals, 
                            std::vector<std::vector<int>>& polygonVertexIndices, 
                            std::vector<std::vector<int>>& edges, 
                            std::vector<std::vector<int>>& uvIndices,
                            std::vector<std::string>& matTitles,
                            std::vector<std::vector<int>>& materials,
                            std::vector<FBXTransform>& transforms
                        ) 
{
    for ( auto& node : nodes) {
        //LGDLOG::start<< node.nodeType<< LGDLOG::end;

        if (true) {
            
            // Process vertex properties
            for (auto& prop : node.properties) {
                
                if(node.nodeType == "Vertices" && processingMesh){
                    if (prop.typeCode == 'd') {
                        size_t arrayLength = prop.data.size() / sizeof(double);
                        std::vector<double> doubleArray(arrayLength);

                        std::memcpy(doubleArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < doubleArray.size()/3; i++)
                        {
                            glm::vec3 vertPos;
                            if(i * 3 >= doubleArray.size())
                                break;
                            vertPos.x = doubleArray[i * 3];
                            if(i * 3 + 1 >= doubleArray.size())
                                break;
                            vertPos.y = doubleArray[i * 3 + 1];
                            if(i * 3 + 2 >= doubleArray.size())
                                break;
                            vertPos.z = doubleArray[i * 3 + 2];

                            vertPositions[objectI].push_back(vertPos);                            
                        }
                    }
                    else if(prop.typeCode == 'f'){
                        size_t arrayLength = prop.data.size() / sizeof(float);
                        std::vector<float> floatArray(arrayLength);

                        std::memcpy(floatArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < floatArray.size()/3; i++)
                        {
                            glm::vec3 vertPos;
                            if(i * 3 >= floatArray.size())
                                break;
                            vertPos.x = floatArray[i * 3];
                            if(i * 3 + 1 >= floatArray.size())
                                break;
                            vertPos.y = floatArray[i * 3 + 1];
                            if(i * 3 + 2 >= floatArray.size())
                                break;
                            vertPos.z = floatArray[i * 3 + 2];

                            vertPositions[objectI].push_back(vertPos);                            
                        }
                    }
                    else{
                        LGDLOG::start<< "WARNING! Variable type for the vertices array is not valid : " << prop.typeCode << LGDLOG::end;
                    }
                }
                
                if(node.nodeType == "Normals" && processingMesh){
                    if (prop.typeCode == 'd') {
                        size_t arrayLength = prop.data.size() / sizeof(double);
                        std::vector<double> doubleArray(arrayLength);

                        std::memcpy(doubleArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < doubleArray.size()/3; i++)
                        {
                            glm::vec3 vertNormal;
                            if(i * 3 >= doubleArray.size())
                                break;
                            vertNormal.x = doubleArray[i * 3];
                            if(i * 3 + 1 >= doubleArray.size())
                                break;
                            vertNormal.y = doubleArray[i * 3 + 1];
                            if(i * 3 + 2 >= doubleArray.size())
                                break;
                            vertNormal.z = doubleArray[i * 3 + 2];

                            vertNormals[objectI].push_back(vertNormal);                            
                        }
                    }
                    else if(prop.typeCode == 'f'){
                        size_t arrayLength = prop.data.size() / sizeof(float);
                        std::vector<float> floatArray(arrayLength);

                        std::memcpy(floatArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < floatArray.size()/3; i++)
                        {
                            glm::vec3 vertNormal;
                            if(i * 3 >= floatArray.size())
                                break;
                            vertNormal.x = floatArray[i * 3];
                            if(i * 3 + 1 >= floatArray.size())
                                break;
                            vertNormal.y = floatArray[i * 3 + 1];
                            if(i * 3 + 2 >= floatArray.size())
                                break;
                            vertNormal.z = floatArray[i * 3 + 2];

                            vertNormals[objectI].push_back(vertNormal);                            
                        }
                    }
                    else{
                        LGDLOG::start<< "WARNING! Variable type for the normals array is not valid : " << prop.typeCode << LGDLOG::end;
                    }
                }

                if(node.nodeType == "UV" && processingMesh){
                    if (prop.typeCode == 'd') {
                        size_t arrayLength = prop.data.size() / sizeof(double);
                        std::vector<double> doubleArray(arrayLength);

                        std::memcpy(doubleArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < doubleArray.size()/2; i++)
                        {
                            glm::vec3 vertUV;
                            
                            if(i * 2 >= doubleArray.size())
                                break;
                            vertUV.x = doubleArray[i * 2];
                            
                            if(i * 2 + 1 >= doubleArray.size())
                                break;
                            vertUV.y = doubleArray[i * 2 + 1];

                            vertUVs[objectI].push_back(vertUV);                            
                        }
                    }
                    else if (prop.typeCode == 'f') {
                        size_t arrayLength = prop.data.size() / sizeof(float);
                        std::vector<float> floatArray(arrayLength);

                        std::memcpy(floatArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < floatArray.size()/2; i++)
                        {
                            glm::vec3 vertUV;
                            
                            if(i * 2 >= floatArray.size())
                                break;
                            vertUV.x = floatArray[i * 2];
                            
                            if(i * 2 + 1 >= floatArray.size())
                                break;
                            vertUV.y = floatArray[i * 2 + 1];

                            vertUVs[objectI].push_back(vertUV);                            
                        }
                    }
                    else{
                        LGDLOG::start<< "WARNING! Variable type for the texture coordinate array is not valid : " << prop.typeCode << LGDLOG::end;
                    }
                }

                if(node.nodeType == "PolygonVertexIndex" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            polygonVertexIndices[objectI].push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "Edges" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            edges[objectI].push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "UVIndex" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());

                        for (size_t i = 0; i < intArray.size(); i++)
                        {
                            uvIndices[objectI].push_back(intArray[i]);                            
                        }
                    }
                }

                if(node.nodeType == "Material"){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());

                        matTitles.push_back(infoStr);
                    }
                }
                
                if(node.nodeType == "Materials" && processingMesh){
                    if (prop.typeCode == 'i') {
                        size_t arrayLength = prop.data.size() / sizeof(int);
                        std::vector<int> intArray(arrayLength);

                        std::memcpy(intArray.data(), prop.data.data(), prop.data.size());
                        materials[objectI] = intArray;                            
                    }
                }

                if(node.nodeType == "P" && processingMesh){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());

                        if(infoStr.size() > 3){
                            if(modelI < transforms.size())
                                if(infoStr[0] == 'L' && infoStr[1] == 'c' && infoStr[2] == 'l')
                                    transforms[modelI].currentReadMode = UTIL::getLastWordBySeparatingWithChar(infoStr, ' ');
                        }
                    }
                    else if (prop.typeCode == 'D') {
                        double doubleValue = prop.singleDoubleVal;
                        
                        //doubleValue /= 100.;

                        if(modelI < transforms.size()){
                            if(transforms[modelI].currentReadMode == "Translation"){
                                if(transforms[modelI].translation.x == FLT_MAX){
                                    transforms[modelI].translation.x = doubleValue/100.;
                                }
                                else if(transforms[modelI].translation.y == FLT_MAX){
                                    transforms[modelI].translation.y = doubleValue/100.;
                                }
                                else if(transforms[modelI].translation.z == FLT_MAX){
                                    transforms[modelI].translation.z = doubleValue/100.;
                                }
                            }
                            else if(transforms[modelI].currentReadMode == "Rotation"){
                                if(transforms[modelI].rotation.x == FLT_MAX){
                                    transforms[modelI].rotation.x = doubleValue;
                                }
                                else if(transforms[modelI].rotation.y == FLT_MAX){
                                    transforms[modelI].rotation.y = doubleValue;
                                }
                                else if(transforms[modelI].rotation.z == FLT_MAX){
                                    transforms[modelI].rotation.z = doubleValue;
                                }
                            }
                            else if(transforms[modelI].currentReadMode == "Scaling"){
                                if(transforms[modelI].scale.x == FLT_MAX){
                                    transforms[modelI].scale.x = doubleValue / 100.;
                                }
                                else if(transforms[modelI].scale.y == FLT_MAX){
                                    transforms[modelI].scale.y = doubleValue / 100.;
                                }
                                else if(transforms[modelI].scale.z == FLT_MAX){
                                    transforms[modelI].scale.z = doubleValue / 100.;
                                }
                            }
                        }
                        else{
                            LGDLOG::start << "WARNING : Reading FBX file : Transform properties : Invalid model index" << LGDLOG::end;
                        }
                    }
                }

                if(node.nodeType == "MappingInformationType" && processingMesh){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr != "ByPolygonVertex" && infoStr != "ByPolygon" && infoStr != "AllSame"){
                            LGDLOG::start<< "WARNING : Mapping information type is : " << infoStr << "! Results might be unexpected."  << LGDLOG::end;
                        }
                    }
                }

                if(node.nodeType == "ObjectType" && processingMesh){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr != "Geometry"){
                            LGDLOG::start<< "WARNING : Object type is  : " << infoStr << "! Results might be unexpected."  << LGDLOG::end;
                        }
                    }
                }

                if(node.nodeType == "Model"){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr == "Mesh"){
                            transforms.push_back({});
                            modelI++;
                        }
                    }
                }
                
                if(node.nodeType == "Geometry"){
                    if (prop.typeCode == 'S') {
                        std::string infoStr(prop.data.begin(), prop.data.end());
                        if(infoStr == "Mesh"){
                            objectI++;   
                            vertPositions.push_back({}); 
                            vertUVs.push_back({}); 
                            vertNormals.push_back({}); 
                            polygonVertexIndices.push_back({}); 
                            edges.push_back({}); 
                            uvIndices.push_back({});
                            materials.push_back({});
                            processingMesh = true;
                            processingShape = false;
                        }
                        else if(infoStr == "Shape"){
                            processingMesh = false;
                            processingShape = true;
                        }
                    }
                }
            }
        }

        // Recursively process nested nodes
        ProcessNodeHierarchy(node.nestedNodes, vertPositions, vertUVs, vertNormals, polygonVertexIndices, edges, uvIndices, matTitles, materials, transforms);
    }
}