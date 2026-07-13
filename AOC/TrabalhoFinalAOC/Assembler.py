
import os, sys
import codecs

INSTR = {
	"NOP" 	: 0,
	"LOAD"	: 1,
	"XTO"	: 2,
	"TOX"	: 3,
	"MOVEXA": 4,
	"MOVEAX": 5,
	"IF"  	: 6,
	"OPER"	: 7,
	"GOTO"	: 8,
	"GE"	: 0x70,
	"EQ"	: 0x71,
	"LE"	: 0x72,
	"ADD"	: 0x73,
	"BS"	: 0x74,
	"BC"	: 0x75,
}

REGISTERS = {
	"R0"	: 0,
	"R1"	: 1,
	"R2"	: 2,
	"R3"	: 3,
	"R4"	: 4,
	"PC1"	: 5,
	"M"		: 6,
	"Y"		: 7
}

def clearCodeLine(codeLineList):
	out = []
	for c in codeLineList:
		if c == ';':
			break
		out.append(c)
	return out
		
def getInstParam(codeLineList):
	inst = ""
	param = ""
	state = 0;
	print("".join(codeLineList))
	if(len(codeLineList) == 0):
		print("D1");
		return [None,None]
	for c in codeLineList:
		if c in [" ", "\t"]:
			state = 1
			continue
		if state == 0:
			inst = inst + c
		else:
			param = param + c
	if inst == "":
		return [None,None]
	if param == "":
		param = "0"
	return [inst.upper(), param]
	

fileName = sys.argv[1]
f = open(fileName, "r")
lines = f.readlines()

code = []
labels = {}
for line in lines:
	l = list(line.strip())
	l = clearCodeLine(l)
	if ':' in l:
		del l[-1]
		labels["".join(l)] = len(code)
	else:
		inst, param = getInstParam(l)
		if inst != None:
			code.append([inst, param])
		
		
		

outFileName = fileName.split(".")
outFileName[-1] = "cdm"
outFileName = ".".join(outFileName)
f = open(outFileName,"w")

lineCount = 0;
for i in range(len(code)):
	print("%04d: %-10s \t %-5s" % (lineCount, code[i][0], code[i][1]))
	if code[i][0] == "XTO" or code[i][0] == "TOX":
		code[i][1] = REGISTERS[code[i][1].upper()]
	if code[i][0] == "GOTO":
		if not code[i][1] in labels:
			print("Nao encontrei o label '%s'." % code[i][1])
			exit(0)
		code[i][1] = labels[code[i][1]]
		addr = int(code[i][1]) | 0x80
		f.write("%X: %X\n" % (i, addr))
	else:
		if(INSTR[code[i][0]] >= 0x70):
			code[i][0] = int(INSTR[code[i][0]])
			code[i][1] = code[i][0] & 0x0F
			code[i][0] = code[i][0] >> 4
			f.write("%X: %X%X\n" % (i, code[i][0], code[i][1]))
		else:
			f.write("%X: %X%X\n" % (i, INSTR[code[i][0]], int(code[i][1])))
	lineCount += 1
	
f.close()
	
#gerar codigo


	


