section .text
    global update_ai_paddle

update_ai_paddle:
    push rbp
    mov rbp, rsp

    ; Load paddle y position
    mov rax, rdi       ; rdi contains pointer to y
    movss xmm2, [rax]  ; Load *y into xmm2

    ; Compare ball_y - 5 with y
    movss xmm3, xmm1    ; Copy ball_y to xmm3
    subss xmm3, dword [five]  ; ball_y - 5
    ucomiss xmm3, xmm2  ; Compare (ball_y - 5) with y
    jbe .move_up      ; If (ball_y - 5) <= y, move up

    movss xmm3, xmm1  ; Copy ball_y to xmm3
    addss xmm3, dword [one_hundred_ten] ; ball_y + 110
    ucomiss xmm3, xmm2  ; Compare (ball_y - 5) with y
    jae .move_down  ; If (ball_y + 110) >= y, move down
.move_up:
    ; Move up
    subss xmm2, xmm0   ; y -= speed
    jmp .store_y

.move_down:
    addss xmm2, xmm0   ; y += speed

.store_y:
    movss [rax], xmm2  ; Store updated y back to memory

    xor rax, rax
    leave
    ret

section .data
    five dd 5.0
    one_hundred_ten dd 110.0         

section .note.GNU-stack
