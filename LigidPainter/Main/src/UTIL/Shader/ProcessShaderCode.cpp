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

//forward decleration for the util function
std::string readTheWord(std::string text, size_t &i);
void removeThePragma(std::string &code, size_t& i);


void Shader::processShaderCode(std::string &code){
    //Including other shader files using #pragma macro : 
    //#pragma LIGID_INCLUDE(exactPathToSourceFile.vert)

    bool inCommentLine = false;
    
    bool inWCommentLine = false;


    if(!code.size())
        return;

    try
    {
        for (size_t i = 0; i < code.size(); i++)
        {
            //Skip processing if only 3 char left
            if(i == code.size() - 1)
                break;
            if(i == code.size() - 2)
                break;
            if(i == code.size() - 3)
                break;


            // Process comment lines
            if (code[i] == '/' && code[i + 1] == '/' && !inWCommentLine)
            {
                inCommentLine = true;
                i += 2;
            }

            if (code[i] == '\n' || code[i] == '\0')
            {
                inCommentLine = false;
            }

            if (code[i] == '/' && code[i + 1] == '*' && !inCommentLine)
            {
                inWCommentLine = true;
                i += 2;
            }

            if (code[i] == '*' && code[i + 1] == '/')
            {
                inWCommentLine = false;
            }
            


            //Hash symbol detected
            if(code[i] == '#' && !inCommentLine && !inWCommentLine){
                i++;

                std::string word = readTheWord(code, i);

                //Pragma detected
                if(word == "pragma"){
                    i++;

                    std::string pragWord = readTheWord(code, i);

                    //LIGID_INCLUDE detected
                    if(pragWord == "LIGID_INCLUDE"){
                        //Skip to the (
                        while (code[i] != '(')
                        {
                            i++;

                            //Came to end of the code
                            //if(i == code.size())
                            //    return;

                            if(code[i] == '\n')
                                break;

                        }

                        //Skip the '(' char
                        i++;

                        
                        //The path to the included file
                        std::string path;

                        //Read the path between the '()'
                        while (code[i] != ')')
                        {
                            path += code[i];

                            i++;

                            //Came to end of the code
                            //if(i == code.size())
                            //    return;

                            // ')' didn't detected
                            if(code[i] == '\n')
                                break;
                        }

                        //Skip the ) char
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

                        removeThePragma(code, i);

                        //Insert a new line at the end of the macro
                        //code.insert(code.begin() + i - 1, '\n');

                        i += 1;

                        //Insert the code 
                        code.insert(i, incCode);
                        
                        //Skip the included code
                        i += incCode.size();
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

std::string readTheWord(std::string text, size_t &i){
    //! isalpha may cause problems

    //Skip the spaces
    while(text[i] == ' ')
        i++;

    std::string res;

    //Read until the end of the word
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

void removeThePragma(
                        std::string& code, 
                        size_t& i //Last index of the pragma
                    ){
    int lastP = i;

    //Detect the index range of the pragma
    int begP;
    for (begP = i; begP >= 0; begP--)
    {
        
        if(code[begP] == '#')
            break;
    
    }

    i = begP - 1; 

    code.erase(
                code.begin() + begP, //From
                code.begin() + lastP //To
            );
}