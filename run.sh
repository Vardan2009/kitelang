#!/bin/sh

nasm -felf64 -o kbuild/source.o kbuild/source.asm
if [ $? -ne 0 ]; then
    echo "Error: nasm failed."
    exit 1
fi

ld -o kbuild/source kbuild/source.o stdlib/obj/*.o
if [ $? -ne 0 ]; then
    echo "Error: ld failed."
    exit 1
fi

clear
./kbuild/source "$@"
echo "\n\n\n[Program exited with exit code $?]"