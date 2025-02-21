#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



typedef struct virus
{
    unsigned short SigSize;
    char virusName[16];
    unsigned char *sig;
} virus;

typedef struct link link;
struct link
{
    link *nextVirus;
    virus *vir;
};

typedef struct menu
{
    char *name;
    link *((*function)(link *));
} menu;

char *INFNECTED_FILE;

void list_print(link *virus_list, FILE *output);
void detect_virus(unsigned char *buffer, unsigned int size, link *virus_list);
void neutralize_virus(char *fileName, int signatureOffset);
void list_free(link *virus_list);

void PrintSignature(virus *vir, FILE *output)
{
    for (int i = 0; i < vir->SigSize; i++)
    {
        fprintf(output, "%X ", vir->sig[i]);
    }
    fprintf(output, "\n\n");
}
virus *readVirus(FILE *input)
{
    virus *vir = (virus *)malloc(sizeof(virus));
    if (vir == NULL)
    {
        printf("cant alloc in heap");
        free(vir);
        exit(1);
    }
    if (fread(&vir->SigSize, sizeof(unsigned short), 1, input) == 0)
    {
        free(vir);
        return NULL;
    }
    if (fread(vir->virusName, sizeof(char), 16, input) == 0)
    {
        free(vir);
        return NULL;
    }
    vir->sig = (unsigned char *)malloc(vir->SigSize);
    if (vir->sig == NULL)
    {
        printf("cant alloc in heap");
        free(vir->sig);
        free(vir);
        exit(1);
    }
    if (fread(vir->sig, sizeof(unsigned char), vir->SigSize, input) == 0)
    {
        free(vir->sig);
        free(vir);
        return NULL;
    }
    return vir;
}

void printVirus(virus *vir, FILE *output)
{

    fprintf(output, "VirusName: %s\n", vir->virusName);
    fprintf(output, "VirusSize: %d\n", vir->SigSize);
    fprintf(output, "Signature:\n");
    PrintSignature(vir, output);
}

link *list_append(link *virus_list, virus *data)
{
    link *temp = malloc(sizeof(link));
    temp->vir = data;
    temp->nextVirus = virus_list;
    return temp;
}

void list_free(link *virus_list)
{
    link *temp;
    while (virus_list != NULL)
    {
        temp = virus_list;
        free(virus_list->vir->sig);
        free(virus_list->vir);
        virus_list = virus_list->nextVirus;
        free(temp);
    }
}

void list_print(link *virus_list, FILE *output)
{
    while (virus_list != NULL && virus_list->vir != NULL)
    {
        printVirus(virus_list->vir, output);
        virus_list = virus_list->nextVirus;
    }
}

link *QUIT(link *current_list)
{
    list_free(current_list);
    exit(1);
    return current_list;
}
void fix_helper(link *temp_list,int min_size,unsigned char *buffer){
     while (temp_list != NULL)
    {
        int vsize = temp_list->vir->SigSize;
        for (int i = 0; i < min_size - vsize; i++)
        {
            if (memcmp(buffer + i, temp_list->vir->sig, vsize) == 0)
            {
                neutralize_virus(INFNECTED_FILE, i);
            }
        }
        temp_list = temp_list->nextVirus;
    }
}
link *Fix_file(link *current_list)
{
    FILE *file = fopen(INFNECTED_FILE, "r");
    unsigned char *buffer = (unsigned char *)malloc(10000); //constant size of 10k bytes
    fix_helper(current_list,fread(buffer, 1, 10000, file),buffer);
    fclose(file);
    free(buffer);
    return current_list;
}

void neutralize_virus(char *fileName, int signatureOffset)
{
    FILE *infected_file = fopen(fileName, "r+");
    if (infected_file == NULL)
    {
        printf("cannot open file: %s\n", fileName);
        fclose(infected_file);
        return;
    }
    unsigned char modifier = 0xC3;
    fseek(infected_file, signatureOffset, SEEK_SET);
    fwrite(&modifier, sizeof(unsigned char), 1, infected_file);
    fclose(infected_file);
}

