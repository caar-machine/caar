SRC_DIR = .
TARGET = vm
CFLAGS = -I. -std=c99 -pedantic -Wall -Werror -Wextra -ggdb $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs sdl2)

include ../template.mk
