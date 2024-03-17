/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

static void parseFBXMeshData(
                        std::vector<glm::vec3>& positions,
                        const std::vector<glm::vec2>& UVs,
                        const std::vector<glm::vec3>& normals,
                        const std::vector<int>& polygonVertexIndices,
                        const std::vector<int>& uvIndices,
                        const std::vector<int>& edges,
                        const std::vector<std::string>& matTitles,
                        const std::vector<int>& materials,
                        std::vector<std::vector<Vertex>>& meshVertices,
                        std::vector<std::vector<unsigned int>>& meshIndices,
                        FBXTransform transform,
                        long meshID,
                        std::vector<long>& materialIDS,
                        std::vector<FbxConnection> connections
                    )
{


    // TODO Seperate unique vertices for each mesh

    //Contains the unique vertex values generated
    std::vector<Vertex> uniqueVertices;

    // polygonVertexIndices index, uniqueVertices array index 
    std::vector<std::map<std::pair<int,int>, int>> posData;

    // Give an error if the polygonVertexIndices array & edges array are the same
    /**/
    
    if (polygonVertexIndices.size() != edges.size())
    {
        LGDLOG::start<< "ERROR: Reading FBX file. Can't parse mesh data. Sizes of the polygonVertexIndices & the edges are not the same." << LGDLOG::end;
        return;
    }

    //std::cout << "TRANSFORM : " << glm::to_string(transform.rotation) << " " << glm::to_string(transform.scale) << " " << glm::to_string(transform.translation) << std::endl;

    if(transform.translation == glm::vec3(FLT_MAX))
        transform.translation = glm::vec3(0.f);
    if(transform.rotation == glm::vec3(FLT_MAX))
        transform.rotation = glm::vec3(0.f);
    if(transform.scale == glm::vec3(FLT_MAX))
        transform.scale = glm::vec3(1.f);

    glm::vec3 copyTrans = transform.translation; //TODO X
    transform.translation.z = copyTrans.y;
    transform.translation.y = copyTrans.z;

    for (size_t i = 0; i < positions.size(); i++)
    {
        // Create a transformation matrix for translation and rotation
        glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
        modelMatrix = glm::translate(modelMatrix, transform.translation);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); 
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 1.0f, 0.0f)); // Y & Z are switched
        modelMatrix = glm::rotate(modelMatrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, transform.scale);

        // Apply the transformation matrix to the vertex
        glm::vec4 transformedVertex = modelMatrix * glm::vec4(positions[i], 1.0f);

        // Store the transformed vertex
        positions[i] = glm::vec3(transformedVertex);
    }

    for (size_t i = 0; i < matTitles.size(); i++)
    {
        posData.push_back({});
        meshVertices.push_back({});
        meshIndices.push_back({});
    }
    // Mapping the indices
    int faceCounter = 0;
    for (size_t i = 0; i < polygonVertexIndices.size(); i++)
    {
        /*
            uvIndices array contains the necessary indices
            for matching the texture coordinates & the vertex
            position data without repeating

            goes like : 0, 1, 2, 3, 5, 8, 9
        */
        // int uvIndex = uvIndices[i];

        // Get the index of the position
        int posIndex = polygonVertexIndices[i];
        if (posIndex < 0){
            posIndex = abs(posIndex) - 1;
        }

        // Get the index of the texture coordinate
        int edgeIndex = edges[i];
        if (edgeIndex  < 0)
            edgeIndex  = abs(edgeIndex) - 1;

        Vertex uniqueVert;
        
        if(posIndex >= positions.size()){
            LGDLOG::start<< "ERROR : Reading fbx file. Position index is not valid." << LGDLOG::end;
            return;
        }
        uniqueVert.Position = positions[posIndex];

        if(edgeIndex >= UVs.size()){
            LGDLOG::start<< "ERROR : Reading fbx file. Texture coordinate index is not valid." << LGDLOG::end;
            return;
        }
        uniqueVert.TexCoords = UVs[edgeIndex];
        
        if(i >= normals.size()){
            LGDLOG::start<< "ERROR : Reading fbx file. Normal vector index is not valid." << LGDLOG::end;
            return;
        }
        uniqueVert.Normal = normals[i];

        int materialI;
        if(materials.size() && materials.size() != 1 && faceCounter < materials.size()) //ByPolygon
            materialI = materials[faceCounter];
        else{ //AllSame
            materialI = 0;
            //std::cout << "A" << std::endl; 
            for (size_t conI = 0; conI < connections.size(); conI++)
            {
                if(connections[conI].startID == meshID){
                    long materialFBXID = 0;
                    for (size_t conII = 0; conII < connections.size(); conII++){
                        if(connections[conI].destionationID == connections[conII].destionationID){
                            materialFBXID = connections[conII].startID;
                        }                        
                    }

                    for (size_t i = 0; i < materialIDS.size(); i++)
                    {
                        if(materialFBXID == materialIDS[i])
                            materialI = i;  
                    }
                    
                }
            }
            //std::cout << "materialI " << materialI << std::endl; 
        }

        posData[materialI][std::make_pair(polygonVertexIndices[i], edges[i])] = meshVertices[materialI].size();
        meshVertices[materialI].push_back(uniqueVert);

        if(polygonVertexIndices[i] < 0)
            faceCounter++;
    }
    

    int faceI = 0;
    faceCounter = 0;
    for (size_t i = 0; i < polygonVertexIndices.size(); i++)
    {
        
        faceI++;
        
        /* Triangulation */
        if (polygonVertexIndices[i] < 0){
            int faceCount = faceI - 2;

            if(!LIGID_FBX_IMPORTER_TRIANGULATE)
                faceCount = 1;

            if(faceCount < 0)
                faceCount = 0;

            int vStartI = i - faceI + 1;
            for (size_t fI = 0; fI < faceCount; fI++)
            {
                glm::ivec3 facePos;
                glm::ivec3 faceEdge;
                
                if(vStartI + 2 + fI >= edges.size()){
                    break;
                }

                facePos.x = polygonVertexIndices[vStartI];
                facePos.y = polygonVertexIndices[vStartI + 1 + fI];
                facePos.z = polygonVertexIndices[vStartI + 2 + fI];
                
                faceEdge.x = edges[vStartI];
                faceEdge.y = edges[vStartI + 1 + fI];
                faceEdge.z = edges[vStartI + 2 + fI];

                int materialI;
                if(materials.size() && materials.size() != 1)
                    materialI = materials[faceCounter];
                else{
                    materialI = 0;
                    for (size_t conI = 0; conI < connections.size(); conI++)
                    {
                        if(connections[conI].startID == meshID){
                            long materialFBXID = 0;
                            for (size_t conII = 0; conII < connections.size(); conII++){
                                if(connections[conI].destionationID == connections[conII].destionationID){
                                    materialFBXID = connections[conII].startID;
                                }                        
                            }

                            for (size_t i = 0; i < materialIDS.size(); i++)
                            {
                                if(materialFBXID == materialIDS[i])
                                    materialI = i;  
                            }
                        }
                    }
                }

                meshIndices[materialI].push_back(posData[materialI][std::make_pair(facePos.x, faceEdge.x)]);
                meshIndices[materialI].push_back(posData[materialI][std::make_pair(facePos.y, faceEdge.y)]);
                meshIndices[materialI].push_back(posData[materialI][std::make_pair(facePos.z, faceEdge.z)]);

                /* Tangent calculations */
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Tangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Bitangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Tangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Bitangent = glm::vec3(0); 
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Tangent = glm::vec3(0);
                //meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Bitangent = glm::vec3(0);
                calculateTangentBitangent(meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]], meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]], meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]]);
                
                /* Normalize tangent values */
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Tangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Tangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Bitangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.x, faceEdge.x)]].Bitangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Tangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Tangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Bitangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.y, faceEdge.y)]].Bitangent); 
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Tangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Tangent);
                meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Bitangent = glm::normalize(meshVertices[materialI][posData[materialI][std::make_pair(facePos.z, faceEdge.z)]].Bitangent);
            }
            
            faceI = 0;
            faceCounter++;
        }
    }
}