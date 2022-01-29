; Tell the assembler that the file starts at 0x1000
; Therefore, each label will be located at 0x1000+offset
(org 4096)

; Define new label called print_hello
(label print_hello)
    (out 0 #\h)
    (out 0 #\e)
    (out 0 #\l)
    (out 0 #\l)
    (out 0 #\o)
    (out 0 10) ; newline character
    (jmp print_hello) ; loop by jumping back to the label