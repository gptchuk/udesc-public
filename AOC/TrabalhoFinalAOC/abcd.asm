start:
    NOP
    LOAD	0
    XTO		y
    XTO		m	; m é o ponteiro do endereço da ram, começa em 0
    XTO		r2	; contador do endereço da ram
    LOAD	1
while:
    XTO		r0
    ADD
    XTO		r1
    GE 			; verifica se a soma é maior que o anterior (verifica overflow)
    IF 			; se não houver overflow, pula a proxima instrução
    GOTO	fim ; se houve overflow, termina o programa
    TOX		r1
    MOVEXA	15
    LOAD	1 	; incremento pro endereço da ram
    XTO		y
    TOX		r2	; endereço armazenado
    ADD			; incrementa o endereço da ram	
    XTO		m	; carrega o endereço no ponteiro (registrador m)
    XTO		r2	; armazenado o novo endereço
    TOX		r0	; recupera o valor antigo do fibonacci
    XTO		y
    TOX		r1	; recupera o ultimo valor do fibonacci
    GOTO    while
fim:
    GOTO     fim