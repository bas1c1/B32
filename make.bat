@echo off
@mkdir build
@gcc memdef.c data.h arch.h mem.c reg.c main.c -m64 -o build\b32.exe
@pause
