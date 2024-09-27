# Kite Programming Language
Kite is a very simple low-level programming language.\
It provides direct access to memory and hardware with pointers and registers, allowing for system-level programming with minimal abstraction.\
This repository contains a simple compiler for it written in C++ that generates 64bit x86 ELF assembly (tested with NASM 2.15.05 on Linux x86_64)

## Showcase
- Hello World!
```
global _start
extern print(ptr)

fn _start() : byte {
  print("Hello, World!")
  return 0
}
```
- Variables (and basic arithmetic)
```
global _start
extern {
  print(ptr),
  printi(int64),
  printc(ptr)
}

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
global _start
extern printi(int64)

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
global _start
extern {
  print(ptr),
  printi(int64),
  printc(ptr)
}

fn _start() : byte {
  let value : byte = 20
  let vptr : ptr = &value

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