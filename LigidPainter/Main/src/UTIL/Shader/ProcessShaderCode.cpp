/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "UTIL/Util.hpp"

std::string readTheWord(std::string text, size_t &i){
    //! isalpha may cause problems

    //Skip the spaces
    while(text[i] == ' ')
        i++;

    std::string res;

    while (isalpha(text[i]) || text[i] == '_' || text[i] == '-')
    {
        res += text[i];
        
        //At the end of the text
        if(i == text.size() - 1)
            break;
        
        i++;
    }
    
    return res;
}

void Shader::processShaderCode(std::string &code){
    //Including other shader files using #pragma macro : 
    //#pragma LIGID_INCLUDE(exactPathToSourceFile.vert)

    try
    {
        for (size_t i = 0; i < code.size(); i++)
        {
            //Hash symbol detected
            if(code[i] == '#'){
                i++;

                std::string word = readTheWord(code, i);

                //Pragma detected
                if(word == "pragma"){
                    i++;

                    std::string pragWord = readTheWord(code, i);

                    //LIGID_INCLUDE detected
                    if(pragWord == "LIGID_INCLUDE"){
                        
                        while (code[i] != '(')
                        {
                            i++;

                            //Came to end of the code
                            if(i == code.size())
                                break;

                            if(code[i] == '\n')
                                break;

                        }

                        i++;

                        //The path to the included file
                        std::string path;

                        while (code[i] != ')')
                        {
                            path += code[i];

                            i++;

                            //Came to end of the code
                            if(i == code.size())
                                break;

                            // ')' didn't detected
                            if(code[i] == '\n')
                                break;
                        }

                        i++;





                        std::ifstream shaderFile;
                        shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

                        //Included code
                        std::string incCode;

                        if(path.size())
                        {
                            shaderFile.open(path.c_str());
                            std::stringstream gShaderStream;
                            gShaderStream << shaderFile.rdbuf();
                            shaderFile.close();
                            incCode = gShaderStream.str();
                        }

                        i++;

                        //Insert a new line at the end of the macro
                        code.insert(code.begin() + i - 1, '\n');

                        //Insert the code 
                        code.insert(i, incCode);
                    }
                }
            }
        }
    }
    
    catch(const std::exception& e)
    {
        std::cout << "ERROR! While processing the shader code : " << code << std::endl;
    }
    
}