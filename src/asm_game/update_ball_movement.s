section .text
    global update_ball_movement

update_ball_movement:
    ; Parameters (System V ABI)
    ; rdi -> pointer to y (float)
    ; rsi -> pointer to time (float)
    ; edx -> range (integer)
    ; ecx -> mode (integer)
    ; xmm0 -> gravity (float)
    ; r8  -> pointer to y_speed (float)
    ; r9  -> pointer to angle (float)
    ; xmm1 -> spin (float)
    
    push    rbp
    mov     rbp, rsp

    ; ------------------------------------------------------------------
    ; Preload the independent update of angle:
    ; angle = angle + spin
    ; (This work is independent of the mode-dependent branch.)
    movss   xmm4, [r9]      ; load current angle into xmm4
    addss   xmm4, xmm1      ; xmm4 = angle + spin
    ; ------------------------------------------------------------------

    ; Check mode. For mode==1, do the sine-based update;
    ; for mode==2, apply gravity to y_speed.
    cmp     ecx, 1
    jne     .curve_check

    ; -----------------------------------------------------------
    ; Mode 1: Compute new y and update time.
    ;   y += sin(time) * float(range)
    ;   time += 5.0 / float(range)
    ; -----------------------------------------------------------
    ; Compute sin(time):
    fld     dword [rsi]     ; FPU: ST0 = time
    fsin                    ; FPU: ST0 = sin(time)
    ; Transfer FPU result to xmm2:
    sub     rsp, 4          ; Allocate space for temporary float storage
    fstp   dword [rsp]     ; Store sin(time) into [rsp] and pop the FPU stack
    movss   xmm2, [rsp]     ; Load sin(time) into xmm2
    add     rsp, 4          ; Restore stack pointer

    ; Convert 'range' to float (first conversion for y update)
    cvtsi2ss xmm3, edx       ; xmm3 = float(range)
    ; Multiply: sin(time) * range
    mulss   xmm2, xmm3       ; xmm2 = sin(time)*range
    ; Update y: y = y + (sin(time)*range)
    addss   xmm2, [rdi]
    movss   [rdi], xmm2

    ; Now update time: time += 5.0 / range
    mov     eax, 5
    cvtsi2ss xmm2, eax       ; xmm2 = 5.0
    cvtsi2ss xmm3, edx       ; xmm3 = float(range) (again)
    divss   xmm2, xmm3       ; xmm2 = 5.0 / range
    addss   xmm2, [rsi]
    movss   [rsi], xmm2
    jmp     .done

.curve_check:
    ; -----------------------------------------------------------
    ; Mode 2: Apply gravity to y_speed.
    ; y_speed += gravity
    ; -----------------------------------------------------------
    cmp     ecx, 2
    jne     .done
    movss   xmm2, [r8]       ; load y_speed
    addss   xmm2, xmm0       ; add gravity
    movss   [r8], xmm2

.done:
    ; Finalize the angle update computed earlier.
    movss   [r9], xmm4

    xor     rax, rax
    pop     rbp
    ret

section .note.GNU-stack
