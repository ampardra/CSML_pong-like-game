section .data
    KEY_L equ 0x4C       ; Define KEY_L as 0x4C (hex for ASCII 'L', i.e. 76)

section .text
    global update_key_l
    extern IsKeyDown    ; External function: int IsKeyDown(int key);

; update_key_l expects:
;   rdi = pointer to mode
;   rsi = pointer to time
update_key_l:
    push rbp
    mov rbp, rsp

    ; Preserve incoming pointers:
    mov r8, rdi       ; r8 now holds pointer to mode
    mov r9, rsi       ; r9 now holds pointer to time

    ; Check if KEY_L is pressed:
    mov edi, KEY_L    ; Load KEY_L constant into edi
    call IsKeyDown    ; Call IsKeyDown(KEY_L)
    test eax, eax     ; Check if the key is pressed (nonzero result)
    jz .done          ; If not pressed, do nothing

    ; If KEY_L is pressed, check if mode != LINE.
    ; LINE is assumed to be 0.
    mov eax, [r8]     ; Load mode into eax from pointer in r8
    cmp eax, 0        ; Compare mode with LINE (0)
    je .done          ; If mode is already LINE, nothing to do

    ; Otherwise, set mode = LINE and reset time to 0.
    mov dword [r8], 0 ; Set mode to LINE (0)
    ; Reset time to 0.0f:
    xorps xmm0, xmm0  ; Zero out xmm0
    movss [r9], xmm0  ; Store 0.0 into time

.done:
    xor rax, rax      ; Clear return value
    leave
    ret

section .note.GNU-stack