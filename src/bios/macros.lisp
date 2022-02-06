(defmacro call (label)
  (push #PC)
  (pop #G)
  (add #G 0x10)
  (push #G)
  (jmp label))

(defmacro ret ()
  (pop #PC))

(defmacro loop ()
  (label _halt)
  (jmp _halt))

(defmacro livt (lab)
  (push lab)
  (pop #IVT))
