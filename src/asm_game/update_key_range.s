section .data
    KEY_E equ 0x45           ; ASCII code for 'E'
    KEY_Q equ 0x51           ; ASCII code for 'Q'
    two:   dd 2.0           ; 2.0 as a float
    five:  dd 5.0           ; 5.0 as a float

section .text
    global update_key_range
    extern IsKeyPressed       ; External function: int IsKeyPressed(int key)

; update_key_range expects:
;   rdi = pointer to range (float)
update_key_range:
    push rbp
    mov rbp, rsp

    ; Preserve pointer to range in r8
    mov r8, rdi

    ; --- Check if KEY_E is pressed ---
    mov edi, KEY_E         ; Load KEY_E into edi
    call IsKeyPressed         ; Call IsKeyPressed(KEY_E)
    test eax, eax          ; Test if key is pressed
    jz .check_KEY_Q        ; If not pressed, skip to KEY_Q check

    ; If KEY_E is pressed, perform: range += 2;
    movss xmm0, [r8]       ; Load current range into xmm0
    addss xmm0, dword [rel two]  ; xmm0 = range + 2.0
    movss [r8], xmm0       ; Store the updated range

.check_KEY_Q:
    ; --- Check if KEY_Q is pressed ---
    mov edi, KEY_Q         ; Load KEY_Q into edi
    call IsKeyPressed         ; Call IsKeyPressed(KEY_Q)
    test eax, eax          ; Check if key is pressed
    jz .done               ; If not pressed, skip the rest

    ; If KEY_Q is pressed, check if (range != 5)
    movss xmm1, [r8]       ; Load current range into xmm1
    ucomiss xmm1, dword [rel five] ; Compare range with 5.0
    je .done               ; If range equals 5.0, do nothing

    ; Else, perform: range -= 2;
    subss xmm1, dword [rel two]  ; xmm1 = range - 2.0
    movss [r8], xmm1       ; Store the updated range

.done:
    xor rax, rax
    leave
    ret

section .note.GNU-stack
