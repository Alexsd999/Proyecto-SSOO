#include "../include/cab.h"
#include "../include/funciones.h"

// Obtiene credenciales
void login(char *nombre, int *pin){
  printf("\n\n==== Bienvenido a ETSIBank ====\n\nPor favor identifiquese:\nUsuario:");
  scanf("%s",nombre);
  char *clave=getpass("PIN: ");

  if(!validaPIN(clave)){
    printf("[ERROR] La clave PIN debe tener solamente 4 digitos.\n");
    exit(1);
  }

  *pin=atoi(clave);
  printf("\n");
  
}

int validaPIN(char *pin){
  if(strlen(pin)!=4)
    return 0;
  for(int i=0;i<4;i++){
    if(!isdigit(pin[i]))
      return 0;
  }
  return 1;
}
// Comprobar si cliente está en bbbdd
int verificaCliente(Cliente *c){
  sqlite3 *db;
  sqlite3_stmt *stmt;
  int existe=0;

  if(sqlite3_open("data/clientes.db",&db)!=SQLITE_OK){
    //perror("[ERROR] Error con la base de datos");
    printf("[ERROR] Error con la base de datos: %s\n",sqlite3_errmsg(db));
    exit(1);
  }
  
  char *consulta="SELECT id_cli FROM clientes WHERE nombre=? AND pin=?;";

  if(sqlite3_prepare_v2(db,consulta,-1,&stmt,NULL)!=SQLITE_OK){
    //perror("[ERROR] Error con la consulta");
    printf("[ERROR] Error con la consulta: %s\n",sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }
  sqlite3_bind_text(stmt,1,c->nombre,-1,SQLITE_STATIC);
  sqlite3_bind_int(stmt,2,c->pin);

  if(sqlite3_step(stmt)==SQLITE_ROW){
    c->id_cli=sqlite3_column_int(stmt,0);
    existe=1;
  }
  
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  
  return existe;
}
// Consulta y muestra las cuentas del cliente
int cuentasCliente(int *id_cliente){
  sqlite3 *db;
  sqlite3_stmt *stmt;
  int total=0;
  int cuentas_id[MAX_CUENTAS];
  int ncuenta;
  
  if(sqlite3_open("data/clientes.db",&db)!=SQLITE_OK){
    //perror("[ERROR] Error con la base de datos");
    printf("[ERROR] Error con la base de datos: %s\n",sqlite3_errmsg(db));
    exit(1);
  }
  
  char *consulta="SELECT id_cuenta,saldo,num_cuenta FROM cuentas WHERE id_cliente=?;";

  if(sqlite3_prepare_v2(db,consulta,-1,&stmt,NULL)!=SQLITE_OK){
    // perror("[ERROR] Error con la consulta");
    printf("[ERROR] Error con la consulta: %s\n",sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }
  
  sqlite3_bind_int(stmt,1,*id_cliente);
  printf("\n=== Cuentas asociadas ===\n");
  printf("%-12s %-34s %-19s %s\n","Nº","Número de Cuenta","Saldo (€)","Estado");
  printf("---------------------------------------------------------------------------------\n");
  
  while(sqlite3_step(stmt)==SQLITE_ROW){
    int id_cuenta=sqlite3_column_int(stmt,0);
    double saldo=sqlite3_column_double(stmt,1);
    char *num_cuenta=(char *)sqlite3_column_text(stmt,2);
    //const unsigned char *num_cuenta=sqlite3_column_text(stmt,1);

    cuentas_id[total]=id_cuenta;

    char *estado= (saldo < 0) ? "INACTIVA" : "ACTIVA";
    total++;

    printf("%-4d %-40s %-16.2f %s\n",total,num_cuenta,saldo,estado);
    //printf("%d Cuenta %s | Saldo: %.2f € | Estado: %s\n",total,num_cuenta,saldo,estado);
    
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  
  if(total==0){
    printf("[ERROR] Este cliente no tiene cuentas.\n");
    exit(1);
  }
  
  printf("Seleccione la cuenta a la que desea acceder: ");
  scanf("%d",&ncuenta);

  while(ncuenta<1 || ncuenta>total){
    printf("Opción invalida, seleccione otra:");
    scanf("%d",&ncuenta);
  }
  
  return cuentas_id[ncuenta-1];
}
// Muestra opciones a realizar
int menuOpciones(){
  int opcion;

  printf("\n¿Qué operación desea hacer?\n");
  printf("1. Ingresar dinero\n");
  printf("2. Retirar dinero\n");
  printf("3. Trasferir dinero\n");
  printf("4. Mostrar últimos cinco movimientos.\n");
  printf("5. Salir\n");
  printf("Seleccione una opción: ");
  scanf("%d",&opcion);
  
  return opcion;
}
// Función generica para importes
long muestraImportes(){
  int opcion;
  long cantidad;
  int valido=0;
  char importe[20];
  
  printf("\n\n%-4s %-12s %-4s %-12s %-4s %s","1."," 10€ ","3.","50€","5.","1000€");
  printf("\n%-4s %-12s %-4s %-12s %-4s %s\n\n","2."," 25€ ","4.","500€","6.","Otro importe");
  printf("Indique una opción: ");
  scanf("%d",&opcion);

  switch(opcion){
  case 1:
    return 10;
  case 2:
    return 25;
  case 3:
    return 50;
  case 4:
    return 100;
  case 5:
    return 500;
  case 6:
    do{
      printf("Introduzca importe:");
      scanf("%s",importe);

      valido=1;
      for(int i=0;importe[i]!='\0';i++){
	if(!isdigit(importe[i])){
	  printf("[ERROR] Por favor, introduzca dígitos.\n");
	  valido=0;
	  break;
	}
      }

      if(valido){
	cantidad=atoi(importe);
	if(cantidad<=0){
	  printf("[ERROR] La cantidad debe ser mayor o igual a 0.\n");
	  valido=0;
	}
      }
    }while(!valido);
    return cantidad;
  default:
    printf("[ERROR] Opción inválida.\n");
    return muestraImportes();
  }
}

int escr_msg(int qid, Mensaje *msg) { 
   int resultado;
   resultado=msgsnd(qid,msg,MSG_SIZE,0);
   return (resultado);
}

int leer_msg(int qid,long tipo,Mensaje *msg) { 
   int resultado;
   resultado=msgrcv(qid,msg,MSG_SIZE,tipo,0);  
   return (resultado); 
}

