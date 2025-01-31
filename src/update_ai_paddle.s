section .text
global update_ai_paddle

update_ai_paddle:
    ; Arguments:
    ; rdi -> pointer to AI Paddle y position
    ; rsi -> AI Paddle speed
    ; rdx -> Ball y position
    push rbp
    mov rbp, rsp

    movss xmm0, [rdx]   ; Load ball.y into xmm0
    subss xmm0, dword [rel five]  ; ball.y - 5
    ucomiss xmm0, [rdi] ; Compare (ball.y - 5) with y
    jg .check_lower     ; If greater, check lower condition

    movss xmm3, [rdi]    ; Load y into xmm3
    subss xmm3, xmm1     ; y -= speed
    movss [r9], xmm3  

    jmp .done

.check_lower:
    movss xmm0, [rdx]   ; Load ball.y again
    addss xmm0, dword [rel one_ten] ; ball.y + 110
    ucomiss xmm0, [rdi] ; Compare (ball.y + 110) with y
    jl .done            ; If less, do nothing

    movss xmm3, [rdi]    ; Load y into xmm3
    addss xmm3, xmm1   ; y += speed
    movss [rdi], xmm3  

.done:
    xor rax, rax
    leave
    ret

section .data
five: dd 5.0
one_ten: dd 110.0

section	.note.GNU-stack