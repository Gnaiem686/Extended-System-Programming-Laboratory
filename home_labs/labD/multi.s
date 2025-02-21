section .bss        ;initilize into memory
    inputbuff: resb 600
    struct: resd 1
section .data
    STATE: dw 0xACE1
    theGreaterSize: db 0
    Smaller: db 0
section .rodata     ;read only data
    Mask: dw 0x002D

    newlinestr: db 10,0
    text: db "%02hhx", 0
    x_struct: db 5
    x_num: db 0xaa, 1,2,0x44,0x4f

    y_struct: db 6
    y_num: db 0xaa, 1,2,3,0x44,0x4f
    Invalid: db "less or greater arguments are added -> %s", 10, 0


global main
extern printf
extern stdin
extern fgets
extern strlen
extern malloc
extern free


Clear_Stack:
  ;free the allocated memory
    call free
    add esp, 4
    call free
    add esp, 4
    call free
    add esp, 4
    ret

;///////////////////////////////    Task1a
print_multi:
    push ebp
    mov ebp, esp
    pushad
    mov esi, [ebp+8]
    xor ebx, ebx
    mov bl, byte[esi]

looperPrinter:
    cmp ebx, 0 
    jz newlinechar
    pushad                    
    mov al, byte[esi + ebx]     ;little endian
    push ebx
    push eax
    push text
    call printf
    add esp, 12
    popad
    dec ebx
    jmp looperPrinter

jmp newlinechar
jmp exit_func

;/////////////////////  Task1b
getmulti:
    push ebp
    mov ebp, esp
    pushad
    push dword[stdin]
    push 600
    push inputbuff
    call fgets
    add esp, 12
    push inputbuff
    call strlen
    add esp, 4
    mov edi, eax
    sub edi, 2
    shr eax, 1
    add eax, 1
    push eax
    call malloc
    mov dword[struct], eax      ;save to use as a return value
    mov esi, eax
    pop eax
    dec eax
    mov byte[esi], al           ;save the size of the array num
    mov ecx, 1

fgets_func:
    cmp edi, 0
    jl End            ;jump to End done
    xor ebx, ebx                  ;ebx
    mov bh, byte[inputbuff + edi]  ;the first char
    dec edi
    call first_char_convert     ;convert to hexa
    mov bl, bh
    xor bh, bh
    cmp edi, 0                  ;skip the next byte
    ;combine two chars into one byte,like "aa" to 0xaa
    jl combine_two
    mov bh, byte[inputbuff + edi]  ;second char
    dec edi
    call first_char_convert

combine_two:
    shl bh, 4
    or bl, bh

;struct build
    mov byte[esi + ecx], bl
    add ecx,1
    jmp fgets_func

first_char_convert:
    cmp bh, '9'
    ;it's a number
    jle numeric
    ; it's alphabet
    jmp letter
    

;/////////////////////////  task2a
Get_MaxMin:
    movzx ecx, byte[eax]            
    movzx edx, byte[ebx]            
    cmp edx, ecx                    
    jg swap_ptr                     ; Jump if the length in edx is greater
    ;ebx is greater or equal
    ret

    swap_ptr:
    xchg eax, ebx                   ; Swap the pointers
    ret                             ; returns eax as the greater


;////////////////// task2b
add_multi:
    push ebp
    mov ebp, esp
    pushad

    mov eax, [ebp+8]            ;first struct
    mov ebx, [ebp+12]           ;second struct
    call Get_MaxMin                 ;bring the bigger number
    
    push ebx
    call print_multi
    add esp, 4
    push eax
    call print_multi
    add esp, 4

    mov esi, eax                ;greater struct
    mov edi, ebx                ;smaller
    movzx eax, byte[edi]
    mov byte[Smaller], al
    movzx eax, byte[esi]
    mov byte[theGreaterSize], al
    add eax, 2                  ; add 2 size and extra byte
    push eax                    ;to malloc
    call malloc                 ;malloc new struct
    mov dword[struct], eax      ;save allocated address of the new struct
    pop ecx                     ;size of new struct
    dec ecx
    mov byte[eax], cl
    xor ecx, ecx
    mov edx, 0                  ;counter
    ;pointers to the next byte in array num
    inc esi
    inc edi
    inc eax
    
add_looper:
    movzx ebx, byte[esi]
    add ebx, ecx
    movzx ecx, byte[edi]
    add ebx, ecx
    mov cl, bh
    mov byte[eax], bl
    add edx,1                 ; inc counter
    ;initilize the pointers to the next    
    add esi,1
    add edi,1
    add eax,1
    cmp dl, byte[Smaller]
    jne add_looper

    cmp dl, byte[theGreaterSize]   ;sizes equal,nothing to append
    je jump_step
;inner function which like for into for in c/java
inner_add_looper:
    movzx ebx, byte[esi]
    add ebx, ecx
    mov cl, bh
    mov byte[eax], bl
    inc edx
    inc esi
    inc eax
    cmp dl, byte[theGreaterSize]
    jne inner_add_looper

jump_step:
    mov byte[eax], cl           ;carry
    jmp End
    
rand_num:
    push ebp
    mov ebp, esp
    pushad

    movzx eax, word[STATE]
    and ax, [Mask]
    mov bx, 0
parity:
    movzx ecx, ax
    and cx, 1
    xor bx, cx
    shr ax, 1
    jnz parity
    
    movzx eax, word[STATE]
    shr ax, 1
    shl bx, 15
    or ax, bx
    mov word[STATE], ax
    
    popad
    pop ebp
    movzx eax, word[STATE]
    ret

PRmulti:
    push ebp
    mov ebp, esp
    pushad

Non_0_loop:
    call rand_num
    cmp al, 0
    je Non_0_loop

    movzx ebx, al
    add ebx, 1
    push ebx
    call malloc
    mov dword[struct], eax
    pop ebx
    dec ebx
    mov byte[eax], bl
    mov esi, eax
    mov edx, 0
rand_num_loop:
    call rand_num
    mov byte[esi + edx + 1], al
    inc edx
    dec ebx
    jnz rand_num_loop
    jmp End

main:
    mov eax, [esp+4]
    mov ebx, [esp+8]
    cmp eax, 1 ;if we have just one its multi just
    je Normal_print_no_args
    jmp check_args
    jmp exit_func

check_args:
    mov eax, [ebx + 4]
    ;comparing if the argument -I/ -R
    cmp word[eax], "-R"     
    je flagR
    cmp word[eax], "-I"
    je flagI
    push eax
    push Invalid
    call printf
    
flagR:
    call PRmulti
    push eax
    call PRmulti
    push eax
    call add_multi
    push eax
    call print_multi
    jmp Clear_Stack

flagI:
    call getmulti
    push eax
    call getmulti
    push eax
    call add_multi
    push eax
    call print_multi
    jmp Clear_Stack

Normal_print_no_args:
    push y_struct           
    push x_struct
    call add_multi
    push eax
    call print_multi
    call free
    add esp, 12
    ret

letter:
    sub bh, 'a'
    add bh, 0xa
    ret

numeric:
    sub bh, '0'                 ;return as number
    ret                         

End:
    popad
    mov eax, dword[struct]
    pop ebp
    ret

;new line
newlinechar:
    push newlinestr
    call printf
    add esp, 4

exit_func:
    popad
    pop ebp
    ret