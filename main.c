#include "data.h"
#include "arch.h"

#include <stdio.h>
//#include <limits.h>
/*void memcpy(s8 *f, s8 *s, size_t size) {
	while (size--)
        *f++ = *s++;
}*/

extern regs reg;
/*
s32 RGBA2DWORD(int iR, int iG, int iB, int iA)
{
    return ((iA * 256 + iR) * 256 + iG) * 256 + iB;
}

struct RGBACOLOR {
    int r;
    int g;
    int b;
    int a;
};

struct RGBACOLOR color_new(int iR, int iG, int iB, int iA) {
    struct RGBACOLOR col;
    col.r = iR;
    col.g = iG;
    col.b = iB;
    col.a = iA;
    return col;
}

s16 fromRGB(s16 r, s16 g, s16 b)
{
    s16 result = 0;
    result |= (r & 0xFF) << 16;
    result |= (g & 0xFF) << 8;
    result |= (b & 0xFF);
    return result;
}
 
int *toRGB(s16 c)
{
    int rgb[3];
    rgb[0] = (c >> 16) & 0xFF;
    rgb[1] = (c >> 8) & 0xFF;
    rgb[2] = (c) & 0xFF;
    return rgb;
}*/

s32 s8ts32(s8 *value) {
	s32 l = 0;
	for (int i = 0; i < 4; ++i)
		l = l | ((s32)value[i] << (8 * i));
	return l;
}

s16 s8tsm32(s8 *value) {
	s16 l = 0;
	for (int i = 0; i < 2; ++i)
		l = l | ((s16)value[i] << (8 * i));
	return l;
}

int s8ti(s8 *value) {
	return (int)(value[0] << 24 | value[1] << 16 | value[2] << 8 | value[3]);
}

int s32ti(s32 value) {
	return s8ti((s8*)&value);
}

void s8afree(s8 *value) {
	value[0] = 0x00;
	value[1] = 0x00;
	value[2] = 0x00;
	value[3] = 0x00;
}

#include <fcntl.h>

volatile s64 CODESSIZE = 0;
volatile int isDebug = 0;

s64 getFileSize(const char* file_name){
	s64 _file_size = 0;
	FILE* fd = fopen(file_name, "rb");
	if(fd == NULL){
		_file_size = -1;
	}
	else{
		while(getc(fd) != EOF)
			_file_size++;
		fclose(fd);
	}
	return _file_size;
}
#ifndef UNIX
	#include <windows.h>
#endif
void WriteCharToXY(int X, int Y, char C){ 
	#if defined(linux) || defined(__linux)
		printf("\033[%d;%df", X, Y);
		putchar(C);
		return;
	#else
		HANDLE StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		struct _CONSOLE_SCREEN_BUFFER_INFO BufferInfo;
		GetConsoleScreenBufferInfo(StdOut, &BufferInfo);
		struct _COORD OldPos = BufferInfo.dwCursorPosition;
		struct _COORD NewPos;
		NewPos.X = X;
		NewPos.Y = Y;
		SetConsoleCursorPosition(StdOut, NewPos);
		DWORD NumWritten;
		WriteConsoleA(StdOut, &C, 1, &NumWritten, NULL);
		SetConsoleCursorPosition(StdOut, OldPos);
		return;
	#endif
}

void handlevidmem() {
	char ch;
	int t;
	for (int i = VIDMEMADDR; i < 794; i++) {
		t = get(i);
		ch = s8ti((s8*)&t);
		if (ch != 0) {
			WriteCharToXY(i-25, (i-25)/256, ch);
		}
	}
}

