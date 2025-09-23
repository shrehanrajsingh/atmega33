; Calculate 3 * 100 with loops

.INCLUDE "M32DEF.INC"
.ORG 0

LDI R16, 100
LDI R17, 0

L1:
    ADD R17, 3
    DEC R16
    BRNE L1

; Nested Loop
LDI R16, 10
LDI R18, 0

L1:
    LDI R17, 100
    L2:
        ADD R18, 5
        DEC R17
        BRNE L2

    DEC R16
    BRNE L1

OUT PORTB, R18