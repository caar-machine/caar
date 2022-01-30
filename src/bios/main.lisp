; Tell the assembler that the file starts at 0x1000
; Therefore, each label will be located at 0x1000+offset
(org 0x1000)

(defmacro halt ()
  (label _halt)
  (jmp _halt))

; Define a macro that takes a parameter named param
(defmacro display (param)
  ; A = address of string
  (push param)
  (pop #A)

  ; B = *A
  (ldr #B #A)

  ; We need to do this in order to return to the right place when popping PC
  (push #PC)
  (pop #C)

  ; Adding to PC the number  of bytes the following instructions will take
  (add #C 0x10) ; :meme:
  (push #C)
  
  ; Actually printing the string
  (jmp print_str))

; This is the reset vector, the first instruction executed by the cpu at boot
(jmp main)

; Define some raw bytes using db (define byte)
; #\nl means newline
; 0 means the end of the string
(label boot_msg)
    (db "===========================" #\nl)
    (db "low-level firmware for CAAR" #\nl)
    (db "===========================" #\nl)
    (db "Welcome to CAAR!" #\nl)
    (db "Firmware: caar-fw @ v0.0.1 by abbix" #\nl)
    (db "Platform: caar-vm" #\nl)
    (db "    CPU: caar1" #\nl)
    (db "Loading bootsector.." #\nl 0)

(label disk_msg)
  (db "Searching for disks..." #\nl 0)

(label main)
   ; Print text
  (display boot_msg)
  (display disk_msg)
  (halt)

 
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
	(pop #PC)
