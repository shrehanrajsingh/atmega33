#if !defined(DEBUGGER_H)
#define DEBUGGER_H

#include "cpu.h"
#include "header.h"
#include "memory.h"
#include <stdbool.h>

/* Maximum number of breakpoints */
#define MAX_BREAKPOINTS 16

/* Debugger execution modes */
typedef enum
{
  DBG_MODE_RUN,      /* Run continuously */
  DBG_MODE_STEP,     /* Step one instruction */
  DBG_MODE_STEP_N,   /* Step N instructions */
  DBG_MODE_CONTINUE, /* Continue until breakpoint */
  DBG_MODE_PAUSE     /* Paused state */
} dbg_mode_t;

/* Breakpoint structure */
typedef struct
{
  int address;     /* Program counter address */
  bool enabled;    /* Whether breakpoint is active */
  int hit_count;   /* Number of times hit */
  char label[32];  /* Optional label for breakpoint */
} breakpoint_t;

/* Debugger context */
typedef struct
{
  cpu_t *cpu;                          /* Reference to CPU */
  dbg_mode_t mode;                     /* Current execution mode */
  int step_count;                      /* Steps remaining in STEP_N mode */
  breakpoint_t breakpoints[MAX_BREAKPOINTS]; /* Breakpoint array */
  int num_breakpoints;                 /* Number of active breakpoints */
  
  /* Execution statistics */
  unsigned long total_instructions;    /* Total instructions executed */
  unsigned long cycles;                /* Total cycles (approximate) */
  
  /* History tracking */
  int prev_pc;                         /* Previous program counter */
  char last_instruction[64];           /* Last executed instruction */
  
  /* Watch variables */
  bool watch_enabled;                  /* Enable register watching */
  int watch_registers[32];             /* Previous register values */
  bool register_changed[32];           /* Track which registers changed */
  
} debugger_t;

#if defined(__cplusplus)
extern "C"
{
#endif

  /* Debugger lifecycle */
  debugger_t *debugger_new (cpu_t *cpu);
  void debugger_destroy (debugger_t *dbg);
  
  /* Execution control */
  void debugger_run (debugger_t *dbg);
  void debugger_step (debugger_t *dbg);
  void debugger_step_n (debugger_t *dbg, int n);
  void debugger_continue (debugger_t *dbg);
  void debugger_pause (debugger_t *dbg);
  
  /* Breakpoint management */
  int debugger_add_breakpoint (debugger_t *dbg, int address, const char *label);
  void debugger_remove_breakpoint (debugger_t *dbg, int index);
  void debugger_enable_breakpoint (debugger_t *dbg, int index);
  void debugger_disable_breakpoint (debugger_t *dbg, int index);
  void debugger_list_breakpoints (debugger_t *dbg);
  bool debugger_check_breakpoint (debugger_t *dbg, int address);
  
  /* Register inspection */
  void debugger_print_registers (debugger_t *dbg);
  void debugger_print_register (debugger_t *dbg, int reg);
  void debugger_print_register_range (debugger_t *dbg, int start, int end);
  void debugger_print_changed_registers (debugger_t *dbg);
  
  /* Memory inspection */
  void debugger_print_memory (debugger_t *dbg, int start, int length);
  void debugger_print_program_memory (debugger_t *dbg, int start, int length);
  
  /* Status and statistics */
  void debugger_print_status (debugger_t *dbg);
  void debugger_print_stats (debugger_t *dbg);
  
  /* Watch functionality */
  void debugger_enable_watch (debugger_t *dbg);
  void debugger_disable_watch (debugger_t *dbg);
  void debugger_update_watch (debugger_t *dbg);
  
  /* Disassembly */
  const char *debugger_disassemble_at (debugger_t *dbg, int address);
  void debugger_print_disassembly (debugger_t *dbg, int start, int count);
  
  /* Interactive debugger shell */
  void debugger_interactive (debugger_t *dbg);
  
  /* Helper to execute one instruction with debugging */
  bool debugger_execute_one (debugger_t *dbg);

#if defined(__cplusplus)
}
#endif

#endif // DEBUGGER_H
