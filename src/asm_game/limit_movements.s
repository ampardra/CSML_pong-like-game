section .data
    zero:          dd 0.0           ; 0.0 as a float
    five:          dd 5.0           ; 5.0 as a float (new speed)
    one_twenty:    dd 120.0         ; 120.0 as a float
    screen_height: dd 800.0         ; SCREEN_HEIGHT (800.0)

section .text
    global limit_movements

; limit_movements expects:
;   rdi = pointer to y (float)
;   rsi = pointer to speed (float)
limit_movements:
    push rbp
    mov rbp, rsp

    ; Load y into xmm0
    movss xmm0, [rdi]
    ; Check if y < 0.0
    ucomiss xmm0, dword [zero]
    jb .set_lower         ; Jump if y is below 0

    ; Otherwise, compute y + 120.0 in xmm1
    movss xmm1, xmm0
    addss xmm1, dword [one_twenty]
    ; Check if (y + 120) > SCREEN_HEIGHT
    ucomiss xmm1, dword [screen_height]
    ja .set_upper         ; Jump if y + 120 is above SCREEN_HEIGHT
    jmp .done

.set_lower:
    ; y < 0: set y = 0 and speed = 5
    movss xmm0, dword [zero]
    movss [rdi], xmm0     ; y = 0.0
    movss xmm0, dword [five]
    movss [rsi], xmm0     ; speed = 5.0
    jmp .done

.set_upper:
    ; y + 120 > SCREEN_HEIGHT:
    ; Compute y = SCREEN_HEIGHT - 120
    movss xmm0, dword [screen_height]
    subss xmm0, dword [one_twenty]
    movss [rdi], xmm0     ; y = SCREEN_HEIGHT - 120
    movss xmm0, dword [five]
    movss [rsi], xmm0     ; speed = 5.0

.done:
    xor rax, rax
    leave
    ret

section .note.GNU-stack
