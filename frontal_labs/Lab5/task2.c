#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <elf.h>
#include <unistd.h>
#include <string.h>

extern int startup(int argc, char** argv, int (*func)(int, char**));
//task 0
void printer_phdr(Elf32_Phdr *phdr, int index) {
    printf("Program header number %d at address 0x%x\n", index, (unsigned int)phdr);
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
    Elf32_Ehdr *headers = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *p_header_group = (Elf32_Phdr *)(map_start + headers->e_phoff);
    int len =headers->e_phnum;
    for (int i = 0; i < len; i++) {
        func(&p_header_group[i], arg);
    }
    return 0;
}

void print_appropriate_protection_flags(Elf32_Phdr *phdr){
    if (phdr->p_type != PT_LOAD) {
        printf("header type  %d not exsist in mapping\n", phdr->p_type);
        return;
    }
    char flags[4] = {' ', ' ', ' ', '\0'};
char* flagNames = "RWE";

for (int i = 0; i < 3; i++) {
    if (phdr->p_flags & (1 << i))
        flags[i] = flagNames[i];
}


    int prot_flags = 0;
    if (phdr->p_flags & PF_R)
        prot_flags |= PROT_READ;
    if (phdr->p_flags & PF_W)
        prot_flags |= PROT_WRITE;
    if (phdr->p_flags & PF_X)
        prot_flags |= PROT_EXEC;

    int mapping_flags = MAP_PRIVATE | MAP_FIXED;
    if (phdr->p_flags & PF_W)
        mapping_flags |= MAP_SHARED;

    // printf("header type: %d\n", phdr->p_type);
    // printf("Protection flags: %d\n", prot_flags);
    // printf("Mapping flags: %d\n", mapping_flags);
    // printf("the flags: %s\n",flags);
}

void printTheFlag(Elf32_Phdr *phdr){
    printf("%c%c%c ", (phdr->p_flags & PF_R) ? 'R' : ' ',
           (phdr->p_flags & PF_W) ? 'W' : ' ',
           (phdr->p_flags & PF_X) ? 'E' : ' ');
}
void printThetype(Elf32_Phdr *phdr){
    printf("%s ", (phdr->p_type == 1) ? "LOAD" :
            (phdr->p_type == 4) ? "NOTE" :
            (phdr->p_type == PT_GNU_STACK) ? "GNU_STACK" :
           (phdr->p_type == 2) ? "GNU_RELRO" :
           (phdr->p_type == 6) ? "PHDR" :
            (phdr->p_type == 3) ? "INTERP" :
           (phdr->p_type == PT_GNU_RELRO) ? "DYNAMIC" : "UNDECLARED");
}
void off_printer(Elf32_Phdr *phdr){
printf("0x%06x ",phdr->p_offset);
}
void vad_pad(Elf32_Phdr *phdr){
    printf("0x%08x 0x%08x ", phdr->p_vaddr, phdr->p_paddr);
}
void sizes_print(Elf32_Phdr *phdr){
    printf("0x%05x 0x%05x ",phdr->p_filesz, phdr->p_memsz);
}
void align_printer(Elf32_Phdr *phdr){
    printf("0x%x\n", phdr->p_align);
}
void info_printer(Elf32_Phdr *phdr, int index) {
    printThetype(phdr);
    off_printer(phdr);
    vad_pad(phdr);
    sizes_print(phdr);
    printTheFlag(phdr);
    align_printer(phdr);
    print_appropriate_protection_flags(phdr);
}


void set_protection_flags(Elf32_Phdr *phdr, int *prot) {
    if (phdr->p_flags & PF_R)
        *prot |= PROT_READ;
    if (phdr->p_flags & PF_X)
        *prot |= PROT_EXEC;
    if (phdr->p_flags & PF_W)
        *prot |= PROT_WRITE;
}
void calculate_offset_and_padding(Elf32_Phdr *phdr, off_t *offset, size_t *padding) {
    *offset = phdr->p_offset & 0xfffff000;
    *padding = phdr->p_vaddr & 0xfff;
}
void load_phdr(Elf32_Phdr *phdr, int fd) {
    off_t offset;
    size_t padding;
    calculate_offset_and_padding(phdr, &offset, &padding);

    void* vaddr = (void*)(phdr->p_vaddr & 0xfffff000);
    size_t filesz = phdr->p_filesz;
    size_t memsz = phdr->p_memsz;
    int prot = 0;

    set_protection_flags(phdr, &prot);
    void *map = mmap(vaddr, memsz + padding, prot, MAP_PRIVATE | MAP_FIXED, fd, offset);
    if (map == MAP_FAILED) {
        perror("error mmap");
        exit(1);
    }
    info_printer(phdr, fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
    fprintf(stderr, "less args: %s\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("cant opening file");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error in file size");
        close(fd);
        return 1;
    }

    void *map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("Error in mapping file");
        close(fd);
        return 1;
    }

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + ehdr->e_phoff);
    printf("Type    Offset   VirtAddr   PhysAddr   FileSiz MemSiz Flg Align\n");

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            load_phdr(&phdr[i], fd);
            info_printer(&phdr[i], fd);
        }
    }
    //foreach_phdr(map_start, load_phdr, fd);
    close(fd);

    startup(argc - 1, argv + 1, (void *)(ehdr->e_entry));
    munmap(map_start, st.st_size);
}