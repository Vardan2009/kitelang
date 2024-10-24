# Kite Programming Language
Kite is a very simple low-level programming language.\
It provides direct access to memory and hardware with pointers and registers, allowing for system-level programming with minimal abstraction.\
This repository contains a simple compiler for it written in C++ that generates 64bit x86 ELF assembly (tested with NASM 2.15.05 on Linux x86_64)

## Showcase
- Hello World!
```
#include <stdio.km>

global _start
fn _start() : byte {
	print("Hello, World!")
	return 0
}
```
- Variables (and basic arithmetic)
```
#include <stdio.km>

global _start
fn _start() : byte {
	let x : byte = 9
	let y : byte = 10

	print("x: ")
	printi(x)
	printc('\n')

	print("y: ")
	printi(y)
	printc('\n')

	print("x + y: ")
  	printi(x + y)
  	printc('\n')

  	return 0
}
```
- Functions
```
#include <stdio.km>

global _start
fn _start() : byte {
	printi(factor(10, 5))
	return 0
}

fn factor(a : int64, b : int64) : int64 {
	return a * b
}
```
- Pointers
```
#include <stdio.km>

global _start
fn _start() : byte {
	let value : byte = 20
	let vptr : ptr8 = &value

  	print("The Value: ")
  	printi(value)
  	printc('\n')

  	print("The Address: ")
  	printi(vptr)
  	printc('\n')

  	print("Dereferenced Pointer Value: ")
  	printi(*vptr)
  	printc('\n')
  	return 0
}
```
- User Input
```
#include <stdio.km>

global _start
fn _start() : byte {
	; allocate space for input reading
	let buf : char[512]
	; read line into buffer
	readln(buf, 512)

	return 0
}
```
- File Operations
```
#include <filesystem.km>
#include <stdio.km>

global _start
fn _start() : byte {
	; Open file and get file descriptor
	let fd : int64 = fopen("/test.txt")

	; if file descriptor is invalid
	if fd < 0 {
		print("Failed to open file!\n")
		return 1
	}
	
	; allocate space for file reading
	let buf : char[2048]

	; read into buffer
	readfd(fd, buf, 2048)

	; print the buffer
	print(buf)

	; close the file
	fclose(fd)

	return 0
}
```
