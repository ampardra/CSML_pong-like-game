section .text
global collision_with_ball

collision_with_ball:
    ; Arguments:
    ; rdi -> pointer to ball.x_speed
    ; rsi -> pointer to ball.y_speed
    ; rdx -> pointer to ball.spin
    ; rcx -> Paddle volacity
    ; r8  -> Paddle acceleration
    push rbp
    mov rbp, rsp

    movss xmm0, dword [rdi]   ; Load ball.x_speed
    mulss xmm0, dword [rel neg_one] ; Multiply by -1
    movss [rdi], xmm0         ; Store back

    movss xmm0, [rcx]   ; Load volacity
    ucomiss xmm0, dword [rel zero] ; Compare with 0
    jle .check_negative ; If less, check the negative condition

    ; Case: volacity > 0
    movss xmm0, [r8]    ; Load acceleration
    subss xmm0, dword [rel zero_point_three] ; acceleration - 0.3
    movss xmm1, [rsi]   ; Load ball.y_speed in xmm1
    addss xmm1, xmm0   ; ball.y_speed += (acceleration - 0.3)
    movss [rsi], xmm1

    mulss xmm0, dword [rel ten] ; (acceleration - 0.3) * 10
    movss xmm1, [rdx]   ; Load ball.spin in xmm1
    subss xmm1, xmm0   ; ball.spin -= (acceleration * 10)
    movss [rdx], xmm1

    jmp .done

.check_negative:
    ucomiss xmm0, dword [rel zero] ; Check if still negative
    jge .done ; If zero, exit

    movss xmm0, [r8]    ; Load acceleration
    subss xmm0, dword [rel zero_point_three] ; acceleration - 0.3
    movss xmm1, [rsi]   ; Load ball.y_speed in xmm1
    subss xmm1, xmm0   ; ball.y_speed -= (acceleration - 0.3)
    movss [rsi], xmm1

    mulss xmm0, dword [rel ten] ; (acceleration - 0.3) * 10
    movss xmm1, [rdx]   ; Load ball.spin in xmm1
    addss xmm1, xmm0   ; ball.spin += (acceleration * 10)
    movss [rdx], xmm1
.done:
    xor rax, rax
    leave
    ret

section .data
neg_one: dd -1.0
zero: dd 0.0
zero_point_three: dd 0.3
ten: dd 10.0

section	.note.GNU-stack