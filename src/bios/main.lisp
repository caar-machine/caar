; Tell the assembler that the file starts at 0x1000
; Therefore, each label will be located at 0x1000+offset
(org 0x1000)

(defmacro call (label)
  (push #PC)
  (pop #C)
  (add #C 0x10)
  (jmp label))

(defmacro ret ()
  (pop #PC))

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
    (db "BootROM for CAAR" #\nl)
    (db "===========================" #\nl)
    (db "Welcome to CAAR!" #\nl)
    (db "Firmware: caar-fw @ v0.0.1 by abbix" #\nl 0)

(label cpu_str)
	(db "    CPU: " 0)

(label platform_str)
	(db "Platform: " 0)

(label vm_str)
	(db "caar-vm (emulator)" #\nl 0)

(label cpu1)
	(db "caar1" #\nl 0)

(label unknown)
	(db "unknown, halting." #\nl 0)

(label disk_msg)
  (db "Searching for disks..." #\nl 0)


(label main)
   ; Print text
  (display boot_msg)

  (display platform_str) ; print "Platform: "
  (in #A 3) ; Read the platform from io port 3
  (cmp #A 1) ; If platform is 1, then we're in an emulator, else, we're in a real machine

  (jne print_unknown) ; Jump to unknown if platform is not 1 (for now)

   ; if vm == 1
  (display vm_str) ; print "caar-vm (emulator)"

  (display cpu_str) ; Print "CPU: "

  (in #A 2) ; Get cpu from io port 2
  (cmp #A 1) ; if cpu == 1

  (jne print_unknown) ; if cpu != 1, print "unknown" and halt

  ; else, print "caar1" and continue
  (display cpu1)
  (display disk_msg)

  (in #A 1) ; Read bus address

  (jmp _halt)

  (label print_unknown)
	(display unknown)
	(jmp _halt)

  (label _halt)
    (jmp _halt)

 
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
	(ret)
