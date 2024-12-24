ALL_ASM = $(wildcard *.asm) $(wildcard *.inc)

all: $(ALL_ASM)
	cl65 -t cx16 -o XOLO.PRG -l ./listings/xolo.list ./src/xolo.asm

clean:
	rm -f *.PRG list/*.list *.o
