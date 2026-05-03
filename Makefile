CC=cl65
X16=~/src/x16/x16emu/x16emu -run -scale 2

xolo:
	$(CC) -O -o build/xolo.prg -t cx16  src/xolo.c src/maze.c src/wait.c src/vera-util.c

distrib-xolo: xolo
	mkdir -p distrib && \
	cp build/xolo.prg distrib

run-xolo: distrib-xolo
	cd distrib && $(X16) -debug -prg xolo.prg && cd -

clean:
	rm -f *.prg build/*.prg *.lbl build/*.lbl build/*.o distrib/*