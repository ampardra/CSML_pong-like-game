section .text
global update_ball_movement

update_ball_movement:
    ; Parameters:
    ; RDI -> y (pointer)
    ; RSI -> time (pointer)
    ; RDX -> range (value)
    ; RCX -> mode (value)
    ; XMM0 -> gravity (value)
    ; R8  -> y_speed (pointer)
    ; XMM1 -> spin (value)
    ; R9  -> angle (pointer)

    push rbp
    mov rbp, rsp

    ; Check if mode == SIN
    cmp rcx, 1
    jne .check_curve

    ; Compute cos(time) * range and add to y
    fld dword [rsi]   ; Load time into FPU stack (ST0 = time)
    fcos              ; Compute cos(time), ST0 = cos(time)
    fmul dword [rdx]  ; Multiply by range, ST0 = cos(time) * range
    fadd dword [rdi]  ; Add to y, ST0 = y + cos(time) * range
    fstp dword [rdi]  ; Store result in y and pop FPU stack

    ; Update time: time += 5 / range
    fild dword [five]   ; Load 5 into FPU stack (ST0 = 5)
    fdiv dword [rdx]    ; Divide by range, ST0 = 5 / range
    fadd dword [rsi]    ; time += 5 / range
    fstp dword [rsi]    ; Store result in time and pop FPU stack

.check_curve:
    cmp rcx, 2
    jne .end

    ; y_speed += gravity (load y_speed, add gravity, store back)
    movss xmm2, [r8]     ; Load y_speed into xmm2
    addss xmm2, xmm0     ; Add gravity
    movss [r8], xmm2     ; Store back to y_speed

.end:
    ; angle += spin (load angle, add spin, store back)
    movss xmm3, [r9]     ; Load angle into xmm3
    addss xmm3, xmm1     ; Add spin
    movss [r9], xmm3     ; Store back to angle

    xor rax, rax
    leave
    ret

section .data
five dd 5.0   ; Constant value for division

section	.note.GNU-stack