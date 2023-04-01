CC=g++
CLAGS=-Ofast
FILES=lol.cpp

lol: clean
	$(CC) $(CFLAGS) $(FILES) -o $@

clean:
ifneq (,$(wilcard ./lol))
	rm lol
endif