void checkCodes() {
	int p = PROGMEMADDR;
	s8 buffer[4];
	s8 buffer_addr[4];
	s8 buffer_val[4];
	short addr;
	int __tempc1__;
	int __tempc2__;
	for (; p < CODESSIZE+PROGMEMADDR; p++) {
		switch (get(p)) {
			case C_MOV:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, s8ts32(buffer));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, s8ts32(buffer));
				p--;
				s8afree(&buffer);
				break;
			case C_SET:
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer_addr[i] = get(p);
				for (int i = 0; i < 4; i++, p++)
					buffer_val[i] = get(p);
				if (isDebug == 1)
					printf("MEM:%d - %x\n", s8ti(buffer_addr), s8ts32(buffer_val));
				set(s8ti(buffer_addr), s8ts32(buffer_val));
				p--;
				if (s8ti(buffer_addr) >= 25 && s8ti(buffer_addr) <= 793)
					handlevidmem();
				s8afree(&buffer_addr);
				s8afree(&buffer_val);
				break;
			case C_STOR:
				p++;
				addr = get(p);
				s32 regval = reg_read(addr);
				int mem_addr = stor(addr);
				if (isDebug == 1)
					printf("MEM:%d - %x\n", mem_addr, regval);
				//p--;
				break;
			case C_GET:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, get(s8ti(buffer)));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, get(s8ti(buffer)));
				p--;
				s8afree(&buffer);
				break;
			case C_JMP:
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				p = s8ti(buffer)+PROGMEMADDR;
				if (isDebug == 1)
					printf("JMP:%x\n", s8ti(buffer));
				p--;
				s8afree(&buffer);
				break;
			case C_JMPZ:
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				if (get(FFREGADDR) == 0) {
					p = s8ti(buffer)+PROGMEMADDR;
					if (isDebug == 1)
						printf("JMP:%x\n", s8ti(buffer));
				}
				p--;
				s8afree(&buffer);
				break;
			case C_GRT:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) > s8ti(buffer)) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;
			case C_LT:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) < s8ti(buffer)) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;
			case C_EQ:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) == s8ti(buffer)) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;
			case C_NEQ:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) != s8ti(buffer)) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;

			case C_MGRT:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) > s32ti(get(s8ti(buffer)))) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;
			case C_MLT:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) < s32ti(get(s8ti(buffer)))) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;
			case C_MEQ:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) == s32ti(get(s8ti(buffer)))) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;
			case C_MNEQ:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				__tempc1__ = reg_read(addr);
				if (s8ti((s8*)&__tempc1__) != s32ti(get(s8ti(buffer)))) {
					set(FFREGADDR, 0x00);
					if (isDebug == 1)
						printf("FF:%x\n", get(FFREGADDR));
				}
				p--;
				s8afree(&buffer);
				break;

			case C_ADD:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, reg_read(addr) + s8ts32(buffer));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;
			case C_MUL:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, reg_read(addr) * s8ts32(buffer));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;
			case C_SUB:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, reg_read(addr) - s8ts32(buffer));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;
			case C_DIV:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				if (s8ts32(buffer) != 0)
					mov(addr, reg_read(addr) / s8ts32(buffer));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;

			case C_MADD:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, reg_read(addr) + s32ti(get(s8ti(buffer))));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;
			case C_MMUL:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, reg_read(addr) * s32ti(get(s8ti(buffer))));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;
			case C_MSUB:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				mov(addr, reg_read(addr) - s32ti(get(s8ti(buffer))));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;
			case C_MDIV:
				p++;
				addr = get(p);
				p++;
				for (int i = 0; i < 4; i++, p++)
					buffer[i] = get(p);
				if (s32ti(get(s8ti(buffer))) != 0)
					mov(addr, reg_read(addr) / s32ti(get(s8ti(buffer))));
				if (isDebug == 1)
					printf("REG:%d - %x\n", addr, reg_read(addr));
				p--;
				s8afree(&buffer);
				break;
			
			case C_HLT:
				for(;;);
		}
	}
}

int main(int argc, char const *argv[]) {
	int fd;
	CODESSIZE = getFileSize(argv[1]);
	if (argc > 2 && strcmp(argv[2], "debug-on") == 0)
		isDebug = 1;
    s8 c;
    fd = open(argv[1], O_RDONLY);
    for (int i = 0; i < CODESSIZE; i++) {
    	read(fd, &c, sizeof(c));
    	set(PROGMEMADDR+i, c);
    }
    close(fd);
    checkCodes();
    return 0;
}
