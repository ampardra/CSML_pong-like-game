section .text
global update_mode_controls
extern IsKeyDown          ; Declare the IsKeyDown function from Raylib

update_mode_controls:
    ; Parameters:
    ; RDI -> mode (pointer)
    ; RSI -> time (pointer)
    ; RDX -> range (pointer)
    ; RCX -> x_speed (pointer)
    ; R8  -> y_speed (pointer)

    push rbp
    mov rbp, rsp

    ; Check if S key is pressed (KEY_S = 83)
    mov edi, 83
    call IsKeyDown
    test eax, eax
    jz .check_L          ; If not pressed, skip

    mov eax, [rdi]       ; Load mode
    cmp eax, 1          ; Compare with SIN
    je .set_line_S
    mov dword [rdi], 1  ; mode = SIN
    mov dword [rsi], 0  ; time = 0
    jmp .check_L

.set_line_S:
    mov dword [rdi], 0  ; mode = LINE
    mov dword [rsi], 0  ; time = 0

.check_L:
    ; Check if L key is pressed (KEY_L = 76)
    mov edi, 76
    call IsKeyDown
    test eax, eax
    jz .check_C          ; If not pressed, skip

    mov eax, [rdi]       ; Load mode
    cmp eax, 0          ; Compare with LINE
    je .check_C
    mov dword [rdi], 0  ; mode = LINE
    mov dword [rsi], 0  ; time = 0

.check_C:
    ; Check if C key is pressed (KEY_C = 67)
    mov edi, 67
    call IsKeyDown
    test eax, eax
    jz .check_E

    mov eax, [rdi]       ; Load mode
    cmp eax, 2          ; Compare with CURVE
    je .set_line_C
    mov dword [rdi], 2  ; mode = CURVE
    mov dword [rsi], 0  ; time = 0
    jmp .check_E

.set_line_C:
    mov dword [rcx], -5  ; x_speed = -5
    mov dword [r8], -5   ; y_speed = -5
    mov dword [rdi], 0   ; mode = LINE

.check_E:
    ; Check if E key is pressed (KEY_E = 69)
    mov edi, 69
    call IsKeyDown
    test eax, eax
    jz .check_Q

    add dword [rdx], 2  ; range += 2

.check_Q:
    ; Check if Q key is pressed (KEY_Q = 81)
    mov edi, 81
    call IsKeyDown
    test eax, eax
    jz .end

    cmp dword [rdx], 5  ; if (range != 5)
    je .end
    sub dword [rdx], 2  ; range -= 2

.end:
    xor rax, rax
    leave
    ret

section	.note.GNU-stack