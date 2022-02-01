(org 0x8000)
(jmp main)

(include "src/bios/print.lisp")

(label hello)
  (db "Hello from the bootloader!!" #\nl 0)

(label main)
  (display hello)

  (label halt)
    (jmp halt)
