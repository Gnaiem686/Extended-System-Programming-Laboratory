#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  //////////////my additions//////////////
  char display_mode;
  int num_files ;
  char* files[100];
  void* maps[100];
  void* addrs[100];
  int sizes[100];
  int index[2];
   
  ////////////////////////////////////////
} state;
state myState;

struct fun_desc{
    char *name;
    void (*fun)(state*);
};

void Quit(state* s);
int fcloseall();

void Toggle_Debug_Mode(state* s){
    if(s->debug_mode == 0){ // the mode is off
    s->debug_mode = 1;
    printf("Debug flag now on\n");
    }
    else
    {
    s->debug_mode = 0;
    printf("Debug flag now off\n");
    }
    
};

void Examine_ELF_File(state* s){
    ////To get the name of the file as in lab4////
    
    
    printf("please enter file name: ");
    char filename[100];
    fgets(filename,100,stdin);

    size_t len = strlen(filename);
    if (filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }


    filename[100 - 1] = '\0';
    
    if(s->debug_mode == 1){ //the debug mode is on
        printf("Debug: file name set to %s \n",filename );
    }
    strncpy(s->file_name, filename, sizeof(s->file_name));
    /////////////////////////////////////////////////////////
    char* newString = malloc(strlen(filename) + 1);
    strcpy(newString, filename); // Copy the contents to the allocated memory
    
    
    

    if(strcmp(s->file_name, "") == 0){
        printf("error: file name is empty\n");
        return;
    }
    FILE* file =  fopen(s->file_name,"r");
    if(file == NULL){
        printf("error: cannot open file\n");
        return;
    }
    int found = 0;
    for (int i = 0; i < s->num_files; i++) {
        if (s->file_name == s->files[i]) {
            
            found = 1; // File exists in the array
            break;
        }
    }
    int open = fileno(file);
     if(found == 0){ 
        s->files[s->num_files] = newString;
        s->num_files = s->num_files + 1;
        off_t fileSize = lseek(open, 0, SEEK_END);
        void* maped = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, open, 0);
        s->maps[s->num_files-1] = maped;
     }
    
    
    for(int i = 0; i < s->num_files; i++){
    file = fopen(s->files[i], "r");
    open = fileno(file);
     unsigned char* elf = (unsigned char*)s->maps[i];
     printf("\nFile: %s\n", s->files[i]);
     printf("First three bytes: %c %c %c\n", elf[1], elf[2], elf[3]);
    if((elf[1] != 'E') || (elf[2] != 'L') || (elf[3] != 'F')){
        printf("The file you provided is not ELF\n");
        Quit(s);
    }
    printf("The data encoding scheme of the object file: %d\n", elf[EI_DATA]);
    

    Elf32_Ehdr* header = (Elf32_Ehdr*)s->maps[i];
    printf("Entry point: 0x%x\n", header->e_entry);
    printf("The file offset in which the section header table resides: %d\n", header->e_shoff);
    printf("The number of section header entries: %d\n", header->e_shnum);
    printf("The size of each section header entry: %d\n", header->e_shentsize);
    printf("The file offset in which the program header table resides: %d\n", header->e_phoff);
    printf("The number of program header entries: %d\n", header->e_phnum);
    printf("The size of each program header entry: %d\n", header->e_phentsize);
    }
    
}

void Print_Section_Names(state* s){
    for(int i = 0; i < s->num_files; i++){
        fopen(s->files[i], "r");
        printf("File %s\n", s->files[i]);
        Elf32_Ehdr* header = (Elf32_Ehdr*)s->maps[i];
        Elf32_Shdr *section_header_table = (Elf32_Shdr *)((char *)s->maps[i] + header->e_shoff);
        Elf32_Shdr *section_header_string_table = &section_header_table[header->e_shstrndx];

    char *section_names = (char *)s->maps[i] + section_header_string_table->sh_offset;


    printf("Section Headers:\n");
    printf("[index] section_name section_address section_offset section_size section_type\n");

    for (int i = 0; i < header->e_shnum; i++)
    {
        Elf32_Shdr *section_header = &section_header_table[i];
        printf("[%d] %s %x %x %x %u\n", i, &section_names[section_header->sh_name],
               section_header->sh_addr, section_header->sh_offset,
               section_header->sh_size, section_header->sh_type);
    }
}
}

