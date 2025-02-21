
section .data
    len: dd 0
    argc: dd 0
    argv: dd 0
    index: dd 0
    new_line dd 10
    current_arg: dd 0
    Outfile: dd 1
    Infile: dd 0
    letter: dd 0
section .text
global main
extern strlen

main:
    mov eax, [esp+4]       
    mov [argc], eax 
    mov [index], eax
    mov eax, [esp+8]       
    mov [argv], eax        
           
    
loop_on_args:
    mov ecx, [argv]            
    mov edi, [current_arg]
    mov ecx, [ecx + edi]   
    
    push ecx                        
    call strlen                     
    add esp, 4                      
    mov [len], eax        
    ;system call
    mov eax, 4                   
    mov ebx, 1                      
    add dword[current_arg],4 
    mov ecx, [argv]            
    mov ecx, [ecx + edi]   
    mov edx, [len]        
    int 0x80                        
    ;print_new_line:
    mov eax, 0x4                    
    mov ebx, 1                      
    mov ecx, new_line               
    mov edx, 1                      
    int 0x80                        

    mov edi, [current_arg]
    mov ecx, dword[argv]            
    mov ecx, dword[ecx + edi]  
    cmp word[ecx], "-o"             
    je outputfile
    cmp word[ecx], "-i"             
    je inputfile               
    

    dec dword[index]
    mov edi, [index]         
    cmp edi, 0            
    jne loop_on_args                  ;if not equal, jump
                  

    exit:
    mov eax, 0x1                   
    mov ebx, 0                      
    int 0x80


inputfile:
    mov eax, 0x5                    
    mov ebx, ecx                    
    add ebx, 2                      
    mov ecx, 0                      
    int 0x80                        
    cmp eax, -1                     
    mov dword[Infile], eax          
    jmp loop_on_args              
outputfile:
    mov eax, 0x5                    
    mov ebx, ecx                   
    add ebx, 2                      
    mov ecx, 101o                   
    mov edx, 777o                  
    int 0x80                        
    cmp eax, -1                                   
    mov dword[Outfile], eax         
    jmp loop_on_args  
    
encode:
    mov eax, 3                
    mov ebx, dword[Infile]      
    mov ecx, letter           
    mov edx, 1                 
    int 0x80 

    ;if error               
    cmp eax, 0                 
    jle exit 

    cmp byte[letter], 'z'       
    jg decode_char
    cmp byte[letter], 'A'       
    jl decode_char              
    add byte[letter], 1 
           
decode_char:
    mov eax, 4                
    mov ebx, dword[Outfile]   
    mov ecx, letter             
    mov edx, 1                  
    int 0x80                    
    jmp encode  

