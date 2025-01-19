AS := nasm
ASMFLAGS := -felf64
KSRC := $(wildcard *.kite)
KITE := ../kitelang/kitelang

all: $(OBJ)
	@$(foreach kfile, $(KSRC), \
		$(KITE) $(kfile);)

	@$(foreach sfile, $(patsubst %.kite,kbuild/%.asm,$(KSRC)), \
		$(AS) $(ASMFLAGS) $(sfile) -o $(sfile:.asm=.o);)

	mkdir -p obj
	mv kbuild/*.o obj/

	rm -rf kbuild

clean:
	rm -rf obj
	rm -rf kbuild

.PHONY: all clean