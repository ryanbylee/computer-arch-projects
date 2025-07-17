# Computer Architecture Projects

This repository contains two small C++ projects from a computer architecture course.

## Project P1: Simple CPU Simulator

Directory `P1` implements a tiny CPU that executes a subset of RISC-V instructions.  The simulator stores instructions in a 4&nbsp;KB array of bytes and maintains 32 integer registers.  Instruction fetch, decode, execute, memory, and writeback are modelled in `CPU.cpp` and `CPU.h` while `cpusim.cpp` provides a command line driver.

### Building
Compile the simulator with a C++ compiler supporting C++11:

```bash
g++ -std=c++11 cpusim.cpp CPU.cpp -o cpusim
```

### Running
Pass an instruction memory file (one byte per line in decimal) to the executable:

```bash
./cpusim P1/23instMem-all.txt
```

The program prints the final values of registers `a0` (`x10`) and `a1` (`x11`).  Sample assembly programs and their pre‑converted instruction memories are located in `P1/`.

## Project P2: Cache Simulator

Directory `P2` contains a two‑level cache simulator with a small victim cache.  `cache.cpp` and `cache.h` implement the cache, while `memory_driver.cpp` reads trace files describing memory accesses.

- L1: direct mapped with 16 sets.
- L2: eight‑way set associative with 16 sets.
- Victim cache: four entries.

### Building
Compile the memory driver together with the cache implementation:

```bash
g++ -std=c++11 memory_driver.cpp cache.cpp -o memory_driver
```

### Running
Provide a trace file where each line has four comma‑separated fields: `MemR, MemW, address, data`.

```bash
./memory_driver P2/trace/test.txt
```

The program outputs the L1 miss rate, L2 miss rate, and average memory access time.

## Repository Layout

```
P1/  CPU simulator sources, test programs, and design documents
P2/  Cache simulator sources and sample memory traces
```

Pre‑built binaries (`cpusim` and `memory_driver`) are included for convenience.
