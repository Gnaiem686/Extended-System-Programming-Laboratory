#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include "LineParser.h"
#include "linux/limits.h"
#include <sys/wait.h>
#include "fcntl.h"

# define HISTLEN 20
#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;
process* process_list = NULL;
void freeProcessList(process* process_list){
    process* current_process = process_list;
    while (current_process != NULL) {
        process* temp_proses = current_process;
        current_process = current_process->next;
        freeCmdLines(temp_proses->cmd);
        free(temp_proses);
    }
}
void updateProcessStatus(process* process_list, int pid, int status){
    process *temp_proc= process_list;
    while (temp_proc) {
        if (temp_proc->pid==pid) {
            temp_proc->status=status;
            return;
        }
        temp_proc=temp_proc->next;
    }
    printf("Process with pid %d doesnt exsist\n", pid);
}
void updateProcessList(process **process_list) {
    process *proces=*process_list;
    pid_t pid;
    while (proces != NULL) {
        pid = waitpid(proces->pid, NULL, WNOHANG);
        if (pid == -1) {
            printf("Waitingpid fail\n");
            exit(1);
        }
        else if (pid == proces->pid) {
            proces->status = TERMINATED;
            printf("Process %d has terminated\n", proces->pid);
        }
        proces = proces->next;
    }
}

void addProcess(process** process_list, cmdLine* pcmd, pid_t pid){
    process* newest_process = malloc(sizeof(process));
    newest_process->cmd = pcmd;
    newest_process->pid = pid;
    newest_process->status = RUNNING;
    if (*process_list==NULL) {
        *process_list=newest_process;
    }
    else{
    newest_process->next = *process_list;
    *process_list = newest_process;
    }
}
//helper to delete the terminated one after printing it
void deleteProcses(process** process_list,process* term_process){
    process* temp_list=*process_list;
    process *connected_process = NULL;
        while (temp_list!=NULL) {
            if (temp_list->pid==term_process->pid) {
                if (connected_process==NULL) {
                    *process_list=temp_list->next;
                }
                else {
                    connected_process->next = temp_list->next;
                }
                free(temp_list);
                return;
            }
            connected_process = temp_list;
            temp_list = temp_list->next;
        }
}

void printProcessList(process** process_list){
    updateProcessList(process_list);
    process* temp_process_list = *process_list;
    printf("PID\tCommand\t\tSTATUS\n");
    while(temp_process_list != NULL){
        printf("%d\t%s\t", temp_process_list->pid, temp_process_list->cmd->arguments[0]);
        switch (temp_process_list->status) {
            case RUNNING:
                printf("Running\t");
                break;
            case SUSPENDED:
                printf("Suspend\t");
                break;
            case TERMINATED:
                printf("Terminated\t");
                process* term_process = temp_process_list;
                deleteProcses(process_list,term_process);
                continue;
        }
        temp_process_list = temp_process_list->next;
    }
}
process* ProcessNodeCreate(cmdLine* pcmdline, pid_t pid, int status) {
    process* link = (process*)malloc(sizeof(process));
    link->cmd = pcmdline;
    link->pid = pid;
    link->status = status;
    link->next = NULL;
    return link;
}

