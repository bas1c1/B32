@echo off
@mkdir build
@gcc memdef.c data.h arch.h mem.c reg.c main.c -m64 -o winb32.exe
@pause
