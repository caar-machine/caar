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
  (mov #A param)

  ; B = *A
  (ldr #B #A)

  (call print_str))
