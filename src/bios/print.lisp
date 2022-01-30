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
