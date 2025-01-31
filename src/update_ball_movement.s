section .text
    global update_ball_movement

update_ball_movement:
    ; Parameters (System V ABI)
    ; rdi -> y (pointer, float)
    ; rsi -> time (pointer, float)
    ; edx -> range (integer)
    ; ecx -> mode (integer)
    ; xmm0 -> gravity (float)
    ; r8 -> y_speed (pointer, float)
    ; r9 -> angle (pointer, float)
    ; xmm1 -> spin (float)
    push rbp
    mov rbp, rsp
    sub rsp, 16  ; Allocate space for temporary float storage

    cmp ecx, 1
    jne .curve_check 
    ; Load time into FPU stack
    fld dword [rsi]     ; ST0 = time
    fsin                ; ST0 = cos(time)
    
    ; Convert range (integer) to float
    cvtsi2ss xmm0, edx  ; xmm0 = (float)range
    movss [rsp], xmm0   ; Store the float range in memory

    ; Multiply cos(time) * range
    fmul dword [rsp]    ; ST0 = cos(time) * range

    ; Load y and add the result
    fadd dword [rdi]    ; ST0 = y + cos(time) * range
    fstp dword [rdi]    ; Store back into y and pop FPU stack

    ; Update time: time += 5.0 / range
    mov eax, 5
    cvtsi2ss xmm1, eax  ; xmm1 = 5.0
    divss xmm1, [rsp]   ; xmm1 = 5.0 / range
    addss xmm1, [rsi]   ; time += (5.0 / range)
    movss [rsi], xmm1   ; Store updated time
    jmp .done
.curve_check:
    cmp ecx, 2
    jne .done
    movss xmm2, [r8]   ; Load y_speed
    addss xmm2, xmm0   ; Add gravity
    movss [r8], xmm2


.done:
    movss xmm2, [r9]   ; Load angle
    addss xmm2, xmm1   ; Add spin
    movss [r9], xmm2
    xor rax, rax
    add rsp, 16  ; Restore stack
    leave
    ret

section .note.GNU-stack
