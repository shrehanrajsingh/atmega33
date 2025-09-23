#include "cpu.h"

cpu_t *
cpu_new ()
{
  cpu_t *cpu = malloc (sizeof (cpu_t));
  cpu->mem = malloc (sizeof (mem_t));
  cpu->pc = 0; /* 22 bits long */

  return cpu;
}

void
cpu_ip_start (cpu_t *cpu)
{
#define dma(X) DATAMEM (*mem)[X]
#define pma(X) PROGMEM (*mem)[X]

  mem_t **mmem = &cpu->mem;
  mem_t *mem = cpu->mem;

  //   uint16_t pc = ((uint16_t)cpu->pch << 8) | cpu->pcl;
  int pc = cpu->pc;
  int one_cycle = 0;

  while (pc != 0 || !one_cycle) /* only run for one program loop for now
                                 */
    {
      one_cycle++;
      char b1 = pma (pc);
      char b2 = pma (pc + 1);

      /* ldi */
      if (((b1 >> 4) & 0x0F) == 0b1110)
        {
          /* ldi: 1110 kkkk rrrr kkkk */

          printf ("found ldi\n");
          char k = ((b1 & 0xF) << 4) | (b2 & 0xF);
          char r = (b2 >> 4) & 0xF;

          dma (r + 15) = k;

          pc += 2;
          goto update_pc;
        }

      /* add */
      if (((b1 >> 2) & 0x3F) == 0b000011)
        {
          /* add: 0000 11rd dddd rrrr */

          printf ("found add\n");
          char r = (((b1 >> 1) & 0x1) << 4) | (b2 & 0xF);
          char d = ((b1 & 0x1) << 4) | ((b2 >> 4) & 0xF);

          //   printf ("%d %d\n", dma (r), dma (d));

          dma (d) += dma (r);
          /* deal with status flags later */

          pc += 2;
          goto update_pc;
        }

      /* jmp */
      if (((b1 >> 1) & 0x7F) == 0b1001010 && (b2 & 0b1100) == 0b1100)
        {
          /* jmp: 1001 010k kkkk 110k kkkk kkkk kkkk kkkk */

          printf ("found jmp\n");
          char b3 = pma (pc + 2);
          char b4 = pma (pc + 3);

          int j = b4 | ((int)b3 << 8) | ((b2 & 0x1) << 16)
                  | (((b2 >> 4) & 0xF) << 17) | ((b1 & 0x1) << 21);

          printf ("%d\n", j);
          pc = j;
          goto update_pc;
        }

    update_pc:
      //   cpu->pch = (pc >> 8) & 0xFF;
      //   cpu->pcl = pc & 0xFF;
      cpu->pc = pc;
    }

#undef pma
#undef dma
}

void
cpu_destroy (cpu_t *cpu)
{
  free (cpu->mem);
}