# Makefile para ajilizar los pasos
all: cliente banco
cliente: build/cliente.o build/funciones.o
	gcc -Wall -o bin/cliente build/cliente.o build/funciones.o -lsqlite3

build/cliente.o: src/cliente.c include/cab.h include/funciones.h
	gcc -Wall -c src/cliente.c -o build/cliente.o

build/funciones.o: src/funciones.c include/cab.h include/funciones.h
	gcc -Wall -c src/funciones.c -o build/funciones.o

banco: build/banco.o build/funciones_banco.o
	gcc -Wall -o bin/banco build/banco.o build/funciones_banco.o -lsqlite3

build/banco.o: src/banco.c include/cab.h include/funciones_banco.h
	gcc -Wall -c src/banco.c -o build/banco.o

build/funciones_banco.o: src/funciones_banco.c include/cab.h include/funciones_banco.h
	gcc -Wall -c src/funciones_banco.c -o build/funciones_banco.o

borrar:
	rm build/*.o bin/*
