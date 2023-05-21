#ifndef LGDUTIL_HPP
#define LGDUTIL_HPP


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Util
{
private:
    /* data */
public:
    Util(/* args */){}

    float getPercent(float value, float percent){
        return value / 100.f * percent;
    }
    glm::vec2 getPercent(glm::vec2 value, glm::vec2 percent){
        return glm::vec2(value.x / 100.f * percent.x,value.y / 100.f * percent.y);
    }
    glm::vec3 getPercent(glm::vec3 value, glm::vec3 percent){
        return glm::vec3(value.x / 100.f * percent.x,value.y / 100.f * percent.y,value.z / 100.f * percent.z);
    }

    string getLastWordBySeparatingWithChar(string s, char del)
    {
    	std::vector<std::string> words;
    	stringstream ss(s);
    	string word;
    	while (!ss.eof()) {
    		getline(ss, word, del);
    		words.push_back(word);
    	}
    	return words[words.size() - 1];
    }
};

#endif