#include "debugger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

debugger_t *
debugger_new (cpu_t *cpu)
{
  debugger_t *dbg = malloc (sizeof (debugger_t));
  dbg->cpu = cpu;
  dbg->mode = DBG_MODE_PAUSE;
  dbg->step_count = 0;
  dbg->num_breakpoints = 0;
  dbg->total_instructions = 0;
  dbg->cycles = 0;
  dbg->prev_pc = 0;
  dbg->watch_enabled = false;
  
  /* Initialize breakpoints */
  for (int i = 0; i < MAX_BREAKPOINTS; i++)
    {
      dbg->breakpoints[i].enabled = false;
      dbg->breakpoints[i].hit_count = 0;
    }
  
  /* Initialize watch registers */
  for (int i = 0; i < 32; i++)
    {
      dbg->watch_registers[i] = 0;
      dbg->register_changed[i] = false;
    }
  
  strcpy (dbg->last_instruction, "None");
  
  return dbg;
}

void
debugger_destroy (debugger_t *dbg)
{
  free (dbg);
}

/* Breakpoint management */
int
debugger_add_breakpoint (debugger_t *dbg, int address, const char *label)
{
  if (dbg->num_breakpoints >= MAX_BREAKPOINTS)
    {
      printf ("Error: Maximum breakpoints (%d) reached\\n\", MAX_BREAKPOINTS);
      return -1;
    }
  
  int idx = dbg->num_breakpoints++;
  dbg->breakpoints[idx].address = address;
  dbg->breakpoints[idx].enabled = true;
  dbg->breakpoints[idx].hit_count = 0;
  
  if (label)
    strncpy (dbg->breakpoints[idx].label, label, 31);
  else
    snprintf (dbg->breakpoints[idx].label, 32, \"BP%d\", idx);
  
  printf (\"Breakpoint %d set at address 0x%04X (%s)\\n\", 
          idx, address, dbg->breakpoints[idx].label);
  
  return idx;
}

void
debugger_remove_breakpoint (debugger_t *dbg, int index)
{
  if (index < 0 || index >= dbg->num_breakpoints)
    {
      printf (\"Error: Invalid breakpoint index %d\\n\", index);
      return;
    }
  
  /* Shift breakpoints down */
  for (int i = index; i < dbg->num_breakpoints - 1; i++)
    {
      dbg->breakpoints[i] = dbg->breakpoints[i + 1];
    }
  
  dbg->num_breakpoints--;
  printf (\"Breakpoint %d removed\\n\", index);
}

void
debugger_enable_breakpoint (debugger_t *dbg, int index)
{
  if (index < 0 || index >= dbg->num_breakpoints)
    {
      printf (\"Error: Invalid breakpoint index %d\\n\", index);
      return;
    }
  
  dbg->breakpoints[index].enabled = true;
  printf (\"Breakpoint %d enabled\\n\", index);
}

void
debugger_disable_breakpoint (debugger_t *dbg, int index)
{
  if (index < 0 || index >= dbg->num_breakpoints)
    {
      printf (\"Error: Invalid breakpoint index %d\\n\", index);
      return;
    }
  
  dbg->breakpoints[index].enabled = false;
  printf (\"Breakpoint %d disabled\\n\", index);
}

void
debugger_list_breakpoints (debugger_t *dbg)
{
  if (dbg->num_breakpoints == 0)
    {
      printf (\"No breakpoints set\\n\");
      return;
    }
  
  printf (\"\\nBreakpoints:\\n\");
  printf (\"ID  Address   Enabled  Hits  Label\\n\");
  printf (\"--  --------  -------  ----  -----\\n\");
  
  for (int i = 0; i < dbg->num_breakpoints; i++)
    {
      printf (\"%2d  0x%04X    %s     %4d  %s\\n\",
              i,
              dbg->breakpoints[i].address,
              dbg->breakpoints[i].enabled ? \"Yes\" : \"No \",
              dbg->breakpoints[i].hit_count,
              dbg->breakpoints[i].label);
    }
  printf (\"\\n\");
}

bool
debugger_check_breakpoint (debugger_t *dbg, int address)
{
  for (int i = 0; i < dbg->num_breakpoints; i++)
    {
      if (dbg->breakpoints[i].enabled && 
          dbg->breakpoints[i].address == address)
        {
          dbg->breakpoints[i].hit_count++;
          printf (\"\\n*** Breakpoint %d hit at 0x%04X (%s) ***\\n\",
                  i, address, dbg->breakpoints[i].label);
          return true;
        }
    }
  return false;
}

/* Register inspection */
void
debugger_print_registers (debugger_t *dbg)
{
  printf (\"\\nRegisters:\\n\");
  for (int i = 0; i < 32; i += 4)
    {
      printf (\"R%02d: 0x%02X (%3d)  \", 
              i, DATAMEM (*dbg->cpu->mem)[i], DATAMEM (*dbg->cpu->mem)[i]);
      printf (\"R%02d: 0x%02X (%3d)  \", 
              i+1, DATAMEM (*dbg->cpu->mem)[i+1], DATAMEM (*dbg->cpu->mem)[i+1]);
      printf (\"R%02d: 0x%02X (%3d)  \", 
              i+2, DATAMEM (*dbg->cpu->mem)[i+2], DATAMEM (*dbg->cpu->mem)[i+2]);
      printf (\"R%02d: 0x%02X (%3d)\\n\", 
              i+3, DATAMEM (*dbg->cpu->mem)[i+3], DATAMEM (*dbg->cpu->mem)[i+3]);
    }
  printf (\"\\n\");
}

void
debugger_print_register (debugger_t *dbg, int reg)
{
  if (reg < 0 || reg > 31)
    {
      printf (\"Error: Invalid register R%d\\n\", reg);
      return;
    }
  
  printf (\"R%02d: 0x%02X (%d)\\n\", 
          reg, DATAMEM (*dbg->cpu->mem)[reg], DATAMEM (*dbg->cpu->mem)[reg]);
}

void
debugger_print_register_range (debugger_t *dbg, int start, int end)
{
  if (start < 0 || start > 31 || end < 0 || end > 31 || start > end)
    {
      printf (\"Error: Invalid register range R%d-R%d\\n\", start, end);
      return;
    }
  
  printf (\"\\nRegisters R%d to R%d:\\n\", start, end);
  for (int i = start; i <= end; i++)
    {
      printf (\"R%02d: 0x%02X (%3d)\", 
              i, DATAMEM (*dbg->cpu->mem)[i], DATAMEM (*dbg->cpu->mem)[i]);
      if ((i - start + 1) % 4 == 0)
        printf (\"\\n\");
      else
        printf (\"  \");
    }
  if ((end - start + 1) % 4 != 0)
    printf (\"\\n\");
  printf (\"\\n\");
}

void
debugger_print_changed_registers (debugger_t *dbg)
{
  bool any_changed = false;
  
  printf (\"\\nChanged registers:\\n\");
  for (int i = 0; i < 32; i++)
    {
      if (dbg->register_changed[i])
        {
          printf (\"R%02d: 0x%02X -> 0x%02X (%d -> %d)\\n\",
                  i,
                  dbg->watch_registers[i],
                  DATAMEM (*dbg->cpu->mem)[i],
                  dbg->watch_registers[i],
                  DATAMEM (*dbg->cpu->mem)[i]);
          any_changed = true;
        }
    }
  
  if (!any_changed)
    printf (\"No registers changed\\n\");
  
  printf (\"\\n\");
}

/* Memory inspection */
void
debugger_print_memory (debugger_t *dbg, int start, int length)
{
  printf (\"\\nData Memory [0x%04X - 0x%04X]:\\n\", start, start + length - 1);
  
  for (int i = 0; i < length; i += 16)
    {
      printf (\"0x%04X: \", start + i);
      
      /* Print hex values */
      for (int j = 0; j < 16 && (i + j) < length; j++)
        {
          printf (\"%02X \", DATAMEM (*dbg->cpu->mem)[start + i + j]);
        }
      
      /* Padding */
      for (int j = i + 16 - length; j > 0 && j < 16; j--)
        printf (\"   \");
      
      printf (\" | \");
      
      /* Print ASCII representation */
      for (int j = 0; j < 16 && (i + j) < length; j++)
        {
          unsigned char c = DATAMEM (*dbg->cpu->mem)[start + i + j];
          printf (\"%c\", (c >= 32 && c < 127) ? c : '.');
        }
      
      printf (\"\\n\");
    }
  printf (\"\\n\");
}

void
debugger_print_program_memory (debugger_t *dbg, int start, int length)
{
  printf (\"\\nProgram Memory [0x%04X - 0x%04X]:\\n\", start, start + length - 1);
  
  for (int i = 0; i < length; i += 16)
    {
      printf (\"0x%04X: \", start + i);
      
      for (int j = 0; j < 16 && (i + j) < length; j++)
        {
          printf (\"%02X \", PROGMEM (*dbg->cpu->mem)[start + i + j]);
        }
      
      printf (\"\\n\");
    }
  printf (\"\\n\");
}

/* Status and statistics */
void
debugger_print_status (debugger_t *dbg)
{
  printf (\"\\n=== Debugger Status ===\\n\");
  printf (\"Mode: \");
  switch (dbg->mode)
    {
    case DBG_MODE_RUN:
      printf (\"RUN\\n\");
      break;
    case DBG_MODE_STEP:
      printf (\"STEP\\n\");
      break;
    case DBG_MODE_STEP_N:
      printf (\"STEP_N (%d remaining)\\n\", dbg->step_count);
      break;
    case DBG_MODE_CONTINUE:
      printf (\"CONTINUE\\n\");
      break;
    case DBG_MODE_PAUSE:
      printf (\"PAUSED\\n\");
      break;
    }
  
  printf (\"PC: 0x%04X\\n\", dbg->cpu->pc);
  printf (\"Previous PC: 0x%04X\\n\", dbg->prev_pc);
  printf (\"Last Instruction: %s\\n\", dbg->last_instruction);
  printf (\"Total Instructions: %lu\\n\", dbg->total_instructions);
  printf (\"Cycles: %lu\\n\", dbg->cycles);
  printf (\"Breakpoints: %d\\n\", dbg->num_breakpoints);
  printf (\"Watch: %s\\n\", dbg->watch_enabled ? \"Enabled\" : \"Disabled\");
  printf (\"======================\\n\\n\");
}

void
debugger_print_stats (debugger_t *dbg)
{
  printf (\"\\n=== Execution Statistics ===\\n\");
  printf (\"Total Instructions Executed: %lu\\n\", dbg->total_instructions);
  printf (\"Total Cycles (approx): %lu\\n\", dbg->cycles);
  printf (\"Current PC: 0x%04X\\n\", dbg->cpu->pc);
  
  if (dbg->num_breakpoints > 0)
    {
      printf (\"\\nBreakpoint Hits:\\n\");
      for (int i = 0; i < dbg->num_breakpoints; i++)
        {
          if (dbg->breakpoints[i].hit_count > 0)
            {
              printf (\"  %s (0x%04X): %d hits\\n\",
                      dbg->breakpoints[i].label,
                      dbg->breakpoints[i].address,
                      dbg->breakpoints[i].hit_count);
            }
        }
    }
  printf (\"===========================\\n\\n\");
}

/* Watch functionality */
void
debugger_enable_watch (debugger_t *dbg)
{
  dbg->watch_enabled = true;
  /* Save current register state */
  for (int i = 0; i < 32; i++)
    {
      dbg->watch_registers[i] = DATAMEM (*dbg->cpu->mem)[i];
      dbg->register_changed[i] = false;
    }
  printf (\"Register watching enabled\\n\");
}

void
debugger_disable_watch (debugger_t *dbg)
{
  dbg->watch_enabled = false;
  printf (\"Register watching disabled\\n\");
}

void
debugger_update_watch (debugger_t *dbg)
{
  if (!dbg->watch_enabled)
    return;
  
  for (int i = 0; i < 32; i++)
    {
      int current = DATAMEM (*dbg->cpu->mem)[i];
      if (current != dbg->watch_registers[i])
        {
          dbg->register_changed[i] = true;
        }
      else
        {
          dbg->register_changed[i] = false;
        }
    }
}

/* Simple disassembler */
const char *
debugger_disassemble_at (debugger_t *dbg, int address)
{
  static char disasm[64];
  
  unsigned char b1 = PROGMEM (*dbg->cpu->mem)[address];
  unsigned char b2 = PROGMEM (*dbg->cpu->mem)[address + 1];
  
  /* LDI */
  if (((b1 >> 4) & 0x0F) == 0b1110)
    {
      char k = ((b1 & 0xF) << 4) | (b2 & 0xF);
      char r = (b2 >> 4) & 0xF;
      snprintf (disasm, 64, \"LDI R%d, 0x%02X\", r + 16, k);
      return disasm;
    }
  
  /* ADD */
  if (((b1 >> 2) & 0x3F) == 0b000011)
    {
      char r = (((b1 >> 1) & 0x1) << 4) | (b2 & 0xF);
      char d = ((b1 & 0x1) << 4) | ((b2 >> 4) & 0xF);
      snprintf (disasm, 64, \"ADD R%d, R%d\", d, r);
      return disasm;
    }
  
  /* JMP */
  if (((b1 >> 1) & 0x7F) == 0b1001010 && (b2 & 0b1100) == 0b1100)
    {
      unsigned char b3 = PROGMEM (*dbg->cpu->mem)[address + 2];
      unsigned char b4 = PROGMEM (*dbg->cpu->mem)[address + 3];
      int j = b4 | ((int)b3 << 8) | ((b2 & 0x1) << 16)
              | (((b2 >> 4) & 0xF) << 17) | ((b1 & 0x1) << 21);
      snprintf (disasm, 64, \"JMP 0x%04X\", j);
      return disasm;
    }
  
  /* LDS */
  if (((b1 >> 1) & 0x7F) == 0b1001000 && (b2 & 0xF) == 0)
    {
      unsigned char b3 = PROGMEM (*dbg->cpu->mem)[address + 2];
      unsigned char b4 = PROGMEM (*dbg->cpu->mem)[address + 3];
      int k_loc = (int)b4 | ((int)b3 << 8);
      char r = b2 | ((b1 & 0x1) << 5);
      snprintf (disasm, 64, \"LDS R%d, 0x%04X\", r, k_loc);
      return disasm;
    }
  
  /* STS */
  if (((b1 >> 1) & 0x7F) == 0b1001001 && (b2 & 0xF) == 0)
    {
      unsigned char b3 = PROGMEM (*dbg->cpu->mem)[address + 2];
      unsigned char b4 = PROGMEM (*dbg->cpu->mem)[address + 3];
      int k_loc = (int)b4 | ((int)b3 << 8);
      char r = b2 | ((b1 & 0x1) << 5);
      snprintf (disasm, 64, \"STS 0x%04X, R%d\", k_loc, r);
      return disasm;
    }
  
  snprintf (disasm, 64, \"??? (0x%02X%02X)\", b1, b2);
  return disasm;
}

void
debugger_print_disassembly (debugger_t *dbg, int start, int count)
{
  printf (\"\\nDisassembly from 0x%04X:\\n\", start);
  printf (\"Address   Bytes         Instruction\\n\");
  printf (\"--------  ------------  -----------\\n\");
  
  int addr = start;
  for (int i = 0; i < count; i++)
    {
      unsigned char b1 = PROGMEM (*dbg->cpu->mem)[addr];
      unsigned char b2 = PROGMEM (*dbg->cpu->mem)[addr + 1];
      
      const char *instr = debugger_disassemble_at (dbg, addr);
      
      /* Determine instruction size */
      int size = 2;
      if (strncmp (instr, \"JMP\", 3) == 0 || 
          strncmp (instr, \"LDS\", 3) == 0 ||
          strncmp (instr, \"STS\", 3) == 0)
        {
          size = 4;
          unsigned char b3 = PROGMEM (*dbg->cpu->mem)[addr + 2];
          unsigned char b4 = PROGMEM (*dbg->cpu->mem)[addr + 3];
          printf (\"0x%04X    %02X %02X %02X %02X    %s\",
                  addr, b1, b2, b3, b4, instr);
        }
      else
        {
          printf (\"0x%04X    %02X %02X          %s\",
                  addr, b1, b2, instr);
        }
      
      if (addr == dbg->cpu->pc)
        printf (\" <-- PC\");
      
      printf (\"\\n\");
      addr += size;
    }
  printf (\"\\n\");
}

/* Interactive debugger shell */
void
debugger_interactive (debugger_t *dbg)
{
  char cmd[256];
  
  printf (\"\\n=== ATMega33 Interactive Debugger ===\\n\");
  printf (\"Type 'help' for available commands\\n\\n\");
  
  debugger_print_status (dbg);
  
  while (1)
    {
      printf (\"(dbg) \");
      if (!fgets (cmd, sizeof (cmd), stdin))
        break;
      
      /* Remove newline */
      cmd[strcspn (cmd, \"\\n\")] = 0;
      
      if (strlen (cmd) == 0)
        continue;
      
      /* Parse command */
      char *token = strtok (cmd, \" \");
      
      if (strcmp (token, \"help\") == 0 || strcmp (token, \"h\") == 0)
        {
          printf (\"\\nAvailable commands:\\n\");
          printf (\"  help, h              - Show this help\\n\");
          printf (\"  step, s              - Execute one instruction\\n\");
          printf (\"  step N               - Execute N instructions\\n\");
          printf (\"  continue, c          - Continue until breakpoint\\n\");
          printf (\"  break ADDR [LABEL]   - Set breakpoint at address\\n\");
          printf (\"  delete N             - Delete breakpoint N\\n\");
          printf (\"  enable N             - Enable breakpoint N\\n\");
          printf (\"  disable N            - Disable breakpoint N\\n\");
          printf (\"  breakpoints, b       - List all breakpoints\\n\");
          printf (\"  registers, r         - Show all registers\\n\");
          printf (\"  register N           - Show register N\\n\");
          printf (\"  changed              - Show changed registers\\n\");
          printf (\"  memory ADDR LEN      - Show data memory\\n\");
          printf (\"  program ADDR LEN     - Show program memory\\n\");
          printf (\"  disasm [ADDR] [CNT]  - Disassemble instructions\\n\");
          printf (\"  watch on/off         - Enable/disable register watching\\n\");
          printf (\"  status               - Show debugger status\\n\");
          printf (\"  stats                - Show execution statistics\\n\");
          printf (\"  quit, q              - Exit debugger\\n\");
          printf (\"\\n\");
        }
      else if (strcmp (token, \"quit\") == 0 || strcmp (token, \"q\") == 0)
        {
          printf (\"Exiting debugger\\n\");
          break;
        }
      else if (strcmp (token, \"step\") == 0 || strcmp (token, \"s\") == 0)
        {
          char *arg = strtok (NULL, \" \");
          if (arg)
            {
              int n = atoi (arg);
              debugger_step_n (dbg, n);
            }
          else
            {
              debugger_step (dbg);
            }
        }
      else if (strcmp (token, \"continue\") == 0 || strcmp (token, \"c\") == 0)
        {
          debugger_continue (dbg);
        }
      else if (strcmp (token, \"break\") == 0)
        {
          char *addr_str = strtok (NULL, \" \");
          char *label = strtok (NULL, \" \");
          if (addr_str)
            {
              int addr = (int)strtol (addr_str, NULL, 0);
              debugger_add_breakpoint (dbg, addr, label);
            }
          else
            {
              printf (\"Usage: break ADDR [LABEL]\\n\");
            }
        }
      else if (strcmp (token, \"delete\") == 0)
        {
          char *idx_str = strtok (NULL, \" \");
          if (idx_str)
            {
              int idx = atoi (idx_str);
              debugger_remove_breakpoint (dbg, idx);
            }
          else
            {
              printf (\"Usage: delete N\\n\");
            }
        }
      else if (strcmp (token, \"enable\") == 0)
        {
          char *idx_str = strtok (NULL, \" \");
          if (idx_str)
            {
              int idx = atoi (idx_str);
              debugger_enable_breakpoint (dbg, idx);
            }
          else
            {
              printf (\"Usage: enable N\\n\");
            }
        }
      else if (strcmp (token, \"disable\") == 0)
        {
          char *idx_str = strtok (NULL, \" \");
          if (idx_str)
            {
              int idx = atoi (idx_str);
              debugger_disable_breakpoint (dbg, idx);
            }
          else
            {
              printf (\"Usage: disable N\\n\");
            }
        }
      else if (strcmp (token, \"breakpoints\") == 0 || strcmp (token, \"b\") == 0)
        {
          debugger_list_breakpoints (dbg);
        }
      else if (strcmp (token, \"registers\") == 0 || strcmp (token, \"r\") == 0)
        {
          debugger_print_registers (dbg);
        }
      else if (strcmp (token, \"register\") == 0)
        {
          char *reg_str = strtok (NULL, \" \");
          if (reg_str)
            {
              int reg = atoi (reg_str);
              debugger_print_register (dbg, reg);
            }
          else
            {
              printf (\"Usage: register N\\n\");
            }
        }
      else if (strcmp (token, \"changed\") == 0)
        {
          debugger_print_changed_registers (dbg);
        }
      else if (strcmp (token, \"memory\") == 0)
        {
          char *addr_str = strtok (NULL, \" \");
          char *len_str = strtok (NULL, \" \");
          if (addr_str && len_str)
            {
              int addr = (int)strtol (addr_str, NULL, 0);
              int len = atoi (len_str);
              debugger_print_memory (dbg, addr, len);
            }
          else
            {
              printf (\"Usage: memory ADDR LEN\\n\");
            }
        }
      else if (strcmp (token, \"program\") == 0)
        {
          char *addr_str = strtok (NULL, \" \");
          char *len_str = strtok (NULL, \" \");
          if (addr_str && len_str)
            {
              int addr = (int)strtol (addr_str, NULL, 0);
              int len = atoi (len_str);
              debugger_print_program_memory (dbg, addr, len);
            }
          else
            {
              printf (\"Usage: program ADDR LEN\\n\");
            }
        }
      else if (strcmp (token, \"disasm\") == 0)
        {
          char *addr_str = strtok (NULL, \" \");
          char *cnt_str = strtok (NULL, \" \");
          int addr = addr_str ? (int)strtol (addr_str, NULL, 0) : dbg->cpu->pc;
          int cnt = cnt_str ? atoi (cnt_str) : 10;
          debugger_print_disassembly (dbg, addr, cnt);
        }
      else if (strcmp (token, \"watch\") == 0)
        {
          char *arg = strtok (NULL, \" \");
          if (arg && strcmp (arg, \"on\") == 0)
            debugger_enable_watch (dbg);
          else if (arg && strcmp (arg, \"off\") == 0)
            debugger_disable_watch (dbg);
          else
            printf (\"Usage: watch on/off\\n\");
        }
      else if (strcmp (token, \"status\") == 0)
        {
          debugger_print_status (dbg);
        }
      else if (strcmp (token, \"stats\") == 0)
        {
          debugger_print_stats (dbg);
        }
      else
        {
          printf (\"Unknown command: %s (type 'help' for commands)\\n\", token);
        }
    }
}

/* Execution control - simplified stubs for now */
void
debugger_step (debugger_t *dbg)
{
  printf (\"Stepping one instruction...\\n\");
  /* This would call the modified cpu_ip_start with step support */
  printf (\"Note: Full step execution requires CPU integration\\n\");
  debugger_print_disassembly (dbg, dbg->cpu->pc, 1);
}

void
debugger_step_n (debugger_t *dbg, int n)
{
  printf (\"Stepping %d instructions...\\n\", n);
  for (int i = 0; i < n; i++)
    {
      debugger_step (dbg);
    }
}

void
debugger_continue (debugger_t *dbg)
{
  printf (\"Continuing execution until breakpoint...\\n\");
  dbg->mode = DBG_MODE_CONTINUE;
}

void
debugger_pause (debugger_t *dbg)
{
  dbg->mode = DBG_MODE_PAUSE;
  printf (\"Execution paused\\n\");
}

void
debugger_run (debugger_t *dbg)
{
  dbg->mode = DBG_MODE_RUN;
  printf (\"Running...\\n\");
}
