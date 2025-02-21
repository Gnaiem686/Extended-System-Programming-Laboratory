#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main() {
    int read_write[2];
    char buff[64];
    if (pipe(read_write)== -1) {
        perror("wrong in pipe");
        exit(1);
    }
    pid_t P_id=fork();
    if (P_id ==-1) {
        perror("wrong in fork");
        exit(1);
    }
    if (P_id > 0) {//parent
        close(read_write[1]);//close the write
        //just read
        read(read_write[0], buff, 64);
        printf("message from child: %s\n", buff);
        
        exit(1);
    } else {
        char msg[64] = "hello from child";
        close(read_write[0]);//close read
        //just write to parent
        write(read_write[1], msg, strlen(msg) + 1);
        
        exit(1);
    }
    return 0;
}