section .data
    KEY_C      equ 0x43         ; ASCII code for 'C'
    CURVE_VAL  dd 2             ; CURVE mode value
    LINE_VAL   dd 0             ; LINE mode value
    neg_five   dd -5.0          ; -5.0 as a float

section .text
    global update_key_c
    extern IsKeyPressed           ; Extern function: int IsKeyPressed(int key)

; update_key_c expects:
;   rdi = pointer to mode
;   rsi = pointer to time
;   rdx = pointer to x_speed
;   rcx = pointer to y_speed
update_key_c:
    push rbp
    mov rbp, rsp

    ; Preserve incoming pointers in registers:
    mov r8, rdi      ; r8 now holds pointer to mode
    mov r9, rsi      ; r9 now holds pointer to time
    mov r10, rdx     ; r10 now holds pointer to x_speed
    mov r11, rcx     ; r11 now holds pointer to y_speed

    ; Check if KEY_C is pressed:
    mov edi, KEY_C   ; Load KEY_C into edi
    call IsKeyPressed   ; Call IsKeyPressed(KEY_C)
    test eax, eax   ; Check if key is pressed
    jz .done        ; If not pressed, exit the function

    ; Key is pressed. Check if current mode is not CURVE.
    mov eax, [r8]   ; Load current mode from pointer in r8
    cmp eax, 2      ; Compare with CURVE (assumed to be 2)
    jne .set_curve  ; If mode != CURVE, jump to set_curve

    ; Else: mode is already CURVE.
    mov dword [r8], 0                 ; mode = LINE (0)
    jmp .done

.set_curve:
    ; Set mode to CURVE and reset time to 0.
    mov dword [r8], 2      ; mode = CURVE (2)
    xorps xmm0, xmm0       ; Zero out xmm0 (0.0)
    movss [r9], xmm0       ; time = 0.0

.done:
    xor rax, rax
    leave
    ret

section .note.GNU-stack