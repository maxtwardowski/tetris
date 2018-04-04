all: tetris

tetris: tetris.o primlib.o
	gcc -g -fsanitize=address $^ -o $@ -lm -lSDL -lpthread -lSDL_gfx

.c.o:
	gcc -g -fsanitize=address -Wall -pedantic -std=c99 -c -D_REENTRANT $<

primlib.o: primlib.c primlib.h

tetris.o: tetris.c primlib.h

clean:
	-rm primlib.o tetris.o tetris

# -Wall -pedantic -Werror
# -fsanitize=address
