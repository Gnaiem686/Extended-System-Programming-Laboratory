#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int addr5;
int addr6;

int foo()
{
    return -1;
}
void point_at(void *p);
void foo1();
char g = 'g';
void foo2();

int main(int argc, char **argv)
{ 
    
    int addr2;
    int addr3;
    char *yos = "ree";
    int *addr4 = (int *)(malloc(50));
    
    printf("Print addresses:\n");
    printf("- &addr2: %p\n", &addr2);
    printf("- &addr3: %p\n", &addr3);
    printf("- foo: %p\n", &foo);
    printf("- &addr5: %p\n", &addr5);

    printf("- argc %p\n", &argc);
    printf("- argv %p\n", argv);
    printf("- &argv %p\n", &argv);
    
    printf("Print distances:\n");
    point_at(&addr5);

    printf("Print more addresses:\n");
    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4);
    printf("- &addr4: %p\n", &addr4);

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);
    
    printf("Print another distance:\n");
    printf("- &foo2 - &foo1: %ld\n", (long) (&foo2 - &foo1));

   
    printf("Arrays Mem Layout (T1b):\n");
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3]; 
    /* task 1 b here */
    
    
    printf("the hexadecimal value of iarray is:%p\nthe hexadecimal value of iarray+1 is:%p\nthe addres of iarray[%d]: %p,the addres of iarray[%d]: %p,the addres of iarray[%d]: %p\n",(iarray),(iarray)+1,0,(void*)&iarray[0],1,(void*)&iarray[1],2,(void*)&iarray[2]);
    printf("the hexadecimal value of farray is:%p\nthe hexadecimal value of farray+1 is:%p\nthe address of farry[%d]: %p,the address of farry[%d]: %p,the address of farry[%d]: %p\n",farray,farray+1,0,(void*)&farray[0],1,(void*)&farray[1],2,(void*)&farray[2]);
    printf("the hexadecimal value of darray is:%p\nthe hexadecimal value of darray+1 is:%p\nthe address of darray[%d]: %p,the address of darray[%d]: %p,the address of darray[%d]: %p\n",darray,darray+1,0,(void*)&darray[0],1,(void*)&darray[1],2,(void*)&darray[2]);
    printf("the hexadecimal value of carray is:%p\nthe hexadecimal value of carray+1 is:%p\nthe address of carray[%d]: %p,the address of carray[%d]: %p,the address of carray[%d]: %p\n",carray,carray+1,0,(void*)&carray[0],1,(void*)&carray[1],2,(void*)&carray[2]);
    
    
    printf("Pointers and arrays (T1d): ");
    int iarray2[] = {1,2,3};
    char carray2[] = {'a','b','c'};
    int* iarray2Ptr;
    char* carray2Ptr; 
    /* task 1 d here */
    iarray2Ptr=iarray2;
    carray2Ptr=carray2;
     for (int i=0; i<3; i++) {
    int val=*(iarray2Ptr+i);
    printf("%d ",val);
    }
    for (int i=0; i<3; i++) {
    int val=*(carray2Ptr+i);
    printf("%c ",val);
    }
    int *p;
    printf("%p\n",p);
    printf("Command line arg addresses (T1e):\n");
    /* task 1 e here */
    for (int i=0; i<argc; i++) {
        printf("argv[%d]-> address=%p",i,(void*)&argv[i]);
        printf("argv[%d] content is %s\n", i, argv[i]);
    }
    return 0;
}

void point_at(void *p)
{
    int local;
    static int addr0 = 2;
    static int addr1;

    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;

    printf("- dist1: (size_t)&addr6 - (size_t)p: %ld\n", dist1);
    printf("- dist2: (size_t)&local - (size_t)p: %ld\n", dist2);
    printf("- dist3: (size_t)&foo - (size_t)p:  %ld\n", dist3);
    
    printf("Check long type mem size (T1a):\n");
    /* part of task 1 a here */
    printf("%ld\n",sizeof(long));
    printf("- addr0: %p\n", &addr0);
    printf("- addr1: %p\n", &addr1);
}

void foo1()
{
    printf("foo1\n");
}

void foo2()
{
    printf("foo2\n");
}
