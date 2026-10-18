.intel_syntax noprefix
.section .note.GNU-stack, "", @progbits

.text

.global most_freq


most_freq:
    enter 1024, 0   

    mov r10, rsp 

    mov ecx, 0
zero_loop:
    cmp ecx, 256
    jge zero_loop_end

        mov dword ptr [r10 + 4 * rcx], 0

    inc ecx
    jmp zero_loop
zero_loop_end:

    mov ecx, 0
count_loop:
    mov r8b, [rdi + rcx]
    cmp r8b, 0
    je count_loop_end
        movzx r8, r8b
        add dword ptr [r10 + 4 * r8], 1
    
    inc ecx
    jmp count_loop
count_loop_end:


    mov eax, 0  
    mov r9d, 0  
    mov ecx, 0  
max_loop:
    cmp ecx, 256
    jge max_loop_end

        cmp r9d, [r10 + 4 * rcx]
        jae nije_vece
            mov al, cl
            mov r9d, [r10 + 4 * rcx]
    nije_vece:

    inc ecx
    jmp max_loop
max_loop_end:

    leave
    ret
