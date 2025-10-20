#pragma once
#include <assert.h>

#define GLCall(x) GLClearError();\
    x;\
    assert(GLLogCall())


void GLClearError();

bool GLLogCall();