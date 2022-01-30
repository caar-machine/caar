; Tell the assembler that the file starts at 0x1000
; Therefore, each label will be located at 0x1000+offset
(org 4096)

(defmacro display (param)
  (push param)
  (pop #A)
  (ldr #B #A)
  (jmp print_str))

; This is the reset vector, the first instruction executed by the cpu at boot
(jmp main)

; Define some text
(label boot_msg)
    (db "hello world" 0)

(label main)
   ; Print hello
   (display boot_msg)
 
(label print_str)
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
    (jmp print_str)
    

(label end)
    (nop)
