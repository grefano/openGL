#include <vector>
#include "vbElement.h"
#include <glad/glad.h>
#include "layout.h"
#include <iostream>
void Layout::push(unsigned int type, unsigned int count){
    elements.push_back({type, count, GL_FALSE});
    stride += count*sizeof(float);

    std::cout << count << "->" << stride << std::endl;
    // stride = 8*sizeof(float);
}
std::vector<vbElement> Layout::getElements(){
    return elements;
}
unsigned int Layout::getStride(){
    return stride;
}