# Kite Programming Language
Kite is a very simple low-level programming language.\
It provides direct access to memory and hardware with pointers and registers, allowing for system-level programming with minimal abstraction.\
This repository contains a simple compiler for it written in C++ that generates 64bit x86 ELF assembly (tested with NASM 2.15.05 on Linux x86_64)

## Showcase
- Hello World!
```
global _start
extern print

fn _start() {
  print("Hello, World!")
  return 0
}
```
- Variables (and basic arithmetic)
```
global _start
extern {
  print,
  printi,
  printc
}

fn _start() {
  let x = 9
  let y = 10

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
extern printi

fn _start() {
  printi(factor(10, 5))
}

fn factor(a, b) {
  return a * b
}
```
- Pointers
```
global _start
extern {
  print,
  printi,
  printc
}

fn _start() {
  let value = 20
  let ptr = &value

  print("The Value: ")
  printi(value)
  printc('\n')

  print("The Address: ")
  printi(ptr)
  printc('\n')

  print("Dereferenced Pointer Value: ")
  printi(*ptr)
  printc('\n')
  return 0
}
```
