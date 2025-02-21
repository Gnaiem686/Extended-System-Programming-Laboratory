#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int EncodeAlphabets(int SubOrAdd,int ch,int character,char **argv,int arg_here,int i){
    if(SubOrAdd){
        if ((character>='a'&&character<='z')) {
            ch=((character+ (argv[arg_here][i]-'0')-'a') % 26)+'a';
            }
        else if ((character>='A'&&character<='Z')){
            ch=((character+ (argv[arg_here][i]-'0')-'A') % 26)+'A';
        }
        else if(character>='0'&&character<='9'){
            ch=((character+ (argv[arg_here][i]-'0')-'0') % 10)+'0';
        }
    }    
    else{//sub
            if ((character>='a'&&character<='z')) {
                ch=((character- (argv[arg_here][i]-'0')-'a'+26) % 26)+'a';
            }
            else if ((character>='A'&&character<='Z')) {
                ch=((character- (argv[arg_here][i]-'0')-'A'+26) % 26)+'A';
            }
            else if(character>='0'&&character<='9'){
                ch=((character- (argv[arg_here][i]-'0')-'0'+10) % 10)+'0';
            }
    }
    
    return ch;
}
void Debug_func(int starting_debug,int debug_until,char **argv){
     for (; starting_debug<debug_until; starting_debug++) {
    //     if(argv[starting_debug][0]=='-'){
    //     if (argv[starting_debug][1]=='i'){
    //     continue;
    //     }
    //     else if (argv[starting_debug][1]=='o') {
    //     continue;
    //     }
    // }
        if(strcmp(argv[starting_debug],"+D")==0){//2 +d (break points) in the same line code
            continue;
        }
        else{
            fprintf(stderr,"%s ",argv[starting_debug]);
        }
    }
    printf("\n");
    return ;
}
int main(int argc, char **argv) {
    char *fin=NULL;
    char *fout=NULL;
    FILE * input=stdin;
    FILE * output=stdout;
    bool Encode_Mode=false;
    int starting_debug=-1;
    int debug_until=argc;
    bool found_D=false;
    int SubOrAdd=-1;
    int arg_here=-1;
    int len=-1;
for (int j=1;j<argc;j++){
    if(argv[j][1]=='e'&&(argv[j][0]=='+'||argv[j][0]=='-')) {
    Encode_Mode=true;
    SubOrAdd=(argv[j][0]=='+');//if 1 add if 0 sub
    len=(strlen(argv[j]));
    arg_here=j;
    }
    if(argv[j][0]=='-'){
        if (argv[j][1]=='i'){//input file
        fin=argv[j]+2;
        }
        else if (argv[j][1]=='o') {//output file
        fout=argv[j]+2;
        }
    }
    if(!found_D&&strcmp(argv[j],"+D")==0){
        starting_debug=j+1;
        found_D=true;
    }
    if ((strcmp(argv[j],"-D")==0)&&found_D) {
        debug_until=j;
    }
    //fprintf(stderr,"invaild argument: %s\n",argv[i]);
}if (fin!=NULL) {
    input=fopen(fin,"read");

}
if (fout!=NULL) {
    output=fopen(fout,"write");

}
if(starting_debug!=-1){
    Debug_func(starting_debug,debug_until,argv);
}
if(Encode_Mode){
//printf("inter your letters:\n");
int i=2;
int character;
while ((character=fgetc(input))!=EOF){
    int ch=character;
    if(isalnum(character)){
    ch=EncodeAlphabets(SubOrAdd,ch,character,argv,arg_here,i);
    }
    i=(i+1)%len;
    if(i<2){i=2;}
    fputc(ch,output);
}
}
else{
    int character;
    while ((character=fgetc(input))!=EOF){
        int ch=character;
        fputc(ch,output);
    }
}
printf("\n");
fclose(input);
fclose(output);
}
