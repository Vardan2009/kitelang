section .data
    datasec_0 db "Hello, World!", 0
section .text
    global _start
    print:
    push rdi
    push rdi
    mov rax, 1
    mov rdi, 1
    .kprint_loop:
        mov rsi, [rsp]
        movzx rdx, byte [rsi]
        test rdx, rdx
        jz .kprint_done
        mov rax, 1
        mov rdi, 1
        mov rdx, 1
        syscall
        inc rsi
        mov [rsp], rsi
        jmp .kprint_loop
    .kprint_done:
        pop rdi
    print_end:
    add rsp, 0
    add rsp, 8
    ret
    printc:
    movzx rdi, dil
    push rdi
    push rdi
    mov rax, 1
    mov rdi, 1
    lea rsi, [rsp]
    mov rdx, 1
    syscall
    pop rdi
    printc_end:
    add rsp, 0
    add rsp, 8
    ret
    printi:
    push rdi
    mov rbp, 0
    .convert_loop:
    mov rax, rdi
    mov rbx, 10
    xor rdx, rdx
    div rbx
    push rdx
    inc rbp
    mov rdi, rax
    cmp rdi, 0
    je .convert_loop_end
    jmp .convert_loop
    .convert_loop_end:
    .print_loop:
    pop rdi
    add rdi, '0'
    call printc
    dec rbp
    test rbp, rbp
    jz .print_end
    jmp .print_loop
    .print_end:
    printi_end:
    add rsp, 0
    add rsp, 8
    ret
    readc:
    sub rsp, 8
    mov rdi, rsp
    mov rax, 0
    mov rdi, 0
    mov rsi, rsp
    mov rdx, 1
    syscall
    movzx rax, byte [rsp]
    readc_end:
    add rsp, 8
    ret
    readln:
    push rdi
    push rsi
    mov al, 0
    movzx rax, al
    push rax
    mov rax, 0
    mov rax, [rsp + 8]
    mov rbx, 1
    sub rax, rbx
    mov rcx, rax
    mov rbx, [rsp + 16]
    imul rcx, rcx, 1
    add rbx, rcx
    mov [rbx], rax
    mov rax, 0
    push rax
    loop_0:
    call readc
    mov [rsp + 8], al
    mov rax, [rsp + 8]
    mov rcx, [rsp + 0]
    mov rbx, [rsp + 24]
    imul rcx, rcx, 1
    add rbx, rcx
    mov [rbx], rax
    mov rax, [rsp + 8]
    push rax
    mov rbx, 10
    pop rax
    cmp rax, rbx
    je boolop_true_2
    mov rax, 0
    jmp boolop_end_2
    boolop_true_2:
    mov rax, 1
    boolop_end_2:
    mov rax, rax
    cmp rax, 0
    jne .if_true_1
    jmp .if_end_1
    .if_true_1:
    mov rax, 0
    mov rcx, [rsp + 0]
    mov rbx, [rsp + 24]
    imul rcx, rcx, 1
    add rbx, rcx
    mov [rbx], rax
    jmp loop_end_0
    add rsp, 0
    .if_end_1:
    add rsp, 0
    mov rax, 1
    add [rsp + 0], rax
    mov rax, [rsp + 16]
    mov rbx, 2
    sub rax, rbx
    mov rax, rax
    cmp [rsp + 0], rax
    jg loop_end_0
    jmp loop_0
    loop_end_0: 
    add rsp, 8
    readln_end:
    add rsp, 8
    add rsp, 8
    add rsp, 8
    ret
    _start:
    mov rax, datasec_0
    push rax
    pop rdi
    call print
    mov al, 0
    jmp _start_end
    _start_end:
    add rsp, 0
    mov rdi, rax
    mov rax, 60
    syscall
    ret
