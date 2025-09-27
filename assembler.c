#include "assembler.h"

/* get r of Rr */
int get_register_r (char **);
int get_immediate (char **);

abl_byte
abl_codegen_fromline (char *buf)
{
  char inst[5]; /* almost all instructions I have seen yet are 3 to 4
                   characters long */
  int ic = 0;
  size_t bl = strlen (buf);

  abl_byte res;
  res.size = 0;
  res.val = 0;

  while (*buf && (*buf == ' ' || *buf == '\t'))
    ++buf;

  while (*buf && (*buf != ' ') && ic < 4)
    {
      if (*buf >= 'a' && *buf <= 'z')
        inst[ic++] = (*buf++) - 32;
      else
        inst[ic++] = *buf++;
    }

  if (ic > 3)
    {
      printf ("instruction name is >4 characters. Aborting...");
      goto end;
    }

  inst[ic] = '\0';

  while (bl > 0 && (buf[bl - 1] == ' ' || buf[bl - 1] == '\t'))
    bl--;

  int i = 0;
  while (ABL_INST_2BYTE[i] != NULL)
    {
      if (!strcmp (ABL_INST_2BYTE[i], inst))
        {
          res.size = 2;
          break;
        }
      i++;
    }

  if (ABL_INST_2BYTE[i] == NULL)
    {
      i = 0;
      while (ABL_INST_4BYTE[i] != NULL)
        {
          if (!strcmp (ABL_INST_4BYTE[i], inst))
            {
              res.size = 4;
              break;
            }
          i++;
        }
    }

#define EQ(X, Y) !strcmp ((X), (Y))
#define SKIP_COMMA                                                            \
  do                                                                          \
    {                                                                         \
      while (*buf && (*buf == ' ' || *buf == '\t'))                           \
        ++buf;                                                                \
      if (*buf == ',')                                                        \
        ++buf;                                                                \
    }                                                                         \
  while (0);

  if (EQ (inst, "LDI"))
    {
      int r = get_register_r (&buf);
      SKIP_COMMA;
      int i = get_immediate (&buf);

      assert (r > 15 && "Accepted registers for LDI: R16-R31");

      res.val = LDI_Rr_K (r, i);
    }

  if (EQ (inst, "ADD"))
    {
      int r1 = get_register_r (&buf);
      SKIP_COMMA;
      int r2 = get_register_r (&buf);

      res.val = ADD_Rd_Rr (r1, r2);
    }

  if (EQ (inst, "JMP"))
    {
      int i = get_immediate (&buf);

      res.val = JMP_K (i);
    }

  if (EQ (inst, "LDS"))
    {
      int r = get_register_r (&buf);
      SKIP_COMMA;
      int k = get_immediate (&buf);

      res.val = LDS_Rd_K (r, k);
    }

  if (EQ (inst, "STS"))
    {
      int k = get_immediate (&buf);
      SKIP_COMMA;
      int r = get_register_r (&buf);

      res.val = STS_K_Rr (k, r);
    }

#undef EQ

end:
  return res;
}

int
get_register_r (char **B)
{
  char *b = *B;
  int r = 0;

  while (*b && (*b == ' ' || *b == '\t'))
    ++b;

  if (!(*b == 'r' || *b == 'R'))
    {
      printf ("invalid register '%c'\n", *b);
      return -1;
    }

  ++b; /* eat r/R */
  while (*b)
    {
      if (*b > '9' || *b < '0')
        break;

      r = r * 10 + (*b++ - '0');
    }

  *B = b;
  return r;
}

int
get_immediate (char **B)
{
  char *b = *B;
  int i = 0;

  while (*b && (*b == ' ' || *b == '\t'))
    ++b;

  int base = 10;

  if (*b == '$' || (*b == '0' && (*(b + 1) == 'x' || *(b + 1) == 'X')))
    {
      base = 16;
      ++b;

      if (*b == 'x' || *b == 'X')
        ++b;
    }
  else if (*b == '0' && (*(b + 1) == 'b' || *(b + 1) == 'B'))
    base = 2;

  while (*b)
    {
      if (*b > '9' && *b < '0')
        break;

      i = i * base + (*b++ - '0');
    }

  *B = b;
  return i;
}