(defmacro ret ()
  (pop #PC))

(defmacro loop ()
  (label _halt)
  (jmp _halt))

(defmacro livt (lab)
  (push lab)
  (pop #IVT))
