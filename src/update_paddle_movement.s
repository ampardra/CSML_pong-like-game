section .data
    ; Key codes (using assumed Raylib/GLFW values)
    KEY_UP   equ 265
    KEY_DOWN equ 264

    ; Floating-point constants
    point_zero:       dd 0.0
    point_zero_five:  dd 0.05
    five:             dd 5.0
    point_three:      dd 0.3

section .text
    global update_paddle_movement
    extern IsKeyDown       ; Prototype: int IsKeyDown(int key)

; update_paddle_movement expects:
;   rdi = pointer to volacity (float)
;   rsi = pointer to y (float)
;   rdx = pointer to speed (float)
;   rcx = pointer to acceleration (float)
update_paddle_movement:
    push rbp
    mov rbp, rsp

    ; Preserve incoming pointers in registers:
    mov r8, rdi      ; r8 = pointer to volacity
    mov r9, rsi      ; r9 = pointer to y
    mov r10, rdx     ; r10 = pointer to speed
    mov r11, rcx     ; r11 = pointer to acceleration

    ; ------------------------------
    ; Check if KEY_UP is pressed
    ; ------------------------------
    mov edi, KEY_UP       ; Load KEY_UP (265) into edi
    call IsKeyDown        ; Call IsKeyDown(KEY_UP)
    test eax, eax
    jnz .key_up_pressed   ; Jump if KEY_UP is pressed

    ; ------------------------------
    ; Check if KEY_DOWN is pressed
    ; ------------------------------
    mov edi, KEY_DOWN     ; Load KEY_DOWN (264) into edi
    call IsKeyDown        ; Call IsKeyDown(KEY_DOWN)
    test eax, eax
    jnz .key_down_pressed ; Jump if KEY_DOWN is pressed

    ; ----------------------------------
    ; Else branch: no key pressed
    ; ----------------------------------
    movss xmm0, dword [five]        ; Load constant 5.0 into xmm0
    movss [r10], xmm0               ; speed = 5.0
    movss xmm0, dword [point_three] ; Load constant 0.3 into xmm0
    movss [r11], xmm0               ; acceleration = 0.3
    movss xmm0, dword [point_zero]  ; Load constant 0.0 into xmm0
    movss [r8], xmm0               ; volacity = 0.0
    jmp .done

.key_up_pressed:
    ; ------------------------------
    ; For KEY_UP pressed:
    ; volacity -= speed
    ; y += volacity
    ; speed += acceleration
    ; acceleration += 0.05
    ; ------------------------------
    movss xmm0, [r8]       ; Load volacity
    movss xmm1, [r10]      ; Load speed
    subss xmm0, xmm1       ; Compute volacity - speed
    movss [r8], xmm0       ; Store updated volacity

    ; y += volacity
    movss xmm2, [r9]       ; Load y
    addss xmm2, xmm0       ; y = y + (updated volacity)
    movss [r9], xmm2       ; Store updated y

    ; speed += acceleration
    movss xmm1, [r10]      ; Reload speed
    movss xmm3, [r11]      ; Load acceleration
    addss xmm1, xmm3       ; speed = speed + acceleration
    movss [r10], xmm1      ; Store updated speed

    ; acceleration += 0.05
    movss xmm3, [r11]                ; Load acceleration
    addss xmm3, dword [point_zero_five] ; acceleration = acceleration + 0.05
    movss [r11], xmm3                ; Store updated acceleration
    jmp .done

.key_down_pressed:
    ; ------------------------------
    ; For KEY_DOWN pressed:
    ; volacity += speed
    ; y += volacity
    ; speed += acceleration
    ; acceleration += 0.05
    ; ------------------------------
    movss xmm0, [r8]       ; Load volacity
    movss xmm1, [r10]      ; Load speed
    addss xmm0, xmm1       ; Compute volacity + speed
    movss [r8], xmm0       ; Store updated volacity

    ; y += volacity
    movss xmm2, [r9]       ; Load y
    addss xmm2, xmm0       ; y = y + updated volacity
    movss [r9], xmm2       ; Store updated y

    ; speed += acceleration
    movss xmm1, [r10]      ; Reload speed
    movss xmm3, [r11]      ; Load acceleration
    addss xmm1, xmm3       ; speed = speed + acceleration
    movss [r10], xmm1      ; Store updated speed

    ; acceleration += 0.05
    movss xmm3, [r11]                ; Load acceleration
    addss xmm3, dword [point_zero_five] ; acceleration = acceleration + 0.05
    movss [r11], xmm3                ; Store updated acceleration

.done:
    xor rax, rax
    leave
    ret

section .note.GNU-stack
