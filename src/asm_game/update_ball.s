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

    ; Preload constants into registers
    movss   xmm7, dword [neg_one]  ; xmm7 = -1.0
    movss   xmm8, dword [two]      ; xmm8 = 2.0
    movss   xmm9, dword [zero]     ; xmm9 = 0.0

    ; Load function parameters (floats) into registers
    movss   xmm1, [rdi]       ; xmm1 = x
    movss   xmm2, [rsi]       ; xmm2 = y
    movss   xmm3, [rdx]       ; xmm3 = x_speed
    movss   xmm4, [rcx]       ; xmm4 = y_speed
    movss   xmm5, [r9]        ; xmm5 = spin
    movss   xmm6, xmm0        ; xmm6 = radius

    ; Compute boundary limits
    mov     eax, 1200
    cvtsi2ss xmm10, eax       ; xmm10 = 1200.0
    subss   xmm10, xmm6       ; xmm10 = SCREEN_WIDTH - radius
    mov     eax, 800
    cvtsi2ss xmm11, eax       ; xmm11 = 800.0
    subss   xmm11, xmm6       ; xmm11 = SCREEN_HEIGHT - radius

    ; ------------------------------------------------------
    ; Update positions using SIMD parallel addition:
    ;
    ; At this point:
    ;   xmm1 = x        (from [rdi])
    ;   xmm2 = y        (from [rsi])
    ;   xmm3 = x_speed  (from [rdx])
    ;   xmm4 = y_speed  (from [rcx])
    ;
    ; We want to compute:
    ;   new_x = x + x_speed
    ;   new_y = y + y_speed
    ;
    ; Since x and y (and the speeds) reside at separate addresses,
    ; we pack them into a single register first.
    ; ------------------------------------------------------
    unpcklps xmm1, xmm2       ; xmm1 = { x, y, undefined, undefined }
    unpcklps xmm3, xmm4       ; xmm3 = { x_speed, y_speed, undefined, undefined }
    addps   xmm1, xmm3        ; xmm1 = { x+x_speed, y+y_speed, undefined, undefined }
    ;
    ; Now the updated x is in xmm1[0] and the updated y is in xmm1[1].
    ; To use y in later comparisons (and for storing), we move the y value
    ; into xmm2. (Using pshufd with immediate 0x55 copies element 1 into all slots.)
    pshufd  xmm2, xmm1, 0x55   ; xmm2 = { y+y_speed, y+y_speed, y+y_speed, y+y_speed }

    ; ------------------------------------------------------
    ; Continue with collision detection and further updates.
    ; (x in xmm1[0] and y in xmm2[0] are used in comparisons.)
    ; ------------------------------------------------------
    
    ; Check for X-axis collisions
    ucomiss xmm1, xmm10       ; compare x (xmm1[0]) with (SCREEN_WIDTH - radius)
    jae     .flip_x           ; if x >= bound, flip x_speed
    ucomiss xmm1, xmm6        ; compare x with radius (left bound)
    jbe     .flip_x
    jmp     .check_y

.flip_x:
    ; Flip x_speed: x_speed *= -1.0
    mulss   xmm3, xmm7        ; xmm3 = -x_speed

    ; Adjust spin based on the sign of the new x_speed
    ucomiss xmm3, xmm9        ; compare x_speed with 0
    jge     .sub_spin_x       ; if x_speed >= 0, subtract spin adjustment
    addss   xmm5, xmm8        ; spin = spin + 2.0
    jmp     .store_x

.sub_spin_x:
    subss   xmm5, xmm8        ; spin = spin - 2.0

.store_x:
    movss   [rdx], xmm3       ; store new x_speed

.check_y:
    ; Check for Y-axis collisions
    ucomiss xmm2, xmm11       ; compare y (xmm2[0]) with (SCREEN_HEIGHT - radius)
    jae     .flip_y
    ucomiss xmm2, xmm6        ; compare y with radius (lower bound)
    jbe     .flip_y
    jmp     .store_values

.flip_y:
    ; Flip y_speed: y_speed *= -1.0
    mulss   xmm4, xmm7        ; xmm4 = -y_speed

    ; Adjust spin based on the sign of x_speed
    ucomiss xmm3, xmm9        ; compare x_speed with 0
    jge     .sub_spin_y
    addss   xmm5, xmm8        ; spin = spin + 2.0
    jmp     .store_y

.sub_spin_y:
    subss   xmm5, xmm8        ; spin = spin - 2.0

.store_y:
    ; Clamp y to boundaries if needed
    ucomiss xmm2, xmm11       ; if y >= (SCREEN_HEIGHT - radius)
    jae     .set_y_max
    ucomiss xmm2, xmm6        ; if y <= radius
    jbe     .set_y_min
    jmp     .store_values

.set_y_max:
    movaps  xmm2, xmm11       ; y = SCREEN_HEIGHT - radius
    jmp     .store_values

.set_y_min:
    movaps  xmm2, xmm6        ; y = radius

.store_values:
    ; Store updated values:
    ; Updated x is in xmm1[0] and updated y is in xmm2[0]
    movss   [rdi], xmm1       ; store new x
    movss   [rsi], xmm2       ; store new y
    movss   [rcx], xmm4       ; store new y_speed
    movss   [r9], xmm5        ; store new spin

    xor     rax, rax          ; return 0
    ret

; ----------------------------------------------------------
section .data
neg_one dd -1.0
zero    dd  0.0
two     dd  2.0
 
section .note.GNU-stack
