AS := nasm
ASMFLAGS := -felf64
SRC := $(wildcard kbuild/*.asm)
OBJ := $(SRC:.asm=.o)

%.o: %.asm
	$(AS) $(ASMFLAGS) $< -o $@

all: $(OBJ)
	mkdir -p obj
	mv $(OBJ) obj/
	rm -f kbuild/*.asm

clean:
	rm -rf obj
	rm -rf kbuild

.PHONY: all clean