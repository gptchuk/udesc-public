; R0 = 0
	LOAD 0
	XTO R0
	
; while(R0 < 3){R0=R0+1}
while1:
	LOAD 3
	XTO Y
	TOX R0
	LT
	IF
	GOTO fim_w1
	LOAD 1
	XTO Y
	TOX R0
	ADD
	XTO R0
	GOTO while1
	

fim_w1:
	GOTO fim_w1
