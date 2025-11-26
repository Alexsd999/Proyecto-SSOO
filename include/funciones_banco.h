#ifndef FUNCIONES_BANCO_H
#define FUNCIONES_BANCO_H

#include "cab.h"

void creaRecursos();
void eliminaRecursos();
int inicializarCuentas(Banco *b1,Cliente listaClientes[MAX_CLI]);
void cierreBanco(int);
void manejadorSIGINT();
int escr_msg(int qid, Mensaje *msg);
int leer_msg(int qid,long tipo,Mensaje *msg);
Cuenta *buscaCuentaCliente(Banco *b1,int id_cli);
Cuenta *buscaCuentaDestino(Banco *b1,int cuenta_dest);
int procesaOperacion(Banco *b1,Mensaje *msg);
#endif
