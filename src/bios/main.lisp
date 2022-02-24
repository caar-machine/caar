; Tell the assembler that the file starts at 0x1000
; Therefore, each label will be located at 0x1000+offset
(org 0x1000)

; This is the reset vector, the first instruction executed by the cpu at boot
(jmp main)

(include "print.lisp")
(include "macros.lisp")
(include "ivt.lisp")

(label print_unknown)
  (display unknown)
  (loop)

(label print_no_devices)
  (display no_devices)
  (loop)

(label print_no_disk)
  (display no_disk)
  (loop)

(label disk_command)
  (db 0) ; Read
  (db 1) ; One sector
  (dw 0x2000) ; where to put the data

(label main)
  (display boot_msg) ; Print welcome message

  (display platform_str) ; print "Platform: "

  (in #A 3) ; Read the platform from io port 3
  (cmp #A 1) ; If platform is 1, then we're in an emulator, else, we're in a real machine
  (jne print_unknown) ; Jump to unknown if platform is not 1 (for now)

   ; if vm == 1
  (display vm_str) ; print "caar-vm (emulator)"
  (display cpu_str) ; Print "CPU: "

  (in #A 2) ; Get cpu from io port 2
  (cmp #A 1) ; if cpu == 1, continue
  (jne print_unknown) ; if cpu != 1, print "unknown" and halt

  ; else, print "caar1" and continue
  (display cpu1)
  (display disk_msg)

  (in #B 1) ; Read bus address
  (ldr #A #B) ; Get number of devices

  (cmp #A 0)
  (je print_no_devices)

  (add #B 4) ; Get devices[0].type

  (call find_disk)

  (push #B) ; We do this to restore the state of #B

  (display disk_found)

  (pop #B)

  (add #B 4) ; Get disk.addr
  (ldw #D #B) ; Load it into D
  (stw #D disk_command)  ; Write to disk address

  (display bootsector_loaded)

  (xor #C #C)
  (xor #A #A)
  (xor #B #B)

  (livt ivt)

  (jmp 0x2000)

  (loop) ; loop forever

; Parameters:
; A -> device count
; B -> bus address
(label find_disk)
  (cmp #C #A) ; if i == device_count
  (je print_no_disk) ; stop
  (add #C 1) ; i++
  (ldr #D #B) ; D = *B
  (cmp #D 1) ; if D == 1, we found a disk
  (je disk_end)
  (add #B 8) ; B += 8
  (jmp find_disk)

(label disk_end)
    (ret)


; Data -----------------------------------------------------------------------
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

(label no_devices)
  (db "No device attached to the machine. Halting" #\nl 0)

(label no_disk)
  (db "couldn't find any disk" #\nl 0)

(label disk_found)
  (db "disk found!" #\nl 0)

(label bootsector_loaded)
  (db "bootsector loaded.. jumping to code" #\nl 0)
