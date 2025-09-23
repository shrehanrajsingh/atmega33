#if !defined(MEMORY_H)
#define MEMORY_H

#include "header.h"

/* Flash */
#define FLASH_SIZE 1 << 15
typedef struct
{
  char v[FLASH_SIZE];
} flash_t;

/* Program Memory Map */
typedef struct
{
  flash_t flash;
} pmm_t;

/* Memory Layouts */
#define PMM_AFS 0x0000 /* application flash section */
#define PMM_BFS 0x3FFF /* boot flash section */

/* Data Memory Map */
#define RFIOM_LOCS_SIZE 96
#define SRAM_INT_SIZE 2048
typedef struct
{
  char v[RFIOM_LOCS_SIZE + SRAM_INT_SIZE];
} dmm_t;

/* Memory Layouts */
#define RF_r0 0x00
#define RF_r1 0x01
#define RF_r2 0x02
#define RF_r3 0x03
#define RF_r4 0x04
#define RF_r5 0x05
#define RF_r6 0x06
#define RF_r7 0x07
#define RF_r8 0x08
#define RF_r9 0x09
#define RF_r10 0x0A
#define RF_r11 0x0B
#define RF_r12 0x0C
#define RF_r13 0x0D
#define RF_r14 0x0E
#define RF_r15 0x0F
#define RF_r16 0x10
#define RF_r17 0x11
#define RF_r18 0x12
#define RF_r19 0x13
#define RF_r20 0x14
#define RF_r21 0x15
#define RF_r22 0x16
#define RF_r23 0x17
#define RF_r24 0x18
#define RF_r25 0x19
#define RF_r26 0x1A
#define RF_r27 0x1B
#define RF_r28 0x1C
#define RF_r29 0x1D
#define RF_r30 0x1E
#define RF_r31 0x1F

#define DMM_IO_START 0x20
#define DMM_ISRAM_START 0x60

#define SF_START 0x3F /* status register location */

#define SF_I(X) ((X)[SF_START] >> 7) /* Global Interrupt Enable */
#define SF_T(X) ((X)[SF_START] >> 6) /* Bit Copy Storage */
#define SF_H(X) ((X)[SF_START] >> 5) /* Half Carry Flag */
#define SF_S(X) ((X)[SF_START] >> 4) /* Sign Bit: S = N ^ V */
#define SF_V(X) ((X)[SF_START] >> 3) /* 2's Complement Overflow flag */
#define SF_N(X) ((X)[SF_START] >> 2) /* Negative Flag */
#define SF_Z(X) ((X)[SF_START] >> 1) /* Zero Flag */
#define SF_C(X) ((X)[SF_START])      /* Carry Flag */

#define RAMEND 0x85F
#define SP_START RAMEND

/* EEPROM */
#define EEPROM_SIZE 1024
typedef struct
{
  char v[EEPROM_SIZE];
} eeprom_t;

/* memory */

#define PROGMEM(X) ((X).pmm.flash.v)
#define DATAMEM(X) ((X).dmm.v)
typedef struct
{
  dmm_t dmm;
  pmm_t pmm;
  eeprom_t eeprom;
} mem_t;

#if defined(__cplusplus)
extern "C"
{
#endif // __cplusplus

  void stack_push (mem_t *, char);
  char stack_peep (mem_t *);
  void stack_pop (mem_t *, char);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // MEMORY_H
