#include "assembler.h"

/* get r of Rr */
int get_register_r (char **);
int get_immediate (char **);

typedef struct _llist
{
  char *name;
  int loc;
  int is_register;
  struct _llist *next;

} list_t;

/* list routines */
int list_exists (list_t *, char *);
list_t *list_get (list_t *, char *);

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

abl_byte *
abl_codegen_fromfile (char *cont)
{
  list_t *l = NULL;

  char line[1024];
  int lc = 0;

  size_t cl = strlen (cont);
  size_t i = 0;
  int pc = 0;
  size_t inst_count = 0;

  /**
   * this is the preprocessor layer
   * Process only labels, directives and macros
   */
  while (i < cl)
    {
      char c = cont[i];

      if ((c == ' ' || c == '\t') && !lc)
        goto loop_end;

      if (c == '\n')
        {
          if (!lc)
            goto loop_end;

          line[lc] = '\0';

          /* analyze */

          /* directive */
          if (*line == '.')
            {
              char dir[10];
              int dc = 0;
              int j;

              for (j = 1 /* eat '.' */; line[j] != ' '; j++)
                {
                  if (line[j] >= 'a' && line[j] <= 'z')
                    line[j] -= 32;
                  dir[dc++] = line[j];
                }
              dir[dc] = '\0';

              if (EQ (dir, "EQU"))
                {
                  char name[64];
                  int nc = 0;
                  int k;

                  for (k = j + 1; line[k] != ','; k++)
                    name[nc++] = line[k];
                  name[nc] = '\0';

                  list_t *nl = malloc (sizeof (list_t));
                  nl->name = strdup (name);
                  char *temp = (char *)line + k + 1;

                  while (*temp && (*temp == ' ' || *temp == '\t'))
                    temp++;

                  if (*temp == 'r' || *temp == 'R')
                    {
                      nl->loc = get_register_r (&temp);
                      nl->is_register = 1;
                    }
                  else
                    {
                      nl->loc = get_immediate (&temp);
                      nl->is_register = 0;
                    }

                  nl->next = l;
                  l = nl;
                }

              goto reset_line;
            }

          /* check for ':' before ' ' */
          int saw_label = 0;
          char label[64];

          for (size_t k = 0; line[k] != ' '; k++)
            {
              if (line[k] == ':')
                {
                  strncpy (label, line, k);
                  label[k] = '\0';
                  saw_label = 1;
                  break;
                }
            }

          if (saw_label)
            {
              if (list_exists (l, label))
                {
                  printf ("error: label '%s' already exists", label);
                  exit (-1);
                }

              list_t *nl = malloc (sizeof (list_t));
              nl->name = strdup (label);
              nl->loc = pc;
              nl->next = l;
              nl->is_register = 0;
              l = nl;

              goto reset_line;
            }
          else /* instruction */
            {
              ++inst_count;
              /**
               * we will only use the instructions to
               * evaluate their size in memory for
               * properly assigning locations to labels
               */
              char inst[5];
              int ic = 0;

              for (size_t j = 0; line[j] != ' '; j++)
                inst[ic++] = line[j];
              inst[ic] = '\0';

              int j = 0;
              int saw_inst = 0;
              while (ABL_INST_2BYTE[j] != NULL)
                {
                  if (EQ (ABL_INST_2BYTE[j], inst))
                    {
                      saw_inst = 1;
                      pc += 2;
                      break;
                    }
                  j++;
                }

              if (!saw_inst)
                {
                  j = 0;

                  while (ABL_INST_4BYTE[j] != NULL)
                    {
                      if (EQ (ABL_INST_4BYTE[j], inst))
                        {
                          saw_inst = 1;
                          pc += 4;
                          break;
                        }
                      j++;
                    }
                }

              assert (saw_inst && "Invalid instruction");
            }

        reset_line:
          /* reset */
          lc = 0;
        }
      else
        line[lc++] = c;

    loop_end:
      i++;
    }

  abl_byte *code_arr = malloc ((inst_count + 1) * sizeof (abl_byte));
  int cac = 0;

  i = 0;
  while (i < cl)
    {
      char c = cont[i];

      if ((c == ' ' || c == '\t') && !lc)
        goto le2;

      if (c == '\n')
        {
          if (!lc)
            goto le2;

          line[lc] = '\0';

          /* analyze */

          /* directive */
          if (*line == '.')
            {
              goto rl2;
            }

          /* check for ':' before ' ' */
          int saw_label = 0;
          char label[64];

          for (size_t k = 0; line[k] != ' '; k++)
            {
              if (line[k] == ':')
                {
                  strncpy (label, line, k);
                  label[k] = '\0';
                  saw_label = 1;
                  break;
                }
            }

          if (saw_label)
            {
              goto rl2;
            }
          else /* instruction */
            {
              /**
               * We will preprocess the instruction
               * name and arguments here
               * This will be the place for all
               * of error checking
               */

              char inst[5];
              int ic = 0;
              size_t j;

              for (j = 0; line[j] != ' '; j++)
                inst[ic++] = line[j];
              inst[ic] = '\0';

              while (line[j] && (line[j] == ' ' || line[j] == '\t'))
                j++;

              if (line[j] != '\n') /* arguments */
                {
                  /**
                   * maximum amount of arguments
                   * instructions can take is 2
                   */

                  char arg1[1024];
                  char arg2[1024];

                  int a1c = 0;
                  int a2c = 0;

                  while (line[j] && line[j] != ',' && line[j] != '\n')
                    arg1[a1c++] = line[j++];
                  arg1[a1c] = '\0';

                  if (line[j] == ',')
                    j++; /* eat ',' */
                  while (line[j] && (line[j] == ' ' || line[j] == '\t'))
                    j++;

                  while (line[j] && line[j] != '\n')
                    arg2[a2c++] = line[j++];
                  arg2[a2c] = '\0';

                  //   printf ("%s(%d) %s(%d)\n", arg1, a1c, arg2, a2c);

                  char ninst[2064];

                  if (!a2c)
                    {
                      /* one argument */
                      if (list_exists (l, arg1))
                        {
                          list_t *lv1 = list_get (l, arg1);

                          if (lv1->is_register)
                            sprintf (ninst, "%s r%d", inst, lv1->loc);
                          else
                            sprintf (ninst, "%s %d", inst, lv1->loc);
                        }
                      else
                        {
                          sprintf (ninst, "%s %s", inst, arg1);
                        }
                    }
                  else
                    {
                      /* two arguments */
                      if (list_exists (l, arg1) && list_exists (l, arg2))
                        {
                          list_t *lv1 = list_get (l, arg1);
                          list_t *lv2 = list_get (l, arg2);

                          char fmt1[32], fmt2[32];

                          if (lv1->is_register)
                            sprintf (fmt1, "r%d", lv1->loc);
                          else
                            sprintf (fmt1, "%d", lv1->loc);

                          if (lv2->is_register)
                            sprintf (fmt2, "r%d", lv2->loc);
                          else
                            sprintf (fmt2, "%d", lv2->loc);

                          sprintf (ninst, "%s %s, %s", inst, fmt1, fmt2);
                        }
                      else if (list_exists (l, arg1))
                        {
                          list_t *lv1 = list_get (l, arg1);

                          char fmt1[32];

                          if (lv1->is_register)
                            sprintf (fmt1, "r%d", lv1->loc);
                          else
                            sprintf (fmt1, "%d", lv1->loc);

                          sprintf (ninst, "%s %s, %s", inst, fmt1, arg2);
                        }
                      else if (list_exists (l, arg2))
                        {
                          list_t *lv2 = list_get (l, arg2);

                          char fmt2[32];

                          if (lv2->is_register)
                            sprintf (fmt2, "r%d", lv2->loc);
                          else
                            sprintf (fmt2, "%d", lv2->loc);

                          sprintf (ninst, "%s %s, %s", inst, arg1, fmt2);
                        }
                      else
                        {
                          sprintf (ninst, "%s %s, %s", inst, arg1, arg2);
                        }
                    }
                  //   printf ("INST: %s\n", ninst);
                  abl_byte code = abl_codegen_fromline (ninst);
                  code_arr[cac++] = code;
                }
              else
                {
                  /* no arguments, generate bytecode */
                  abl_byte code = abl_codegen_fromline (line);
                  code_arr[cac++] = code;
                }
            }

        rl2:
          /* reset */
          lc = 0;
        }
      else
        line[lc++] = c;

    le2:
      i++;
    }

  //   while (l)
  //     {
  //       printf ("%s: %d\n", l->name, l->loc);
  //       l = l->next;
  //     }

  //   printf ("cac: %d\n", cac);
  //   for (int i = 0; i < cac; i++)
  //     {
  //       printf ("%d: %u\n", code_arr[i].size, code_arr[i].val);
  //     }

  abl_byte endbyte;
  endbyte.size = 0;
  endbyte.val = 0;

  code_arr[inst_count] = endbyte;
  return code_arr;
}

int
list_exists (list_t *l, char *name)
{
  while (l)
    {
      if (EQ (name, l->name))
        return 1;
      l = l->next;
    }

  return 0;
}

list_t *
list_get (list_t *l, char *name)
{
  while (l)
    {
      if (EQ (name, l->name))
        return l;
      l = l->next;
    }

  return NULL;
}

#undef SKIP_COMMA
#undef EQ
