CC=cl65
X16=~/src/x16/x16emu/x16emu -run -scale 2

OBJS = build/xolo.o build/maze.o build/wait.o build/vera-util.o build/overlay.o

xolo: $(OBJS)
	$(CC) -o build/xolo.prg -t cx16 $(OBJS)

build/%.o: src/%.c
	$(CC) -O -c -o $@ -t cx16 $<

distrib-xolo: xolo
	mkdir -p distrib && \
	cp build/xolo.prg distrib

run-xolo: distrib-xolo
	cd distrib && $(X16) -debug -prg xolo.prg && cd -

clean:
	rm -f *.prg build/*.prg *.lbl build/*.lbl build/*.o src/*.o distrib/*