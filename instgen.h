#if !defined(INSTGEN_H)
#define INSTGEN_H

#include "header.h"

/* ldi: 1110 kkkk rrrr kkkk */
#define LDI_Rr_K(r, K)                                                        \
  (0b1110 << 12) | (((int)(K >> 4) & 0xF) << 8) | (((r - 15) & 0xF) << 4)     \
      | (K & 0xF)

/* add: 0000 11rd dddd rrrr */
#define ADD_Rd_Rr(d, r)                                                       \
  (0b11 << 10) | (((int)(r >> 4) & 0x1) << 9) | (((int)(d) & 0x1F) << 4)      \
      | (r & 0xF)

/* jmp: 1001 010k kkkk 110k kkkk kkkk kkkk kkkk */
#define JMP_K(K)                                                              \
  0x940C0000 | (((((int)K) >> 17) & 0x1F) << 20) | (((int)K) & 0x1FF)

#endif // INSTGEN_H
