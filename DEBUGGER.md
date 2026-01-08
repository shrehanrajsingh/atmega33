# ATMega33 Debugger Documentation

## Overview

The ATMega33 debugger provides comprehensive debugging capabilities for the ATMega32 emulator, allowing you to step through code, inspect registers and memory, set breakpoints, and track execution statistics.

## Features

### Core Debugging Features

- **Step-by-step Execution**: Execute one instruction at a time or N instructions
- **Breakpoints**: Set up to 16 breakpoints with labels and hit counting
- **Register Inspection**: View all registers, specific registers, or only changed registers
- **Memory Inspection**: Examine both data memory (SRAM) and program memory (Flash)
- **Disassembly**: View disassembled instructions at any address
- **Execution Statistics**: Track total instructions executed, cycles, and breakpoint hits
- **Register Watching**: Automatically track which registers change between steps

### Interactive Debugger Shell

The debugger provides an interactive command-line interface with 20+ commands for complete control over program execution and inspection.

## Getting Started

### Building with Debugger Support

The debugger is automatically included in the build:

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j8
```

### Basic Usage Example

```c
#include "debugger.h"

int main() {
    // Set up CPU and load program
    cpu_t *cpu = cpu_new();
    // ... load program into memory ...
    
    // Create debugger
    debugger_t *dbg = debugger_new(cpu);
    
    // Enter interactive mode
    debugger_interactive(dbg);
    
    // Cleanup
    debugger_destroy(dbg);
    cpu_destroy(cpu);
    free(cpu);
    
    return 0;
}
```

## Interactive Commands

### Execution Control

| Command | Alias | Description | Example |
|---------|-------|-------------|---------|
| `step` | `s` | Execute one instruction | `step` |
| `step N` | `s N` | Execute N instructions | `step 10` |
| `continue` | `c` | Continue until breakpoint | `continue` |

### Breakpoint Management

| Command | Description | Example |
|---------|-------------|---------|
| `break ADDR [LABEL]` | Set breakpoint at address | `break 0x0010 loop_start` |
| `delete N` | Delete breakpoint N | `delete 0` |
| `enable N` | Enable breakpoint N | `enable 0` |
| `disable N` | Disable breakpoint N | `disable 0` |
| `breakpoints` | List all breakpoints | `breakpoints` |

### Register Inspection

| Command | Alias | Description | Example |
|---------|-------|-------------|---------|
| `registers` | `r` | Show all 32 registers | `registers` |
| `register N` | - | Show specific register | `register 16` |
| `changed` | - | Show changed registers | `changed` |

### Memory Inspection

| Command | Description | Example |
|---------|-------------|---------|
| `memory ADDR LEN` | Show data memory | `memory 0x0100 64` |
| `program ADDR LEN` | Show program memory | `program 0x0000 32` |
| `disasm [ADDR] [CNT]` | Disassemble instructions | `disasm 0x0000 10` |

### Watch and Status

| Command | Description | Example |
|---------|-------------|---------|
| `watch on/off` | Enable/disable register watching | `watch on` |
| `status` | Show debugger status | `status` |
| `stats` | Show execution statistics | `stats` |

### General

| Command | Alias | Description |
|---------|-------|-------------|
| `help` | `h` | Show help message |
| `quit` | `q` | Exit debugger |

## Detailed Command Examples

### Setting Breakpoints

```
(dbg) break 0x0010 main_loop
Breakpoint 0 set at address 0x0010 (main_loop)

(dbg) break 0x0020
Breakpoint 1 set at address 0x0020 (BP1)

(dbg) breakpoints
Breakpoints:
ID  Address   Enabled  Hits  Label
--  --------  -------  ----  -----
 0  0x0010    Yes         0  main_loop
 1  0x0020    Yes         0  BP1
```

### Stepping Through Code

```
(dbg) step
Stepping one instruction...

Disassembly from 0x0000:
Address   Bytes         Instruction
--------  ------------  -----------
0x0000    E0 34          LDI R19, 0x04 <-- PC

