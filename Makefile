CC=g++-10
CLAGS=-Ofast
FILES=lol.cpp

lol: clean
	$(CC) $(CFLAGS) lol.cpp -o $@

alt: clean_alt
	$(CC) $(CFLAGS) alt.cpp -o $@


clean:
ifneq (,$(wilcard ./lol))
	rm lol
endif

clean_alt:
ifneq (,$(wilcard ./alt))
	rm alt
endif
