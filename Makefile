clean:
	rm -rf ram_predictor ram_predictor.o

ram_predictor.o: ram_predictor.c
	gcc -O0 -g -c -Wall -Werror -fpic  -o ram_predictor.o ram_predictor.c

ram_predictor: ram_predictor.o
	gcc -g -o ram_predictor ram_predictor.o
