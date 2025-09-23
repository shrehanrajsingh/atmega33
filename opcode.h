#if !defined(OPCODE_H)
#define OPCODE_H

/* ldi: 1110 kkkk rrrr kkkk */
#define OPC_LDI 0xD000
/* add: 0000 11rd dddd rrrr */
#define OPC_ADD 0x0C00
/* jmp: 1001 010k kkkk 110k kkkk kkkk kkkk kkkk */
#define OPC_JMP 0x940C

#endif // OPCODE_H
