# Makefile para ajilizar los pasos

cliente: build/cliente.o build/funciones.o
	gcc -Wall -o bin/cliente build/cliente.o build/funciones.o -lsqlite3

build/cliente.o: src/cliente.c include/cab.h include/funciones.h
	gcc -Wall -c src/cliente.c -o build/cliente.o

build/funciones.o: src/funciones.c include/cab.h include/funciones.h
	gcc -Wall -c src/funciones.c -o build/funciones.o

borrar:
	rm build/*.o bin/*
