#include "data.h"
#include "arch.h"

extern memory mem;
extern regs reg;

s32 reg_read(short index) {
	return reg.data[index];
}

void mov(short index, s32 value) {
	reg.data[index] = value;
}

int stor(short index) {
	push(reg_read(index));
	mov(index, 0x00);
	return mem.pointer;
}

void load(short index, int memindex) {
	mov(index, mem.get(memindex));
}