#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <iostream>
#include <string.h>

void gray_scale(unsigned char* img){
    for (size_t i= 0; i < strlen((const char*)img); i=i+4){
        char avg= (img[i] + img[i+1] + img[i+2]) /3 ;
        img[i]= avg ;
        img[i+1]= avg ;
        img[i+2]= avg ;
    }
}

void gray_scale2(unsigned char* img){
    for (size_t i= 0; i < strlen((const char*)img); i=i+4){
        char avg= (img[i]*0.587 + img[i+1]*0.114 + img[i+2]*0.2989) ;
        img[i]= avg ;
        img[i+1]= avg ;
        img[i+2]= avg ;
    }
}

unsigned char* GrayScale_Rep(unsigned char * origin_img){
    unsigned char * buffer = (unsigned char*) malloc(strlen((const char*)origin_img) / 4);
    int j= 0 ;
    for(size_t i = 0; i < strlen((const char*)origin_img) ; i+=4 ){
        buffer[j]= origin_img[i] ;
        j++ ;
    }
    return buffer;
}


int main(void)
{
    std::string filepath = "res/textures/Lenna.png";
    int width, height, comps;
    int req_comps = 4;
    unsigned char * buffer = stbi_load(filepath.c_str(), &width, &height, &comps, req_comps);//comps=4
    gray_scale(buffer);
    //gray_scale2(buffer);
    unsigned char * new_buffer= GrayScale_Rep(buffer);
    int result = stbi_write_png("res/textures/GrayScale_Lenna.png", width, height, 1, new_buffer, width * 1 ); //changed comps
    std::cout << result << std::endl;
    return 0;
}