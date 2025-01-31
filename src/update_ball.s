section .text
    global update_ball

update_ball:
    ; Function parameters (System V ABI - Linux)
    ; rdi -> x pointer
    ; rsi -> y pointer
    ; rdx -> x_speed pointer
    ; rcx -> y_speed pointer
    ; r8  -> radius (float, passed in xmm0)
    ; r9  -> spin pointer

    push rbp
    mov rbp, rsp

    ; Load values (floats)
    movss xmm1, [rdi]  ; xmm1 = x
    movss xmm2, [rsi]  ; xmm2 = y
    movss xmm3, [rdx]  ; xmm3 = x_speed
    movss xmm4, [rcx]  ; xmm4 = y_speed
    movss xmm5, [r9]   ; xmm5 = spin
    movss xmm6, xmm0   ; xmm6 = radius (float)

    ; x += x_speed
    addss xmm1, xmm3
    ; y += y_speed
    addss xmm2, xmm4

    ; Check X boundary collision
    mov eax, 1200
    cvtsi2ss xmm7, eax   ; xmm7 = 1200.0 (SCREEN_WIDTH)
    subss xmm7, xmm6     ; xmm7 = SCREEN_WIDTH - radius

    ucomiss xmm1, xmm7   ; Compare x with SCREEN_WIDTH - radius
    jae flip_x_speed
    ucomiss xmm1, xmm6   ; Compare x with radius
    jbe flip_x_speed
    jmp check_y

flip_x_speed:
    mulss xmm3, dword [neg_one]  ; x_speed *= -1.0
    ucomiss xmm3, dword [zero]
    jge sub_spin_x
    addss xmm5, dword [two]
    jmp store_x

sub_spin_x:
    subss xmm5, dword [two]

store_x:
    movss [rdx], xmm3  ; Store new x_speed
    jmp check_y

check_y:
    mov eax, 800
    cvtsi2ss xmm7, eax   ; xmm7 = 800.0 (SCREEN_HEIGHT)
    subss xmm7, xmm6     ; xmm7 = SCREEN_HEIGHT - radius

    ucomiss xmm2, xmm7   ; Compare y with SCREEN_HEIGHT - radius
    jae flip_y_speed
    ucomiss xmm2, xmm6   ; Compare y with radius
    jbe flip_y_speed
    jmp store_values

flip_y_speed:
    mulss xmm4, dword [neg_one]  ; y_speed *= -1.0
    ucomiss xmm3, dword [zero]
    jge sub_spin_y
    addss xmm5, dword [two]
    jmp store_y

sub_spin_y:
    subss xmm5, dword [two]

store_y:
    ucomiss xmm2, xmm7
    jae set_y_max
    ucomiss xmm2, xmm6
    jbe set_y_min
    jmp store_values

set_y_max:
    movss xmm2, xmm7
    jmp store_values

set_y_min:
    movss xmm2, xmm6

store_values:
    movss [rdi], xmm1  ; Store new x
    movss [rsi], xmm2  ; Store new y
    movss [rcx], xmm4  ; Store new y_speed
    movss [r9], xmm5   ; Store new spin

    xor rax, rax
    leave
    ret

section .data
neg_one dd -1.0
zero    dd  0.0
two     dd  2.0

section	.note.GNU-stack

