(org 0x1000)

(label lol)
    (push test)
    (pop #B)

    (ldr #A #B)
    (label _test)
    (jmp _test)

(label test)
    (dw 0x1000)
