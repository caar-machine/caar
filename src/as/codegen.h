#include <ast.h>
#include <lib/vec.h>
#include <stdint.h>

typedef uint8_t Byte;
typedef vec_t(Byte) Bytes;

Bytes codegen(Ast ast);
