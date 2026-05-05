CC=cl65
X16=~/src/x16/x16emu/x16emu -run -scale 2

OBJS = build/xolo.o build/maze.o build/wait.o build/vera-util.o build/overlay.o build/tank.o build/enemy.o build/explosion.o

xolo: $(OBJS)
	$(CC) -o build/xolo.prg -t cx16 $(OBJS)

build/%.o: src/%.c
	$(CC) -O -c -o $@ -t cx16 $<

distrib: xolo
	mkdir -p distrib && \
	cp build/xolo.prg distrib && \
	cp assets/*.bin distrib

run: distrib
	cd distrib && $(X16) -debug -prg xolo.prg && cd -

clean:
	rm -f *.prg build/*.prg *.lbl build/*.lbl build/*.o src/*.o distrib/*