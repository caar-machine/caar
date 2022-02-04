(org 0x8000)
(jmp main)

(include "src/bios/print.lisp")
(include "src/bios/macros.lisp")

(label COLOR_WHITE)
  (db 0xd5)
  (db 0xe8)
  (db 0xee)
  (db 0xff)

; Lmao, again
; I really need to fix byte sizing or idk how it's called
(label FB_SIZE)
  (db 0x00)
  (db 0x00)
  (db 0xc)
  (db 0)


(label main)
  (display searching_gpu)

  (in #B 1) ; Read bus address
  (ldr #A #B) ; Get number of devices
  (add #B 4) ; Get devices[0].type

  (call find_gpu)

  (push #B)
  (display found_gpu)
  (pop #B)

  (ldw #F COLOR_WHITE)
  (ldw #G FB_SIZE)

  (add #B 4) ; Get gpu.addr
  (ldw #D #B) ; Load it into D
  (ldw #D #D)  ; Getting framebuffer

  (xor #C #C) ; Making sure C is 0

  (label draw)
    (cmp #C #G) ; if C == fb.size, stop
    (je stop)
    (stw #D #F) ; *D = color
    (add #D 1) ; D++
    (add #C 1) ; C++
    (jmp draw)

  (display hello)

(label stop)
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


