default: all

all: clean ram_predictor.o ram_predictor cache_predictor.o cache_predictor

clean:
	rm -rf ram_predictor ram_predictor.o cache_predictor cache_predictor.o

ram_predictor.o: ram_predictor.c
	gcc -O0 -g -c -Wall -Werror -fpic  -o ram_predictor.o ram_predictor.c

ram_predictor: ram_predictor.o
	gcc -g -o ram_predictor ram_predictor.o

cache_predictor.o: cache_predictor.c
	gcc -O0 -g -c -Wall -Werror -fpic  -o cache_predictor.o cache_predictor.c
	
cache_predictor: cache_predictor.o
	gcc -g -o cache_predictor cache_predictor.o

