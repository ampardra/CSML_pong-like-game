section .text
global update_paddle_movement

extern IsKeyDown  ; Declare external function
extern KEY_UP
extern KEY_DOWN

update_paddle_movement:
    push rbp
    mov rbp, rsp

    ; Check if KEY_UP is pressed
    mov edi, dword [rel KEY_UP]  ; Load KEY_UP constant
    call IsKeyDown               ; Call IsKeyDown(KEY_UP)
    test eax, eax
    jz .check_down               ; If not pressed, check KEY_DOWN

    ; velocity -= speed
    movss xmm0, [rdi]    ; Load velocity
    movss xmm1, [rdx]    ; Load speed
    subss xmm0, xmm1     ; velocity -= speed
    movss [rdi], xmm0    ; Store new velocity

    ; y += velocity
    movss xmm3, [rsi]    ; Load y
    addss xmm3, xmm0     ; y += velocity
    movss [rsi], xmm3    ; Store new y

    ; speed += acceleration
    movss xmm1, [rdx]    ; Reload speed
    movss xmm2, [rcx]    ; Load acceleration
    addss xmm1, xmm2     ; speed += acceleration
    movss [rdx], xmm1    ; Store new speed

    ; acceleration += 0.05
    movss xmm3, dword [rel point_zero_five]  ; Load 0.05
    addss xmm2, xmm3                         ; acceleration += 0.05
    movss [rcx], xmm2                         ; Store new acceleration
    jmp .done

.check_down:
    mov edi, dword [rel KEY_DOWN] ; Load KEY_DOWN constant
    call IsKeyDown                ; Call IsKeyDown(KEY_DOWN)
    test eax, eax
    jz .reset                     ; If not pressed, reset speed and acceleration

    ; velocity += speed
    movss xmm0, [rdi]    ; Load velocity
    movss xmm1, [rdx]    ; Load speed
    addss xmm0, xmm1     ; velocity += speed
    movss [rdi], xmm0    ; Store new velocity

    ; y += velocity
    movss xmm3, [rsi]    ; Load y
    addss xmm3, xmm0     ; y += velocity
    movss [rsi], xmm3    ; Store new y

    ; speed += acceleration
    movss xmm1, [rdx]    ; Reload speed
    movss xmm2, [rcx]    ; Load acceleration
    addss xmm1, xmm2     ; speed += acceleration
    movss [rdx], xmm1    ; Store new speed

    ; acceleration += 0.05
    movss xmm3, dword [rel point_zero_five]  ; Load 0.05
    addss xmm2, xmm3                         ; acceleration += 0.05
    movss [rcx], xmm2                         ; Store new acceleration
    jmp .done

.reset:
    ; speed = 5
    movss xmm0, dword [rel five]
    movss [rdx], xmm0

    ; acceleration = 0.3
    movss xmm1, dword [rel point_three]
    movss [rcx], xmm1

    ; velocity = 0
    xorps xmm2, xmm2
    movss [rdi], xmm2

.done:
    leave
    ret

section .data
five: dd 5.0
point_three: dd 0.3
point_zero_five: dd 0.05

section	.note.GNU-stack