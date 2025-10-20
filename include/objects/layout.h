#pragma once
#include <vector>
#include <vbElement.h>

class Layout{
    private:
    std::vector<vbElement> elements;
    unsigned int stride = 0;
    public:
    void push(unsigned int type, unsigned int count);
    std::vector<vbElement> getElements();
    unsigned int getStride();
};