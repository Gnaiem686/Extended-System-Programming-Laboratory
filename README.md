# Extended System Programming Laboratory (Linux, C, x86)

This course focuses on **low-level system programming** principles through hands-on labs based on interacting with the **Linux operating system**. Most labs are implemented in **C**, with some **x86 assembly**, and emphasize understanding how programs interact with the OS at the process, memory, file-system, and binary level.

> The course (including the final exam) is conducted in English.

---

## Topics Covered

1. **Processes, memory models, and OS interaction**
2. **C programming foundations for systems:** storage classes, pointers, structures
3. **The raw machine:** basics of **x86 assembly**, linking assembly with C functions
4. **Direct system calls:** program interface to OS services
5. **Linux user view:** files, processes, permissions, and access control
6. **Command interpreters:** Unix shell concepts
7. **Binary file formats:** reading, parsing, and manipulation
8. **ELF format:** linking and loading fundamentals

---

## Repository Structure

This repository contains lab assignments and implementations from the course.

Typical layout:
- `home_labs/` – take-home labs (e.g., binary analysis, linked lists, patching)
- `lab*/` – additional lab folders (depending on course structure)

Each lab folder generally includes:
- source code (`.c`, sometimes `.S` / `.asm`)
- a `makefile`
- instructions or notes (if provided)

---

## Build & Run (General)

Most labs can be built using:

```bash
make
```

Run the produced executable according to the lab instructions (varies per lab).

---

## Tooling & Debugging

Common tools used throughout the course:
- **gcc** / **make**
- **gdb** (debugging, stack traces, breakpoints)
- **valgrind** (memory leak detection, invalid access checks)
- **objdump / readelf / nm** (binary inspection, ELF exploration)

Example Valgrind usage:

```bash
valgrind --leak-check=full ./program [args]
```

---

## Notes

This repository is intended for educational use and reflects course lab requirements and learning goals in **Linux systems programming**.
