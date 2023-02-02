#include "data.h"

abstract typedef struct 
{
	/*
	FF: 0
	dop: 1-24 (incl)
	vidmem: 25-793 (incl)
	progmem: 794-16794 (incl)
	varmem: 16795-20795 (incl)
	free: 20796 - 65534 (incl)
	*/
	int pointer;
	s32 data[MEMSIZE];
	void (*push)(s32);
	void (*pop)();
	void (*next)();
	void (*back)();
	void (*start)();
	s32 (*get)(int);
	void (*set)(int, s32);
} memory;

abstract typedef struct
{
	s32 data[24];
	s32 (*reg_read)(short);
	void (*mov)(short, s32);
	int (*stor)(short);
	void (*load)(short, int);
} regs;