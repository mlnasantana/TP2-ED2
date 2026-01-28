all: main.o intercalacao.o quicksortExt.o area.o geradorDeArquivos.o analiseExperimental.o
	@gcc main.o intercalacao.o quicksortExt.o area.o geradorDeArquivos.o analiseExperimental.o -o ordena -lm
	@rm -f main.o intercalacao.o quicksortExt.o area.o geradorDeArquivos.o analiseExperimental.o

main.o: main.c
	@gcc -c main.c -Wall -g

intercalacao.o: intercalacao.c
	@gcc -c intercalacao.c -Wall -g

quicksortExt.o: quicksortExt.c
	@gcc -c quicksortExt.c -Wall -g

area.o: area.c
	@gcc -c area.c -Wall -g

geradorDeArquivos.o: geradorDeArquivos.c
	@gcc -c geradorDeArquivos.c -Wall -g

analiseExperimental.o: analiseExperimental.c
	@gcc -c analiseExperimental.c -Wall -g

clean:
	@rm -f ordena *.o *.dat fita*.bin