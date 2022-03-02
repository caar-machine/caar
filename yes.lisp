(org 0x1000)
(jmp lol)

(label str)
   (db "hello world" #\nl 0)

(include "src/bios/print.lisp")
(include "src/bios/macros.lisp")

(label lol)
   (display str)
   (display str)
   (loop)

