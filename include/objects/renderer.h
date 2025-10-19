#pragma once

#define GLCall(x) GLClearError();\
    x;\
    assert(GLLogCall())


void GLClearError();

bool GLLogCall();