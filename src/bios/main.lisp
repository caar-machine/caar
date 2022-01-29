; Tell the assembler that the file starts at 0x1000
; Therefore, each label will be located at 0x1000+offset
(org 4096)

; This is the reset vector, the first instruction executed by the cpu at boot
(jmp main)

; Define some text
(label some_text)
    (db "hello world" 0)

(label main)
    ; put the address of the text into A
    (push some_text) 
    (pop #A)
    ; Put A[0] into B
    (ldr #B #A)
    ; Print it
    (jmp print_hello)

(label print_hello)
    ; If A[i] is 0
    (cmp #B 0)
    ; return
    (je end)
    ; Else, write char to serial
    (out 0 #B)
    ; i++
    (add #A 1)
    ; B = A[i]
    (ldr #B #A)
    ; Continue
    (jmp print_hello)
    

(label end)
    (nop)