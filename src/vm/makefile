SRC_DIR = .
TARGET = vm
CFLAGS = -I. -std=c99 -Wall -Werror -Wextra -ggdb -I.. -O2
LDFLAGS = $(shell pkg-config --libs sdl2 glfw3 gl)
CFILES += ../lib/log.c

include ../template.mk
