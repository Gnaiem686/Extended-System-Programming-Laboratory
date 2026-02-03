# Lab B – AntiVirus (C) | Virus Signature Detection & Binary Patching

A C program that loads virus signatures from a binary signatures file into a dynamically allocated linked list, scans a suspected file for matching byte patterns, and can neutralize detected viruses by patching the binary in-place.

This project focuses on:
- pointers + dynamic allocation
- linked lists in C
- binary file I/O (`fread`, `fseek`, `fwrite`)
- debugging + memory safety (Valgrind)

---

## Features

- **Load signatures** from a binary signatures file (expects little-endian magic `VIRL`)
- **Store signatures in a linked list** (one node per virus)
- **Print signatures** (name, size, hex signature)
- **Detect viruses** in a suspected file using naive scanning + `memcmp`
- **Fix file / Neutralize** viruses by patching the first byte at the detected location to a **RET instruction** (`0xC3`)

---

## Signatures File Format

The signatures file begins with 4-byte magic:
- `VIRL` (little-endian)

Then repeated blocks:

| Offset | Size | Description |
|-------:|-----:|-------------|
| 0      | 2    | Signature length `N` (unsigned short, little-endian) |
| 2      | 16   | Virus name (null-terminated string padded with `\0`) |
| 18     | N    | Signature bytes |

---

## Build

```bash
make
```

Produces an executable (commonly): `AntiVirus`

---

## Run

The program expects the **suspected file** as a command-line argument:

```bash
./AntiVirus suspectedFile
```

Then use the menu:

1) Load signatures  
2) Print signatures  
3) Detect viruses  
4) Fix file  
5) Quit  

---

## Detection Output

When a virus is detected, the program prints:
- starting byte location (offset)
- virus name
- signature size

---

## Fix / Neutralize

“Fix file” scans the suspected file and neutralizes each detected virus by patching the first byte at the detected offset with:
- `0xC3` (x86 near `RET`)

This makes the virus code return immediately (educational simulation).

---

## Memory Safety (Valgrind)

Run with:

```bash
valgrind --leak-check=full ./AntiVirus suspectedFile
```

Goal: **no memory leaks** and **no invalid reads/writes**.

---

## Notes / Assumptions

- The suspected file is read into a fixed-size buffer (10KB). Scanning is done on the loaded buffer.
- Neutralization is done by writing `0xC3` at the detected virus location.
- This is a simplified educational implementation using a naive scanning algorithm.

---


## License

Educational / course assignment use.
