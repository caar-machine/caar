(include "macros.lisp")

(label fault)
    (display fault_msg)
    (loop)

(label kb_handler)
    (display key_pressed_message)
    (ret)

(label ivt)
    ; --- Invalid opcode
    (db 1) 
    (dw fault)

    ; --- Page fault
    (db 1) 
    (dw fault)

    ; --- GPF
    (db 1)
    (dw fault) 

    ; --- Debug
    (db 1) 
    (dw fault)

    ; --- Empty for now
    (db 1) 
    (dw fault)

    (db 1) 
    (dw fault)

    (db 1) 
    (dw fault)

    (db 1) 
    (dw fault)

    (db 1) 
    (dw fault)

    (db 1) 
    (dw fault)

    ; --- Keyboard
    (db 1) 
    (dw kb_handler)



(label fault_msg)
    (db "CPU fault" #\nl 0)

(label key_pressed_message)
    (db "key pressed" #\nl 0)