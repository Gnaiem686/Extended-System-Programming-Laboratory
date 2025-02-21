section .data
    textarg db "the arguments of argc is %d", 10,0    
    newline db "%s", 10

section .text
global main
extern printf, puts

main:

    mov edi, dword[esp+4]
    push edi
    push textarg
    call printf
    add esp, 8
    ;prepare to print args
    mov esi, dword[esp + 4]
    inc esi
    mov edx, dword[esp + 8]
    mov edi, 0

args_looper:
    cmp edi, esi
    jge exit_finish

    mov ebx, edx           ; ebx = argv
    mov edx, edi           ; edx = i
    shl edx, 2             ; multiply i by 4 to get the offset in bytes
    add ebx, edx           ; ebx = argv[i]

    push edi               ; push i as the argument for printf
    push dword [ebx]       ; push argv[i] as the argument for printf
    push newline           ; push newline format string as the argument for printf
    call printf            ; call printf
    add esp, 12            ; clean up the stack after the call

    inc edi                ; increment loop counter
    jmp args_looper        ; jump back to the start of the loop


exit_finish:
    xor edi, edi
    mov ebx, 0x1
    int 0x80
