
    AREA  PLTentry, CODE
    CODE16
    
    BX    PC
    NOP

    CODE32
    ;LDR   R12, =function ;GOT
    LDR   R12, [PC, #0]
    BX    R12

    
    END