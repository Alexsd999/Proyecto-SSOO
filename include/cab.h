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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>

#define MAX_CLI 5
#define MAX_CUENTAS 50
#define MAX_MOVIMIENTOS 100
//TODO: Implementar para mas cuentas por cliente

#define INGRESAR 1
#define RETIRAR 2
#define TRANSFERIR 3
#define MOVIMIENTOS 4
#define SALIR 5
#define RUTA_CLAVE "."
#define COLA 'm'
#define MEMORIA 'S'
#define MSG_SIZE (sizeof(Mensaje)-sizeof(long))

// Estructura de una cuenta
typedef struct{
  int id_cuenta;
  int id_cliente;
  double saldo;
  char num_cuenta[32];
  char nombre[30];
  int activa; // Quitar si no se va a usar
} Cuenta;
//Estructura de una operación
typedef struct{
  int tipo;
  double cantidad;
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
//Estructra para las colas de mensajes, buffer
typedef struct{
  long mtype;
  int id_cliente;
  Operacion op;
} Mensaje;
//Estructura para historial
typedef struct{
  
} Historial; 

#endif
