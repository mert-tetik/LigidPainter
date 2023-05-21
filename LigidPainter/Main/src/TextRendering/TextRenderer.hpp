#ifndef LGDTEXTRENDERER_HPP
#define LGDTEXTRENDERER_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Font.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "freetype/freetype.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class TextRenderer
{
private:
    /* data */
public:
    Font font;
    
    TextRenderer(/* args */){}
    TextRenderer(Font font){
        this->font = font;
    }

    void renderText(Shader shader,std::string text,float x,float y,float z,float maxX,bool multipleLines,float scale){
	    float lastXText = x;
        
        glActiveTexture(GL_TEXTURE0);
    
	    std::string::const_iterator c;
        
        shader.setInt("txtr",0);
	    shader.setInt("renderText",1);

        //Render all the chars in the text parameter
	    int counter = 0;
	    for (c = text.begin(); c != text.end(); c++)
	    {
	    	character ch = font.characters[*c];//Get the current char

	    	if(*c == '\n'){//New line if the char is \n
                if(!multipleLines)//Break the for loop & stop rendering the text if multiple lines are not allowed 
                    break;

	    		x=lastXText; //Go to the start
	    		y-=0.03f; //New line
	    	}
	    	else{
	    		if(maxX < x){ //Go to new line if the text is out of it's boundaries
	    			x=lastXText;
	    			y-=(ch.Size.y) * scale * 1.3f;
	    			if(!multipleLines) //Break the for loop & stop rendering the text if multiple lines are not allowed 
	    				break;
	    		}

                //Calculate the position of the char
	    		float xpos = x + ch.Bearing.x * scale;
	    		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

                //Calculate the size of the char
	    		float w = ch.Size.x * scale * 0.8f;
	    		float h = ch.Size.y * scale;

                //Set the transform values
	    		shader.setVec2("scale",glm::vec2(w/1.7,h/1.7));
	    		shader.setVec3("pos",glm::vec3(xpos + w/1.7,ypos + h/1.7,z));

                //Draw the char
	    		glBindTexture(GL_TEXTURE_2D,ch.TextureID);
	    		glDrawArrays(GL_TRIANGLES, 0, 6);

                //To the right
	    		x += (ch.Advance >> 6) * scale / 1.2f;

	    		counter++;
	    	}
	    }
	    shader.setInt("renderText",0);
    }
};

#endif