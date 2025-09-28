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

  /* jmp ===> 1001 010k kkkk 110k kkkk kkkk kkkk kkkk */
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

void
add_mem (mem_t *m, int *c, abl_byte b)
{
  if (b.size == 2)
    {
      PROGMEM (*m)[(*c)++] = (b.val >> 8) & 0xFF;
      PROGMEM (*m)[(*c)++] = b.val & 0xFF;
    }
  else if (b.size == 4)
    {
      PROGMEM (*m)[(*c)++] = (b.val >> 24) & 0xFF;
      PROGMEM (*m)[(*c)++] = (b.val >> 16) & 0xFF;
      PROGMEM (*m)[(*c)++] = (b.val >> 8) & 0xFF;
      PROGMEM (*m)[(*c)++] = b.val & 0xFF;
    }
}

void
test2 ()
{
  printf ("%u\n", LDI_Rr_K (18, 4));
  printf ("%u\n", LDI_Rr_K (15, 32));
  printf ("%u\n", ADD_Rd_Rr (18, 15));
  printf ("%u\n", JMP_K (0));
  printf ("%u\n", LDS_Rd_K (16, 300));
}

void
test3 ()
{
  mem_t mem;
  int c = 0;

  add_mem (&mem, &c, abl_codegen_fromline ("LDI R20, 10"));
  add_mem (&mem, &c, abl_codegen_fromline ("LDI R21, 20"));
  add_mem (&mem, &c, abl_codegen_fromline ("ADD R20, R21"));
  add_mem (&mem, &c, abl_codegen_fromline ("JMP 0"));

  cpu_t *cpu = cpu_new ();
  *cpu->mem = mem;

  cpu_ip_start (cpu);

  for (int i = 0; i < 32; i++)
    {
      printf ("r%d: %d\n", i, DATAMEM (*cpu->mem)[i]);
    }

  cpu_destroy (cpu);
  free (cpu);
}

void
test4 ()
{
  tui_init ();

  tui_ctx_t *ctx = tui_new (80, 24);
  tui_disp (ctx);

  tui_destroy (ctx);
  free (ctx);

  tui_kill ();
}

void
test5 ()
{
  FILE *f = fopen ("../test/test.asm", "r");
  if (f == NULL)
    {
      printf ("Error opening file test.asm\n");
      return;
    }

  fseek (f, 0, SEEK_END);
  long fs = ftell (f);
  fseek (f, 0, SEEK_SET);

  char *fcont = (char *)malloc (fs + 2);
  if (fcont == NULL)
    {
      printf ("Memory allocation failed\n");
      fclose (f);
      return;
    }

  size_t read_size = fread (fcont, 1, fs, f);
  fcont[read_size++] = '\n';
  fcont[read_size] = '\0';

  fclose (f);

  abl_byte *code = abl_codegen_fromfile (fcont);
  mem_t mem;
  int c = 0;

  while (code->size)
    {
      add_mem (&mem, &c, *code);
      code++;
    }

  cpu_t *cpu = cpu_new ();
  *cpu->mem = mem;

  cpu_ip_start (cpu);

  for (int i = 0; i < 32; i++)
    {
      printf ("r%d: %d\n", i, DATAMEM (*cpu->mem)[i]);
    }

  cpu_destroy (cpu);
  free (cpu);

  free (fcont);
}

int
main (int argc, char **argv)
{
  // printf ("Hello, from atmega33!\n");
  // test4 ();
  test5 ();
  // return !printf ("Program Ended!\n");
  return 0;
}