void Print_Symbols(state* s) {
    for (int i = 0; i < s->num_files; i++) {
        FILE* file = fopen(s->files[i], "r");
        if (file == NULL) {
            printf("Failed to open file: %s\n", s->files[i]);
            continue;
        }
        
        printf("File: %s\n", s->files[i]);
        Elf32_Ehdr* header = (Elf32_Ehdr*)s->maps[i];
        Elf32_Shdr* section_header_table = (Elf32_Shdr*)(s->maps[i] + header->e_shoff);
        
        for (int a = 0; a < header->e_shnum; a++) {
            if (section_header_table[a].sh_type == SHT_SYMTAB || section_header_table[a].sh_type == SHT_DYNSYM) {
                // Found a symbol table section
                Elf32_Sym* symbolTable = (Elf32_Sym*)((char*)header + section_header_table[a].sh_offset);
                Elf32_Shdr* symbol_table_header = &section_header_table[a];
                char* string_table = (char*)(s->maps[i] + section_header_table[symbol_table_header->sh_link].sh_offset);
                
                printf("[index] value   section_index section_name symbol_name\n");
                for (int j = 0; j < symbol_table_header->sh_size / sizeof(Elf32_Sym); j++) {
                    Elf32_Sym* symbol = &symbolTable[j];
                    char* symbol_name = string_table + symbol->st_name;
                    printf("[%2d] %08x ", j, symbol->st_value);
                    
                    if (symbol->st_shndx == SHN_UNDEF) {
                        printf("%2d             Undefined         ", symbol->st_shndx);
                    } else {
                        printf("%2d             Defined           ", symbol->st_shndx);
                    }
                    
                    printf("%s\n", symbol_name);
                }
            }
        }
        
        fclose(file);
    }
}

void Check_Files_for_Merge(state* s){
    
    if(s->num_files < 2){
        printf("There is less than 2 files\n");
        Quit(s);
    }
    for(int i = 0; i < s->num_files; i++){ /// loop over the files to check if every one has exactly one symbol type
        int count = 0;
        Elf32_Ehdr* header = (Elf32_Ehdr*)s->maps[i];
        Elf32_Shdr *section_header_table = (Elf32_Shdr *)(s->maps[i] + header->e_shoff);
        for(int j = 0; j < header->e_shnum; j++){
        Elf32_Shdr *section_header = &section_header_table[j];
        if(section_header->sh_type == SHT_SYMTAB || section_header->sh_type == SHT_DYNSYM){
            s->index[i] = j;
            count++;
        }
        }
        if(count != 1){
            printf("feature not supported\n");
            Quit(s);
        }
    }
        Elf32_Ehdr* header0 = (Elf32_Ehdr*)s->maps[0];
        Elf32_Shdr *section_header_table0 = (Elf32_Shdr *)(s->maps[0] + header0->e_shoff);
        Elf32_Shdr *symbol_table_header0 = &section_header_table0[s->index[0]]; 
        Elf32_Sym *symbol_table0 = (Elf32_Sym *)(s->maps[0] + symbol_table_header0->sh_offset);
        char *string_table0 = (char *)(s->maps[0] + section_header_table0[symbol_table_header0->sh_link].sh_offset);


        Elf32_Ehdr* header1 = (Elf32_Ehdr*)s->maps[1];
        Elf32_Shdr *section_header_table1 = (Elf32_Shdr *)(s->maps[1] + header1->e_shoff);
        Elf32_Shdr *symbol_table_header1 = &section_header_table1[s->index[1]]; 
        Elf32_Sym *symbol_table1 = (Elf32_Sym *)(s->maps[1] + symbol_table_header1->sh_offset);
        
        char *string_table1 = (char *)(s->maps[1] + section_header_table1[symbol_table_header1->sh_link].sh_offset);

        for(int i = 1; i < symbol_table_header0->sh_size / sizeof(Elf32_Sym); i++){ // loop over the symbols in the first symbol table
            Elf32_Sym *symbol0 = &symbol_table0[i];
            char *symbol_name0 = string_table0 + symbol0->st_name;
            if(strcmp(symbol_name0, "") == 0){
                continue;
            }
            if(symbol0->st_shndx == SHN_UNDEF){ // the sym is undefined 
            int found1 = 0;
            int defined = -1;
                for(int j = 1; j < symbol_table_header1->sh_size / sizeof(Elf32_Sym); j++){
                    Elf32_Sym *symbol1 = &symbol_table1[j];
                    char *symbol_name1 = string_table1 + symbol1->st_name;
                    if(strcmp(symbol_name0, symbol_name1) == 0){
                        found1 = 1; // we found sym in the second table
                        defined = symbol1->st_shndx;
                        
                    }
                }

                if(found1 == 0 || defined == 0){
                    printf("Symbol sym undefined\nCan't Merge\n");
                    Quit(s);
                }
            }

            else{ // sym is defined
                int found1 = 0;
                int defined = -1;
                
                for(int j = 1; j < symbol_table_header1->sh_size / sizeof(Elf32_Sym); j++){
                    Elf32_Sym *symbol1 = &symbol_table1[j];
                    char *symbol_name1 = string_table1 + symbol1->st_name;
                    if(strcmp(symbol_name0, symbol_name1) == 0){
                        found1 = 1; // we found sym in the second table
                        defined = symbol1->st_shndx;
                        printf("the name: %s\n", symbol_name1);
                        break;
                    }
                    
                }
                if(found1 == 1 && defined != 0){
                    printf("Symbol sym multiply defined\nCan't Merge\n");
                    Quit(s);
                    
                }
             }

        }
        
        

        
    




}

