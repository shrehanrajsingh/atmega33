#if !defined(OPCODE_H)
#define OPCODE_H

/* ldi: 1110 kkkk rrrr kkkk */
#define OPC_LDI 0xD000
/* add: 0000 11rd dddd rrrr */
#define OPC_ADD 0x0C00
/* jmp: 1001 010k kkkk 110k kkkk kkkk kkkk kkkk */
#define OPC_JMP 0x940C0000
/* lds: 1001 000d dddd 0000 kkkk kkkk kkkk kkkk */
#define OPC_LDS 0x90000000
/* sts: 1001 001r rrrr 0000 kkkk kkkk kkkk kkkk */
#define OPC_STS 0x92000000

#endif // OPCODE_H
