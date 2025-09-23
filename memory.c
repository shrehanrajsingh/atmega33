#include "memory.h"

void
stack_push (mem_t *m, char b)
{
  uint16_t sp = m->dmm.v[SP_START] | (((uint16_t)m->dmm.v[SP_START - 1]) << 8);
  m->dmm.v[sp--] = b;

  /* set stack pointer */
  m->dmm.v[SP_START] = sp & 0xFF;
  m->dmm.v[SP_START - 1] = (sp >> 8) & 0xFF;
}

char
stack_peep (mem_t *m)
{
  return m->dmm
      .v[(int)m->dmm.v[SP_START] | ((int)m->dmm.v[SP_START - 1] << 8)];
}

void
stack_pop (mem_t *m, char b)
{
  uint16_t sp
      = (m->dmm.v[SP_START] | (((uint16_t)m->dmm.v[SP_START - 1]) << 8)) + 1;

  /* set stack pointer */
  m->dmm.v[SP_START] = sp & 0xFF;
  m->dmm.v[SP_START - 1] = (sp >> 8) & 0xFF;
}