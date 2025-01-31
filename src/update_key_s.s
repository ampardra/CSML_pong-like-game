section .data
    KEY_S equ 0x53

section .text
    global update_key_s
    extern IsKeyDown

; update_key_s expects:
;   rdi -> mode (pointer, integer)
;   rsi -> time (pointer, float)
update_key_s:
    push rbp
    mov rbp, rsp

    ; Preserve incoming pointers in r8 and r9
    mov r8, rdi      ; r8 now holds pointer to mode
    mov r9, rsi      ; r9 now holds pointer to time

    ; Check if KEY_S is pressed:
    mov edi, KEY_S   ; Use edi for key code argument
    call IsKeyDown   ; Call IsKeyDown(KEY_S)
    test eax, eax    ; Check return value in eax
    jz .done         ; If not pressed, jump to done

    ; Check the current mode (using pointer in r8)
    mov eax, [r8]    ; Load mode into eax
    cmp eax, 1       ; Compare mode with SIN (assume SIN is 1)
    je .set_line     ; If mode == SIN, jump to set_line

    ; Set mode to SIN and time to 0
    mov dword [r8], 1   ; Set mode to SIN (1)
    xorps xmm0, xmm0    ; Set xmm0 to 0.0 (alternative to mov dword [r9], 0)
    movss [r9], xmm0    ; Set time to 0
    jmp .done

.set_line:
    ; Set mode to LINE and time to 0
    mov dword [r8], 0   ; Set mode to LINE (0)
    xorps xmm0, xmm0    ; Zero out xmm0 again
    movss [r9], xmm0    ; Set time to 0

.done:
    xor rax, rax
    leave
    ret

section .note.GNU-stack