void Merge_ELF_Files(state* s){
    FILE* out_file = fopen("out.ro", "wb");
    if(out_file == NULL){
        printf("cannot open out file\n");
        Quit(s);
    }
    // The prime informations from the first and second files
    Elf32_Ehdr* header1 =(Elf32_Ehdr*)s->maps[0];
    Elf32_Ehdr* header2 =(Elf32_Ehdr*)s->maps[1];
    Elf32_Shdr* section_header1 = (Elf32_Shdr*)(s->maps[0] + header1->e_shoff);
    Elf32_Shdr* section_header2 = (Elf32_Shdr*)(s->maps[1] + header2->e_shoff);
    char* string_table1 = (char*)(s->maps[0] + section_header1[header1->e_shstrndx].sh_offset);
    char* string_table2 = (char*)(s->maps[1] + section_header2[header2->e_shstrndx].sh_offset);

    //Creat the section header for the out file
    char out_shdr[header1->e_shnum * header1->e_shentsize];
    memcpy(out_shdr, (char*)(section_header1), header1->e_shnum * header1->e_shentsize);

    //The sections of the second file and the offset of the out file
    Elf32_Shdr* section_type = NULL;
    long offset;

    //To look like elf file 
    fwrite((char*)s->maps[0], 1, 52, out_file);
    
    // now loop over the sections in the first file
    for(int i = 0; i < header1->e_shnum; i++){
        fseek(out_file, 0, SEEK_CUR);
        offset = ftell(out_file);
        if(strncmp(string_table1 + section_header1[i].sh_name, ".text", 5) == 0||
            strncmp(string_table1 + section_header1[i].sh_name, ".data", 5) == 0||
            strncmp(string_table1 + section_header1[i].sh_name, ".rodata", 7) == 0){
            fwrite((char*)(s->maps[0] + section_header1[i].sh_offset), 1, section_header1[i].sh_size, out_file);
            for(int j = 0; j < header2->e_shnum; j++){ //loop over the sections in the second file to match the section type 
                    if(strcmp(string_table1 + section_header1[i].sh_name, string_table2 + section_header2[j].sh_name) == 0){
                        section_type = section_header2 + j;
                        fwrite((char*)(s->maps[1] + section_type->sh_offset), 1, section_type->sh_size, out_file);
                        ((Elf32_Shdr*)out_shdr)[i].sh_size += section_type->sh_size;
                        ((Elf32_Shdr*)out_shdr)[i].sh_offset = offset;
                    }
            }
            }
        else if(strncmp(string_table1 + section_header1[i].sh_name, ".symtab", 7) == 0){
                char out_symbols[section_header1[i].sh_size];
                memcpy(out_symbols, (char*)(s->maps[0] + section_header1[i].sh_offset), section_header1[i].sh_size);
                
                // Get the important informations from the first file
                Elf32_Shdr *section_header_table1 = (Elf32_Shdr *)(s->maps[0] + header1->e_shoff);
                Elf32_Shdr *symbol_table_header1 = &section_header_table1[s->index[0]]; 
                Elf32_Sym *symbol_table1 = (Elf32_Sym *)(s->maps[0] + symbol_table_header1->sh_offset);
                char *strtab1 = (char *)(s->maps[0] + section_header_table1[symbol_table_header1->sh_link].sh_offset);
                int size1 = symbol_table_header1->sh_size / sizeof(Elf32_Sym);

                // Get the important information from the second file 
                Elf32_Shdr *section_header_table2 = (Elf32_Shdr *)(s->maps[1] + header2->e_shoff);
                Elf32_Shdr *symbol_table_header2 = &section_header_table2[s->index[1]]; 
                Elf32_Sym *symbol_table2 = (Elf32_Sym *)(s->maps[1] + symbol_table_header2->sh_offset);
                char *strtab2 = (char *)(s->maps[1] + section_header_table2[symbol_table_header2->sh_link].sh_offset);
                int size2 = symbol_table_header2->sh_size / sizeof(Elf32_Sym);

                // loop over the first symbol table 
                for(int i = 1; i < size1; i++){
                    if(symbol_table1[i].st_shndx == SHN_UNDEF){
                        for(int j = 1; j < size2; j++){
                            if(strcmp(strtab1 + symbol_table1[i].st_name, strtab2 + symbol_table2[j].st_name) == 0){
                                ((Elf32_Sym*)out_symbols + i)->st_shndx = (symbol_table2+j)->st_shndx;
                                ((Elf32_Sym*)out_symbols + i)->st_value = (symbol_table2+j)->st_value;
                            }
                        }
                    }
                }
                fwrite(out_symbols, 1, section_header1[i].sh_size, out_file);
                ((Elf32_Shdr*)out_shdr)[i].sh_offset = offset;
            }
        else { // any other section 
            section_type = section_header1 + i;
            fwrite((char*)(s->maps[0] + section_header1[i].sh_offset), 1, section_header1[i].sh_size, out_file);
            ((Elf32_Shdr*)out_shdr)[i].sh_size += section_type->sh_size;
            ((Elf32_Shdr*)out_shdr)[i].sh_offset = offset;
        }
    }

    long shoff = ftell(out_file);
    fwrite(out_shdr, header1->e_shentsize, header1->e_shnum, out_file);

    // we use 32 because the size of the elf header is 52
    fseek(out_file, 32, SEEK_SET);

    fwrite(&shoff, 4, 1, out_file);
    fclose(out_file);
     printf("the files merged successfuly\n");

}

