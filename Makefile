# -*- MakeFile -*-

all: run

run: build/emulator.exe
	cd build && wine emulator.exe

build/emulator.exe: build/arithmetic.o build/dataflow.o build/main.o build/move.o build/transput.o
	x86_64-w64-mingw32-gcc build/arithmetic.o build/dataflow.o build/main.o build/move.o build/transput.o -o build/emulator.exe -lws2_32

build/main.o: src/main.c src/arithmetic.h src/dataflow.h src/main.h src/move.h src/transput.h
	x86_64-w64-mingw32-gcc -c src/main.c -o build/main.o -lws2_32

build/arithmetic.o: src/arithmetic.c src/arithmetic.h src/main.h
	x86_64-w64-mingw32-gcc -c src/arithmetic.c -o build/arithmetic.o -lws2_32

build/dataflow.o: src/dataflow.c src/dataflow.h src/main.h
	x86_64-w64-mingw32-gcc -c src/dataflow.c -o build/dataflow.o -lws2_32

build/move.o: src/move.c src/move.h src/main.h
	x86_64-w64-mingw32-gcc -c src/move.c -o build/move.o -lws2_32

build/transput.o: src/transput.c src/transput.h src/main.h
	x86_64-w64-mingw32-gcc -c src/transput.c -o build/transput.o -lws2_32
