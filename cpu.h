#if !defined(CPU_H)
#define CPU_H

#include "header.h"
#include "memory.h"

typedef struct
{
  mem_t *mem;

  /* program counter */
  int pc;
} cpu_t;

#if defined(__cplusplus)
extern "C"
{
#endif // __cplusplus

  cpu_t *cpu_new ();
  void cpu_destroy (cpu_t *);

  /**
   * This routine will parse instructions based
   * on the memory layout.
   * The memory stores PC, code, data and all
   * other important variables required
   * to execute the program.
   * The CPU will parse the code, identify instructions
   * and execute them.
   * IP -> Instruction Parser
   */
  void cpu_ip_start (cpu_t *);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // CPU_H
