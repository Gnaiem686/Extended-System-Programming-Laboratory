#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv){
if(argc < 2){
    printf("few number of args have been added\n");
    exit(0);
}
if(argc > 2){
    printf("Im taking the just first arg\n");
}

printf("The number of digits is: %d\n", digit_counter(argv[1]));
}

int digit_counter(char* arg){
    int count = 0;
    for(int i = 0; arg[i] != '\0'; i++){
        if(arg[i] <= '9'&& arg[i]>='0'){
            count++;
        }
    }
    return count;
} 