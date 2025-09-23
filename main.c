#include "main.h"

void
test1 ()
{
  mem_t mem;
  /* ldi ===> 1110 KKKK rrrr KKKK */
  /* ldi r18, 0x4 */
  PROGMEM (mem)[0] = 0b11100000;
  PROGMEM (mem)[1] = 0b00110100;

  /* ldi r15, 0x20 */
  PROGMEM (mem)[2] = 0b11100010;
  PROGMEM (mem)[3] = 0b00000000;

  /* add ===> 0000 11rd dddd rrrr (add rd, rr) */
  /* add r18, r15 */
  PROGMEM (mem)[4] = 0b00001101;
  PROGMEM (mem)[5] = 0b00101111;

  /* jmp ===> 1001 010k kkkk 110k kkkk */
  /* jmp 0 */
  PROGMEM (mem)[6] = 0b10010100;
  PROGMEM (mem)[7] = 0b00001100;
  PROGMEM (mem)[8] = 0b00000000;
  PROGMEM (mem)[9] = 0b00000000;

  cpu_t *cpu = cpu_new ();
  *cpu->mem = mem;

  cpu_ip_start (cpu);

  /* print all registers (use cpu->mem, do not use mem) */
  for (int i = 0; i < 32; i++)
    {
      printf ("r%d: %d\n", i, DATAMEM (*cpu->mem)[i]);
    }

  cpu_destroy (cpu);
  free (cpu);
}

int
main (int argc, char **argv)
{
  printf ("Hello, from atmega33!\n");
  test1 ();
  return !printf ("Program Ended!\n");
}
