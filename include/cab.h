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
#include <sys/msg.h>

#define MAX_CLI 10
#define MAX_CUENTAS 50
//TODO: Implementar para mas cuentas por cliente

#define INGRESAR 1
#define RETIRAR 2
#define TRANSFERIR 3
#define SALIR 4
#define RUTA_CLAVE "."
#define PROYECTO_CLAVE 'S'
#define MSG_SIZE (sizeof(msg)-sizeof(long))

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
  char cuenta_dest[32];
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
//Estryctyra para las colas de mensajes
typedef struct{
  long mtype;
  int id_cliente;
  Operacion op;
} Mensaje;

#endif