void Quit(state* s){
    printf("quitting\n");
    for (int i = 0; i < s->num_files; i++) {
        if (s->addrs[i] != MAP_FAILED) {
            munmap(s->addrs[i], s->sizes[i]);  // Unmap the file
        }
    }
    fcloseall(); //Just in case
    exit(0);
};

int main(int argc, char **argv){
struct fun_desc menu[] = { 
{ "Toggle Debug Mode", Toggle_Debug_Mode},
{ "Examine_ELF_File", Examine_ELF_File }, 
{ "Print_Section_Names", Print_Section_Names }, 
{ "Print_Symbols", Print_Symbols }, 
{ "Check_Files_for_Merge", Check_Files_for_Merge },
{ "Merge_ELF_Files", Merge_ELF_Files },
{ "Quit", Quit },
{ NULL, NULL } };

char option[5];
state myState;
myState.debug_mode = 0; // its off
myState.display_mode = 0;
myState.num_files = 0;
myState.index[0] = 0;
myState.index[1] = 0;

while(1){
if(myState.debug_mode == 1){
    printf("Unit Size: %d\n", myState.unit_size);
    printf("File Name: %s\n", myState.file_name);
}
printf("Chose action:\n");
for(int i = 0; i < 7 ; i++){
    printf("%d-", i);
    printf("%s\n", menu[i].name);
}
fgets(option,5,stdin);
int op = option[0] - '0';
menu[op].fun(&myState);
}

}