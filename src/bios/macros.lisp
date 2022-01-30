(defmacro call (label)
  (push #PC)
  (pop #C)
  (add #C 0x10)
  (jmp label))

(defmacro ret ()
  (pop #PC))

(defmacro loop ()
  (label _halt)
  (jmp _halt))