(dbg) step 3
Stepping 3 instructions...
[Executes 3 instructions]

(dbg) registers
Registers:
R00: 0x00 (  0)  R01: 0x00 (  0)  R02: 0x00 (  0)  R03: 0x00 (  0)
R04: 0x00 (  0)  R05: 0x00 (  0)  R06: 0x00 (  0)  R07: 0x00 (  0)
...
R16: 0x04 (  4)  R17: 0x20 ( 32)  R18: 0x24 ( 36)  R19: 0x00 (  0)
...
```

### Register Watching

```
(dbg) watch on
Register watching enabled

(dbg) step
Stepping one instruction...

(dbg) changed
Changed registers:
R16: 0x00 -> 0x04 (0 -> 4)
```

### Memory Inspection

```
(dbg) memory 0x0100 32
Data Memory [0x0100 - 0x011F]:
0x0100: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  | ................
0x0110: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  | ................

(dbg) program 0x0000 16
Program Memory [0x0000 - 0x000F]:
0x0000: E0 34 E2 00 0D 2F 94 0C 00 00 00 00 00 00 00 00
```

### Disassembly

```
(dbg) disasm 0x0000 5
Disassembly from 0x0000:
Address   Bytes         Instruction
--------  ------------  -----------
0x0000    E0 34          LDI R19, 0x04 <-- PC
0x0002    E2 00          LDI R16, 0x20
0x0004    0D 2F          ADD R18, R15
0x0006    94 0C 00 00    JMP 0x0000
0x000A    00 00          ??? (0x0000)
```

### Execution Statistics

```
(dbg) stats
=== Execution Statistics ===
Total Instructions Executed: 1523
Total Cycles (approx): 1523
Current PC: 0x0006

Breakpoint Hits:
  main_loop (0x0010): 15 hits
  check_condition (0x0030): 8 hits
