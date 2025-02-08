section .text
    global update_ball

update_ball:
    ; Function parameters (System V ABI - Linux)
    ; rdi -> pointer to x (float)
    ; rsi -> pointer to y (float)
    ; rdx -> pointer to x_speed (float)
    ; rcx -> pointer to y_speed (float)
    ; r8  -> radius (float, passed in xmm0)
    ; r9  -> pointer to spin (float)

    push    rbp
    mov     rbp, rsp

    ; ----------------------------------------------------------
    ; Preload constants into registers:
    ; SCREEN_WIDTH (1200) and SCREEN_HEIGHT (800)
    mov     eax, 1200
    cvtsi2ss xmm7, eax       ; xmm7 = 1200.0
    mov     eax, 800
    cvtsi2ss xmm8, eax       ; xmm8 = 800.0

    ; Load constant values from memory into SSE registers.
    movss   xmm10, dword [neg_one]  ; xmm10 = -1.0
    movss   xmm11, dword [two]      ; xmm11 = 2.0
    movss   xmm12, dword [zero]     ; xmm12 = 0.0

    ; ----------------------------------------------------------
    ; Load function parameters (floats) into registers.
    movss   xmm1, [rdi]       ; xmm1 = x
    movss   xmm2, [rsi]       ; xmm2 = y
    movss   xmm3, [rdx]       ; xmm3 = x_speed
    movss   xmm4, [rcx]       ; xmm4 = y_speed
    movss   xmm5, [r9]        ; xmm5 = spin
    movss   xmm6, xmm0        ; xmm6 = radius

    ; ----------------------------------------------------------
    ; Compute boundary limits.
    ; x_bound = SCREEN_WIDTH - radius
    movaps  xmm13, xmm7       ; xmm13 = 1200.0
    subss   xmm13, xmm6       ; xmm13 = SCREEN_WIDTH - radius
    ; y_bound = SCREEN_HEIGHT - radius
    movaps  xmm14, xmm8       ; xmm14 = 800.0
    subss   xmm14, xmm6       ; xmm14 = SCREEN_HEIGHT - radius

    ; ----------------------------------------------------------
    ; Update positions:
    addss   xmm1, xmm3        ; x = x + x_speed
    addss   xmm2, xmm4        ; y = y + y_speed

    ; ----------------------------------------------------------
    ; Check for X-axis collisions.
    ucomiss xmm1, xmm13       ; compare x with (SCREEN_WIDTH - radius)
    jae     .flip_x         ; if x >= bound, flip x_speed
    ucomiss xmm1, xmm6        ; compare x with radius (left bound)
    jbe     .flip_x
    jmp     .check_y

.flip_x:
    ; Flip x_speed:  x_speed *= -1.0
    mulss   xmm3, xmm10       ; xmm10 = -1.0, so xmm3 = -x_speed

    ; Adjust spin based on the sign of the new x_speed.
    ucomiss xmm3, xmm12       ; compare x_speed with 0
    jge     .sub_spin_x     ; if x_speed >= 0, subtract spin adjustment
    addss   xmm5, xmm11       ; spin = spin + 2.0
    jmp     .store_x

.sub_spin_x:
    subss   xmm5, xmm11       ; spin = spin - 2.0

.store_x:
    movss   [rdx], xmm3       ; store new x_speed

    ; ----------------------------------------------------------
.check_y:
    ; Check for Y-axis collisions.
    ucomiss xmm2, xmm14       ; compare y with (SCREEN_HEIGHT - radius)
    jae     .flip_y
    ucomiss xmm2, xmm6        ; compare y with radius (lower bound)
    jbe     .flip_y
    jmp     .store_values

.flip_y:
    ; Flip y_speed:  y_speed *= -1.0
    mulss   xmm4, xmm10       ; xmm4 = -y_speed

    ; Adjust spin based on the sign of x_speed (as in original code).
    ucomiss xmm3, xmm12       ; compare x_speed with 0
    jge     .sub_spin_y
    addss   xmm5, xmm11       ; spin = spin + 2.0
    jmp     .store_y

.sub_spin_y:
    subss   xmm5, xmm11       ; spin = spin - 2.0

.store_y:
    ; Clamp y to boundaries if needed.
    ucomiss xmm2, xmm14       ; if y >= (SCREEN_HEIGHT - radius)
    jae     .set_y_max
    ucomiss xmm2, xmm6        ; if y <= radius
    jbe     .set_y_min
    jmp     .store_values

.set_y_max:
    movaps  xmm2, xmm14       ; y = SCREEN_HEIGHT - radius
    jmp     .store_values

.set_y_min:
    movaps  xmm2, xmm6        ; y = radius

    ; ----------------------------------------------------------
.store_values:
    movss   [rdi], xmm1       ; store new x
    movss   [rsi], xmm2       ; store new y
    movss   [rcx], xmm4       ; store new y_speed
    movss   [r9], xmm5        ; store new spin

    xor     rax, rax
    pop     rbp
    ret

; ----------------------------------------------------------
section .data
neg_one dd -1.0
zero    dd  0.0
two     dd  2.0

section	.note.GNU-stack

