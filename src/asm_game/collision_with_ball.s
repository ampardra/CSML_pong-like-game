section .data
    neg_one:      dd -1.0          ; Constant -1.0 for multiplication
    point_three:  dd 0.3           ; Constant 0.3
    ten:          dd 10.0          ; Constant 10.0
    zero:         dd 0.0           ; Constant 0.0 for comparisons

section .text
    global collision_with_ball
    ; Function prototype:
    ; void collision_with_ball(float* x_speed, float* y_speed, float* spin, float volacity, float acceleration);

collision_with_ball:
    push rbp
    mov rbp, rsp

    ; --- Step 1: Reverse ball.x_speed ---
    ; Load ball.x_speed (from pointer in rdi) into xmm2
    movss xmm2, [rdi]
    ; Multiply by -1.0 (negate)
    mulss xmm2, dword [neg_one]
    ; Store updated ball.x_speed back to memory
    movss [rdi], xmm2

    ; --- Step 2: Compare paddle volacity (in xmm0) with 0.0 ---
    movss xmm3, dword [zero]   ; Load 0.0 into xmm3
    ucomiss xmm0, xmm3         ; Compare volacity (xmm0) with 0.0
    ; If volacity > 0, jump to .volacity_positive
    ja .volacity_positive
    ; If volacity < 0, jump to .volacity_negative
    jb .volacity_negative
    ; If volacity == 0, no change is made
    jmp .done

.volacity_positive:
    ; --- When volacity > 0 ---
    ; Compute (acceleration - 0.3)
    movss xmm4, dword [point_three] ; Load 0.3 into xmm4
    movss xmm5, xmm1        ; Copy acceleration (from xmm1) to xmm5
    subss xmm5, xmm4        ; xmm5 = acceleration - 0.3

    ; Update ball.y_speed: ball.y_speed += (acceleration - 0.3)
    movss xmm6, [rsi]       ; Load ball.y_speed from pointer in rsi
    addss xmm6, xmm5        ; xmm6 = ball.y_speed + (acceleration - 0.3)
    movss [rsi], xmm6       ; Store updated ball.y_speed

    ; Update ball.spin: ball.spin -= (acceleration * 10)
    movss xmm7, xmm1        ; Copy acceleration to xmm7
    mulss xmm7, dword [ten] ; xmm7 = acceleration * 10
    movss xmm8, [rdx]       ; Load ball.spin from pointer in rdx
    subss xmm8, xmm7        ; xmm8 = ball.spin - (acceleration * 10)
    movss [rdx], xmm8       ; Store updated ball.spin
    jmp .done

.volacity_negative:
    ; --- When volacity < 0 ---
    ; Compute (acceleration - 0.3)
    movss xmm4, dword [point_three] ; Load 0.3 into xmm4
    movss xmm5, xmm1        ; Copy acceleration to xmm5
    subss xmm5, xmm4        ; xmm5 = acceleration - 0.3

    ; Update ball.y_speed: ball.y_speed -= (acceleration - 0.3)
    movss xmm6, [rsi]       ; Load ball.y_speed
    subss xmm6, xmm5        ; xmm6 = ball.y_speed - (acceleration - 0.3)
    movss [rsi], xmm6       ; Store updated ball.y_speed

    ; Update ball.spin: ball.spin += (acceleration * 10)
    movss xmm7, xmm1        ; Copy acceleration to xmm7
    mulss xmm7, dword [ten] ; xmm7 = acceleration * 10
    movss xmm8, [rdx]       ; Load ball.spin
    addss xmm8, xmm7        ; xmm8 = ball.spin + (acceleration * 10)
    movss [rdx], xmm8       ; Store updated ball.spin

.done:
    pop rbp
    ret

section .note.GNU-stack
