section .text
global limit_movements

limit_movements:
    ; Arguments:
    ; rdi -> pointer to Paddle y position
    ; rsi -> pointer to Paddle speed
    push rbp
    mov rbp, rsp

    movss xmm0, [rdi]   ; Load y
    ucomiss xmm0, dword [rel zero] ; Compare y < 0
    jae .check_upper ; If not less, check upper limit

    movss dword [rdi], xmm1  ; y = 0
    movss xmm1, dword [rel five]
    movss dword [rsi], xmm1 ; speed = 5
    jmp .done

.check_upper:
    movss xmm0, [rdi]   ; Load y
    addss xmm0, dword [rel one_twenty] ; y + 120
    ucomiss xmm0, dword [rel screen_height] ; Compare y + 120 > SCREEN_HEIGHT
    jbe .done ; If not greater, exit

    movss xmm0, dword [rel screen_height] ; SCREEN_HEIGHT
    subss xmm0, dword [rel one_twenty] ; SCREEN_HEIGHT - 120
    movss [rdi], xmm0 ; y = SCREEN_HEIGHT - 120
    movss xmm1, dword [rel five]
    movss dword [rsi], xmm1 ; speed = 5
.done:
    xor rax, rax
    leave
    ret

section .data
zero: dd 0.0
five: dd 5.0
one_twenty: dd 120.0
screen_height: dd 800.0

section	.note.GNU-stack
