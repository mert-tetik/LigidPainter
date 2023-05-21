#ifndef LGDTEXTRENDERER_HPP
#define LGDTEXTRENDERER_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    glm::vec3 renderText(Shader shader,std::string text,float x,float y,float z,float maxX,bool multipleLines,float scale,float mostLeft){
		glm::vec3 data;
		//.x = Returns the text's starting position int x axis
		//.y = Returns the text's ending position int x axis
		//.z = hitTheBoundaries


		//Text is rendered using button shader
		//Shader is the shader class of the button shader
		//X Y Z : Is the location of the text, z = depth value
		//MaxX is the location value in the x axis where you want text to end or get to new line (buttons right side's x coordinate)
		//If mutliple line is set to true, multiple lines will be enabled
		//Scale : scale of the text. 1 is default and pretty big. 
		//Most left is the location value in the x axis where text can't go further left of that value (buttons left side's x coordinate)

		float lastXText = x;
        
        glActiveTexture(GL_TEXTURE0);
    
	    std::string::const_iterator c;
        
        shader.setInt("txtr",0);
	    shader.setInt("renderText",1);

		//Get the location of the text's last char
	    float overallX = 0.f;
		for (std::string::const_iterator aC = text.begin(); aC != text.end(); aC++){
	    	character ch = font.characters[*aC];//Get the current char
            
			//To the right
	    	overallX += (ch.Advance >> 6) * scale / 1.2f;
		}
		
		x-=overallX/2.f; //Allign the text in the middle

		//Check if the text's left side is out of the boundaries
	    float overallX2 = 0.f;
		bool hitTheBoundaires = false;
		for (std::string::const_iterator aC = text.begin(); aC != text.end(); aC++){
	    	character ch = font.characters[*aC];//Get the current char
            
			if(maxX < x + overallX2) {
				hitTheBoundaires = true;
				break;
			}
			
			//To the right
	    	overallX2 += (ch.Advance >> 6) * scale / 1.2f;
		}

		if(hitTheBoundaires) //If the text's left side is out of the boundaries align the text on the left side of the button
			x = mostLeft;

		data.x = x;

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
	    		float ypos = y - (-ch.Bearing.y + ch.Size.y);

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

		data.y = x;
		data.z = hitTheBoundaires;

		return data;
	}
};

#endif