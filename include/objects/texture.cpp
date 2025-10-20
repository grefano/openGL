#include <iostream>
#include <texture.h>
#include <stb/stb_image.h>

Texture::Texture(const char* filename){
    int wImg, hImg, numColCh;
    unsigned char* bytes = stbi_load(filename, &wImg, &hImg, &numColCh, 0);
    
    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wImg, hImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

}

Texture::~Texture(){
    std::cout << "deleted texture " << std::endl;
    glDeleteTextures(1, &ID);
}