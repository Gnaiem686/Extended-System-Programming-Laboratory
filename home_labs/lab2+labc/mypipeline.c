
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main() {
    
    int read_write[2];
   // char buff[64];
    if (pipe(read_write)== -1) {
        perror("wrong in pipe\n");
        exit(1);
    }
    fprintf(stderr,"parent_process>forking…\n");
    pid_t child_1=fork();
    fprintf(stderr,"parent_process>created process with id: %d\n",child_1);
    if (child_1 ==-1) {
        perror("wrong in creating child_1\n");
        exit(1);
    }
    else if(child_1==0){
        fprintf(stderr,"child1>redirecting stdout to the write end of the pipe…\n");
        close(STDOUT_FILENO);
        dup(read_write[1]);
        close(read_write[1]);
        fprintf(stderr,"child1>going to execute cmd: …\n");
        if (execlp("ls", "ls", "-l", NULL)==-1) {//creats new prosses
            perror("wrong in  child_1 execlp creating new prosses\n");
            exit(1);
        }
    }
    fprintf(stderr,"parent_process>closing the write end of the pipe…\n");
    //closing the parent write-end
    close(read_write[1]);


    pid_t child_2=fork();
    if (child_2 ==-1) {
        perror("wrong in creating child_2\n");
        exit(1);
    }
    else if (child_2==0) {
        fprintf(stderr,"child2>redirecting stdout to the write end of the pipe…\n");
        close(STDIN_FILENO);
        dup(read_write[0]);
        close(read_write[0]);
        fprintf(stderr,"child2>going to execute cmd: …\n");
        if (execlp("tail", "tail", "-n", "2", NULL)==-1) {//creats new prosses
            perror("wrong in  child_2 execlp creating new prosses\n");
            exit(1);
        }
    }
    fprintf(stderr,"parent_process>closing the read end of the pipe…\n");
    //closing the parent read-end
    close(read_write[0]);
    fprintf(stderr,"parent_process>waiting for child processes to terminate…\n");
    waitpid(child_1,NULL,0);
    waitpid(child_2,NULL,0);
    fprintf(stderr,"parent_process>exiting…\n");
    exit(0);
}