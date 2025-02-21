#include <stdio.h>
#include <stdlib.h>

int count=0;

void PrintHex(const unsigned char* buffer) {
    for (long i = 0; i < count; i++) {
        printf("%02X ", buffer[i]);
    }
    
}

int main(int argc, char* argv[]) {
    FILE* myfile = fopen(argv[1], "rb");
    while (fgetc(myfile)!=EOF){
      count++;
    }
    myfile = fopen(argv[1], "rb");
    unsigned char* buffer = (unsigned char*) malloc(count); 
    fread(buffer, 1, count, myfile);
    fclose(myfile);
    PrintHex(buffer);
    free(buffer);
    printf("\n");
    return 0;
}