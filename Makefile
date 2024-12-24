ALL_ASM = $(wildcard *.asm) $(wildcard *.inc)

sprites: 
	./util/make-sprites.py

build: $(ALL_ASM) sprites
	cl65 -t cx16 -o XOLO.PRG -l ./listings/xolo.list ./src/xolo.asm

emu : build 
	~/emu/x16/x16-47/x16emu -prg XOLO.PRG -run

clean:
	rm -f *.PRG list/*.list *.o
