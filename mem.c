#include "data.h"
#include "arch.h"

extern memory mem;

void start() {
	mem.pointer = 20796;
	for (int i = 0; i < MEMSIZE; i++)
		mem.data[i] = 0x00;
}

void push(s32 val) {
	next();

	mem.data[mem.pointer] = val;
}

s32 pop() {
	s32 temp = mem.data[mem.pointer];
	mem.data[mem.pointer] = 0x00;
	return temp;
}

void next() {
	mem.pointer++;
}

void back() {
	mem.pointer--;
}

s32 get(int index) {
	return mem.data[index];
}

void set(int index, s32 value) {
	mem.data[index] = value;
}