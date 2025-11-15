//Cabecera con estructuras compartidas
#ifndef CAB_H
#define CAB_H

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <sqlite3.h> // Deberá estar instalada con sudo apt-get install libsqlite3-dev
// Instalar tambien el programa de la db con sudo apt install sqlite3
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#define MAX_CLI 10
#define MAX_CUENTAS 50
//TODO: Implementar para mas cuentas por cliente

#define INGRESAR 0
#define RETIRAR 1
#define TRANSFERIR 2

// Estructura de una cuenta
typedef struct{
  int id_cuenta;
  int id_cliente;
  long saldo;
  char num_cuenta[32];
  int activa;
} Cuenta;
//Estructura de una operación
typedef struct{
  int tipo;
  long cantidad;
  int cuenta_orig;
  int cuenta_dest;
} Operacion;
//Estructura de Banco
typedef struct{
  int num_cuentas;
  Cuenta cuentas[MAX_CUENTAS];
} Banco;
//Estructura de Datos del cliente
typedef struct{
  int id_cli;
  char nombre[50];
  int pin;
} Cliente;

#endif
