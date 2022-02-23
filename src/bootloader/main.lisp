(org 0x2000)
(jmp main)

(include "../bios/print.lisp")
(include "../bios/macros.lisp")

(label COLOR_BG)
  (dw 0x073642)

(label COLOR_FG)
  (dw 0xffffff)

; TODO: stop using hardcoded fb size
(label FB_SIZE)
  (dw 0xc0000)

(label some_char)
  (db 0)

; Function: fill
; fill screens
; params:
; E -> fb.size
; F -> color
(label fill-screen)
  (cmp #C #E) ; if C == fb.size, stop
  (je stop)
  (stw #D #F) ; *D = color
  (add #D 1) ; D++
  (add #C 1) ; C++
  (jmp fill-screen)

 (label stop)
    (xor #C #C)
    (ret)

(label main)
  (display searching_gpu)

  (in #B 1) ; Read bus address
  (ldr #A #B) ; Get number of devices
  (add #B 4) ; Get devices[0].type

  (call find_gpu)

  (push #B)
  (display found_gpu)
  (pop #B)

  (ldw #F COLOR_BG) ; Load background color into F
  (ldw #E FB_SIZE) ; Load fb.size into E

  (add #B 4) ; Get gpu.addr
  (ldw #D #B) ; Load it into D
  (ldw #D #D)  ; Getting framebuffer

  (xor #C #C) ; Making sure C is 0

  (call fill-screen)

  (display hello)

  (loop)


(label find_gpu)
  (cmp #C #A) ; if i == device_count
  (je print_no_gpu) ; TODO: add support for running headless.
  (add #C 1) ; i++
  (ldr #D #B) ; D = *B
  (cmp #D 2) ; if type == 2, we found a gpu
  (je gpu_end)
  (add #B 8) ; B += 8
  (jmp find_gpu)

(label gpu_end)
    (ret)

(label print_no_gpu)
  (display no_gpu)
  (loop)


(label searching_gpu)
  (db "Searching for a gpu..." #\nl 0)

(label found_gpu)
  (db "Found gpu, filling screen..." #\nl 0)

(label hello)
  (db "Hello from the bootloader!!" #\nl 0)

(label no_gpu)
  (db "Couldn't find GPU. Halting. " #\nl 0)
