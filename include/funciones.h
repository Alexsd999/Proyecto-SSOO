#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "cab.h"

void login(char *nombre, int *pin);
int validaPIN(char *pin);
int verificaCliente(Cliente *c);
int cuentasCliente(int *id_cliente);
int menuOpciones();
long muestraImportes();
int escr_msg(int qid, Mensaje *msg);
int leer_msg(int qid,long tipo,Mensaje *msg);

#endif
