#include <iostream>

#include "src/Libav.cpp"

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("usage ./main {file path}\n");
        return 1;
    }

    int ret = 0;
    const char *filePath = argv[1];
    Libav *lAv = new Libav(filePath);

    ret = lAv->openInput();
    ret = lAv->openOutput();

    if(ret < 0){
        fprintf(stderr, "Error opening the output file");
        return 0;
    }

    lAv->remux();

    delete lAv;
    return 0;
}