===========================
```

## Supported Instructions (Disassembly)

The debugger can disassemble the following instructions:

1. **LDI** (Load Immediate): `LDI Rd, K`
2. **ADD** (Add without Carry): `ADD Rd, Rr`
3. **JMP** (Jump): `JMP k`
4. **LDS** (Load Direct from Data Space): `LDS Rd, k`
5. **STS** (Store Direct to Data Space): `STS k, Rr`

Unknown instructions are displayed as `??? (0xXXXX)`.

## Debugger API

### Initialization

```c
debugger_t *debugger_new(cpu_t *cpu);
void debugger_destroy(debugger_t *dbg);
```

### Execution Control

```c
void debugger_step(debugger_t *dbg);
void debugger_step_n(debugger_t *dbg, int n);
void debugger_continue(debugger_t *dbg);
void debugger_pause(debugger_t *dbg);
void debugger_run(debugger_t *dbg);
```

### Breakpoint Management

```c
int debugger_add_breakpoint(debugger_t *dbg, int address, const char *label);
void debugger_remove_breakpoint(debugger_t *dbg, int index);
void debugger_enable_breakpoint(debugger_t *dbg, int index);
void debugger_disable_breakpoint(debugger_t *dbg, int index);
void debugger_list_breakpoints(debugger_t *dbg);
bool debugger_check_breakpoint(debugger_t *dbg, int address);
```

### Register Inspection

```c
void debugger_print_registers(debugger_t *dbg);
void debugger_print_register(debugger_t *dbg, int reg);
void debugger_print_register_range(debugger_t *dbg, int start, int end);
void debugger_print_changed_registers(debugger_t *dbg);
```

### Memory Inspection

```c
void debugger_print_memory(debugger_t *dbg, int start, int length);
void debugger_print_program_memory(debugger_t *dbg, int start, int length);
```

### Status and Statistics

```c
void debugger_print_status(debugger_t *dbg);
void debugger_print_stats(debugger_t *dbg);
```

### Watch Functionality

```c
void debugger_enable_watch(debugger_t *dbg);
void debugger_disable_watch(debugger_t *dbg);
void debugger_update_watch(debugger_t *dbg);
```

### Disassembly

```c
const char *debugger_disassemble_at(debugger_t *dbg, int address);
void debugger_print_disassembly(debugger_t *dbg, int start, int count);
```

### Interactive Mode

```c
void debugger_interactive(debugger_t *dbg);
```

## Integration with CPU

To fully integrate the debugger with the CPU execution loop, modify `cpu_ip_start()` to:

1. Check for breakpoints before each instruction
2. Support step mode execution
3. Update debugger statistics
4. Call `debugger_update_watch()` after each instruction

Example integration:

```c
void cpu_ip_start_with_debug(cpu_t *cpu, debugger_t *dbg) {
    while (1) {
        // Check breakpoint
        if (debugger_check_breakpoint(dbg, cpu->pc)) {
            debugger_pause(dbg);
            debugger_interactive(dbg);
        }
        
        // Execute instruction
        // ... existing instruction execution code ...
        
        // Update debugger
        dbg->total_instructions++;
        dbg->cycles += instruction_cycles;
        debugger_update_watch(dbg);
        
        // Handle step mode
        if (dbg->mode == DBG_MODE_STEP) {
            debugger_pause(dbg);
            debugger_interactive(dbg);
        }
    }
}
```

## Limitations

- Maximum 16 breakpoints
- Disassembly only supports the 5 currently implemented instructions
- Cycle counting is approximate (assumes 1 cycle per instruction)
- No support for conditional breakpoints yet
- No support for watchpoints (memory breakpoints) yet

## Future Enhancements

- [ ] Conditional breakpoints (break when register == value)
- [ ] Memory watchpoints (break on memory access)
- [ ] Instruction history/trace buffer
- [ ] Save/load debugging sessions
- [ ] Remote debugging support
- [ ] GUI integration
- [ ] Performance profiling
- [ ] Code coverage analysis

## Troubleshooting

### Debugger Commands Not Working

Ensure you're in interactive mode:
```c
debugger_interactive(dbg);
```

### Breakpoints Not Triggering

1. Verify breakpoint is enabled: `breakpoints`
2. Check address is correct: `disasm 0xADDR 1`
3. Ensure CPU integration calls `debugger_check_breakpoint()`

### Register Values Not Updating

1. Enable watch mode: `watch on`
2. Step through code: `step`
3. Check changed registers: `changed`

## Examples

### Example 1: Basic Debugging Session

```
$ ./atmega33

=== ATMega33 Interactive Debugger ===
Type 'help' for available commands

(dbg) break 0x0000 start
Breakpoint 0 set at address 0x0000 (start)

(dbg) watch on
Register watching enabled

(dbg) step 5
Stepping 5 instructions...

(dbg) registers
[Shows all register values]

(dbg) changed
Changed registers:
R16: 0x00 -> 0x04 (0 -> 4)
R17: 0x00 -> 0x20 (0 -> 32)

(dbg) disasm
[Shows disassembly around PC]

(dbg) quit
Exiting debugger
```

### Example 2: Debugging a Loop

```
(dbg) break 0x0010 loop_start
Breakpoint 0 set at address 0x0010 (loop_start)

(dbg) break 0x0020 loop_end
Breakpoint 1 set at address 0x0020 (loop_end)

(dbg) continue
Continuing execution until breakpoint...

*** Breakpoint 0 hit at 0x0010 (loop_start) ***

(dbg) register 16
R16: 0x05 (5)

(dbg) continue
[Continues to next breakpoint]

(dbg) stats
=== Execution Statistics ===
Total Instructions Executed: 45
Total Cycles (approx): 45
Current PC: 0x0020

Breakpoint Hits:
  loop_start (0x0010): 5 hits
  loop_end (0x0020): 1 hits
===========================
```

## Contributing

When adding new instructions to the emulator, update the disassembler in `debugger.c`:

1. Add instruction pattern matching in `debugger_disassemble_at()`
2. Format output string with instruction mnemonic and operands
3. Return correct instruction size (2 or 4 bytes)

## License

Same as ATMega33 project (see main README).