link *Detect_viruses(link *current_list)
{
    FILE *infected_file = fopen(INFNECTED_FILE, "rb");
    unsigned char *buffer = (unsigned char *)malloc(10000); //constant size of 10k bytes
    int min_size = fread(buffer, 1, 10000, infected_file);
    detect_virus(buffer, min_size, current_list);
    free(buffer);
    fclose(infected_file);
    return current_list;
}
void detect_helper(unsigned char *buffer,int count,int vsize,char *vname,int size,link* virus_list){
    for (int i = 0; i < size - vsize; i++)
        {
            if (memcmp(buffer + i, virus_list->vir->sig, virus_list->vir->SigSize) == 0)
            {
                printf("virus%d\n", count);
                printf("Location: (0x%X)\n", i);
                printf("Virus Name: %s\n", vname);
                printf("size: %d\n", vsize);
                count++;
            }
        }
}
void detect_virus(unsigned char *buffer, unsigned int size, link *virus_list)
{
    while (virus_list != NULL)
    {
        detect_helper(buffer,1,(virus_list->vir->SigSize),(virus_list->vir->virusName),size,virus_list);
        virus_list = virus_list->nextVirus;
    }
}

link *Print_signatures(link *current_list)
{
    list_print(current_list, stdout);
    return current_list;
}
link *Load(link *current_list)
{
    int index = 0;
    char str[100];
    printf("Enter your fileName: ");
    fgets(str, 100, stdin);
    if (str[0] == EOF)
        exit(1);

    for (int i = 0; str[i] != '\n'; i++)
    {
        index = i;
    }
    str[index + 1] = '\0';

    FILE *file = fopen(str, "rb");
    if (file == NULL)
    {
        printf("cant open the file\n");
        exit(1);
    }
char magic_numbers[4];
    if (fread(magic_numbers, 4, 1, file) == 0)
    {
        printf("cant read\n");
        exit(1);
    }

    if (memcmp(magic_numbers, "VISL", 4) != 0)
    {
        printf("not correct\n");
        fclose(file);
        exit(1);
    }
    virus *vir = readVirus(file);
    while (vir != NULL)
    {
        current_list = list_append(current_list, vir);
        vir = readVirus(file);
    }
    printf("Load succsessfuly\n");
    
    fclose(file);
    return current_list;
}

int main(int argc, char **argv)
{
    if (argc<2) {
        printf("there is missing arguments\n");
        exit(1);
    }
    INFNECTED_FILE = argv[1];  // initialize the INFECTION FILE
    link *current_list = NULL; // initialize the current list
    struct menu menu[] = {
        {NULL, NULL},
        {"Load signatures", Load},
        {"Print signatures", Print_signatures},
        {"Detect viruses ", Detect_viruses},
        {"Fix file", Fix_file},
        {"Quit ", QUIT},

    };
    printf("choose an OPtion:\n");
    for (int i = 1; i <= 5; i++)
    {
        printf("%d)\t%s\n", i, menu[i].name);
    }
    printf("option: ");
    char input[20];
    fgets(input, 20, stdin);
    while (input[0] != EOF)
    {
        int val = input[0] - '0';
        if (val >= 1 && val <= 5)
        {
            printf("within bounds\n");
            current_list = menu[val].function(current_list);
            printf("DONE.\n\n");
            printf("choose an OPtion:\n");
            for (int i = 1; i <= 5; i++)
            {
                printf("%d)\t%s\n", i, menu[i].name);
            }
            printf("option: ");
        }
        else
        {
            printf("the input is not a number from 1 to 5\n");
            fgets(input, 20, stdin);
            continue;
        }
        if (fgets(input, 20, stdin) == NULL)
            break;
    }

    return 0;
}