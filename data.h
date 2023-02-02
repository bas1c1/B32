#define s8 unsigned char
#define s16 unsigned int
#define s32 unsigned long int
#define s64 unsigned long long
#define abstract /*JUST COMMENT*/
#define REGISTER /*JUST COMMENT*/
#define INSTR /*JUST COMMENT*/
#define MEMSIZE 	65535
#define PROGMEMADDR 794
#define VIDMEMADDR  25
#define VARMEMADDR  16795
#define FREEMEMADDR 20796
#define FFREGADDR 	0

abstract INSTR
	#define C_MOV 	0x01
	#define C_SET 	0x02
	#define C_GET 	0x05
	#define C_STOR	0x06
	#define C_JMP   0x07
	#define C_JMPZ  0x08
	#define C_GRT   0x09
	#define C_LT    0x0a
	#define C_EQ	0x0b
	#define C_NEQ	0x0c
	#define C_MGRT  0x0d
	#define C_MLT   0x0e
	#define C_MEQ   0x0f
	#define C_MNEQ	0x10
	#define C_ADD	0x11
	#define C_SUB 	0x12
	#define C_MUL   0x13
	#define C_DIV 	0x14
	#define C_MADD	0x15
	#define C_MSUB 	0x16
	#define C_MMUL  0x17
	#define C_MDIV 	0x18
	#define C_HLT 	0x1b