void get_history_queue(int n, int *newest,int *oldest,char* history_queue[]) {
    if (n < 1 || n > (*newest) - (*oldest) + 1) {
        perror("Error: invalid index\n");
        exit(1);
        
    }
     for (int i = (*oldest); i <= n; i++) {
        printf("%d: %s\n", i - (*oldest) + 1, history_queue[i % HISTLEN]);
    }   
}
void Print_History_queue(char* history_queue[],int *newest,int *oldest){
    for (int i = (*oldest); i <= (*newest); i++) {
        printf("%d: %s\n", i - (*oldest) + 1, history_queue[i % HISTLEN]);
}
}
void Add_To_History(char* history_queue[], cmdLine *pCmdLine, int *newest,int *oldest){
    char* copy_space = (char*) malloc(strlen(pCmdLine->arguments[0]) + 1);
    strcpy(copy_space, pCmdLine->arguments[0]);
    if (newest - oldest + 1 > HISTLEN) {
        free(history_queue[*oldest]);
        (*oldest)++;
    }
    *newest = (*newest + 1) % HISTLEN;
    history_queue[*newest] = copy_space;
}

 void mypipeline(cmdLine *pCmdLine,cmdLine *NextpCmdLine) {
    int read_write[2];
   // char buff[64];
//    pCmdLine->arguments[0];
//    pCmdLine->next->arguments[0];
    if (pipe(read_write)== -1) {
        perror("wrong in pipe\n");
        exit(1);
    }
    pid_t child_1=fork();
    if (child_1 ==-1) {
        perror("wrong in creating child_1\n");
        exit(1);
    }
    else if(child_1==0){
        addProcess(&process_list, pCmdLine, child_1);
        close(STDOUT_FILENO);
        dup(read_write[1]);
        close(read_write[1]);
        if (execlp(pCmdLine->arguments[0],pCmdLine->arguments[0], NULL)==-1) {//creats new prosses
            perror("wrong in  child_1 execlp creating new prosses\n");
            exit(1);
        }
    }
    //closing the parent write-end
    close(read_write[1]);


    pid_t child_2=fork();
    if (child_2 ==-1) {
        perror("wrong in creating child_2\n");
        exit(1);
    }
    else if (child_2==0) {
        addProcess(&process_list, pCmdLine, child_2);
        close(STDIN_FILENO);
        dup(read_write[0]);
        close(read_write[0]);
        if (execlp(NextpCmdLine->arguments[0],NextpCmdLine->arguments[0],NextpCmdLine->arguments[1] ,NULL)==-1) {//creats new prosses
            perror("wrong in  child_2 execlp creating new prosses\n");
            exit(1);
        }
    }
    //closing the parent read-end
    close(read_write[0]);
    waitpid(child_1,NULL,0);
    waitpid(child_2,NULL,0);
    
}
void debug_function(cmdLine* pCmdLine){
    int from=0;
    for (int i=0; i<(pCmdLine->argCount); i++) {
        if(strcmp(pCmdLine->arguments[i],"-d")==0){
            from=i+1;
            break;
        }
    }
    for (; from<(pCmdLine->argCount); from++) {
        if(strcmp(pCmdLine->arguments[from],"-d")==0){
            continue;
        }
        fprintf(stderr,"%s ",pCmdLine->arguments[from]);
    }
    printf("\n");
}
void Duplicate(cmdLine* pCmdLine){
    if (pCmdLine->inputRedirect) {
            int inp_f=open(pCmdLine->inputRedirect, O_RDONLY);
            if(inp_f==-1){
                perror("cant open input file");
                exit(1);
            }
            if (dup2(inp_f, STDIN_FILENO) == -1) {
            perror("error dupplicate to input");
            exit(1);
      }
            close(inp_f);
        }
        if (pCmdLine->outputRedirect) {
            int out_f=open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (out_f==-1) {
             perror("cant open output file");
                exit(1);
            }
            if (dup2(out_f, STDOUT_FILENO) == -1) {
        perror("error dupplicate to output");
        exit(1);
      }
      close(out_f);
        }
}
void execute(cmdLine *pCmdLine) {
    pid_t P_id = fork();
    if (P_id == -1) {
        perror("fork");
        _exit(1);
    } else if (P_id == 0) {
      //Child process started executing command
      addProcess(&process_list, pCmdLine, P_id);
      Duplicate(pCmdLine);
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("execvp");
            exit(1);
        }
        
    } else {
      //Parent process waiting for child process to finish  executing command
      if(pCmdLine->blocking){
        if (waitpid(P_id, NULL, 0) == -1) {
            perror("waitpid");
            exit(1);
        }
      }
    }
    //Child process finish executing command
}
int There_Is_d(char **argv ,int argc){
    for (int i=0; i<argc; i++) {
        if(strcmp(argv[i],"-d")==0){
            return 1;
        }
    }
    return 0;
}
void wake(pid_t pid) {
   
    if (kill(pid, SIGCONT) == -1) {
        perror("kills sigcont failed");
    } else {
        printf("the Process %d has been woken up\n", pid);
    }
}
void suspend(pid_t pid) {
    if (kill(pid, SIGTSTP) == -1) {
        perror("kills sigtstp failed");
    } else {
        printf("the Process %d has been suspended\n", pid);
    }
}
void kills(pid_t pid) {
    if (kill(pid, SIGINT) == -1) {
        perror("kills sigkill failed");
    } else {
        printf("the Process %d has been killed\n", pid);
    }
}
int main(int argc, char **argv)
{
char* history_queue[HISTLEN];
int newest=-1;
int oldest=0;
char current_work[PATH_MAX];
char line[2048];
cmdLine *pCmdLine ;
int exsist =There_Is_d(argv,argc);

while (1) {
    if (getcwd(current_work, sizeof(current_work)) == NULL) {
        perror("error getcwd");
        exit(1);
    }
    fprintf(stderr,"%s> ", current_work);
    if (fgets(line, sizeof(line), stdin) == NULL) {
        perror("error fgets");
        exit(1);
    }
    pCmdLine = parseCmdLines(line);
    Add_To_History(history_queue ,pCmdLine,&newest,&oldest);
    if(exsist){
    fprintf(stderr, "PID is: %d\n", getpid());
    fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
    }
    if (strcmp(pCmdLine->arguments[0], "cd") == 0) {
        if (chdir(pCmdLine->arguments[1]) == -1) {
            perror("chdir");
        }
    freeCmdLines(pCmdLine);
    continue;
    }
    if (strcmp(pCmdLine->arguments[0], "quit") == 0) {
        freeCmdLines(pCmdLine);
        exit(1);
    }
    else if (strcmp(pCmdLine->arguments[0], "history") == 0) {
        Print_History_queue(history_queue,&newest,&oldest);
    
    }
    else if (strcmp(pCmdLine->arguments[0], "!!") == 0) {
        if (newest==-1&&strcmp(history_queue[newest],"!!")==0) {
            printf("There is no previous history\n");
        }else {
            printf("%s\n",history_queue[newest-1]);
        }
    }
    else if (pCmdLine->arguments[0][0]== '!' &&pCmdLine->arguments[0][1]>0) {
        get_history_queue(pCmdLine->arguments[0][1],&newest,&oldest,history_queue);
    }
    
    else if (pCmdLine->next) {
        mypipeline(pCmdLine,pCmdLine->next);
    }
    else if (strcmp(pCmdLine->arguments[0], "suspend") == 0) {
            if (pCmdLine->argCount>1) {
                suspend(atoi(pCmdLine->arguments[1]));
            }
            else {
                printf("there is no id to Suspend\n");
            }
    }
    else if (strcmp(pCmdLine->arguments[0], "wake") == 0) {
            if (pCmdLine->argCount>1) {
                wake(atoi(pCmdLine->arguments[1]));
            }
            else {
                printf("there is no id to wake\n");
            }
    }
    else if (strcmp(pCmdLine->arguments[0], "procs") == 0) {
        printProcessList(&process_list);
    }
    else if (strcmp(pCmdLine->arguments[0], "kill") == 0) {
            if (pCmdLine->argCount>1) {
                kills(atoi(pCmdLine->arguments[1]));
            }
            else {
                printf("there is no id to kill\n");
            }
        }
    else {
        execute(pCmdLine);
    }
    freeCmdLines(pCmdLine);
}
	return 0;
}