
; exemplo usando IF

	load 	10			; X <- 10 (0xA)
	xto 	Y			; Y <- X
	load 	11			; X <- 11 (0xB)
	ge					; X <- X > Y
	if					; > if(X_0 == 1) salta
	goto 	senao		; > a proxima inst
	load 	10			; X <- 10
	xto		R0			; R0 <- X
	goto	fim			;
senao:		
	load 	11			; X <- 11
	xto		R0			; R0 <- X
fim:					; 
	goto 	fim			; loop infinito
	
