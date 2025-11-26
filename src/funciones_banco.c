#include "../include/cab.h"
#include "../include/funciones_banco.h"

void creaRecursos(){
  sem_t *semID[3]={NULL,NULL,NULL};
  char *nom_sem[3]={"huecos","elementos","mutex"};
  int valor[3]={MAX_MOVIMIENTOS,0,1};
  key_t clave_cola=ftok(RUTA_CLAVE,COLA);
  int msgqueue_id;
  key_t clave_mem=ftok(RUTA_CLAVE,MEMORIA); 
  int shmid;

  msgqueue_id=msgget(clave_cola,0660);
  if(msgqueue_id!=-1)
      msgctl(msgqueue_id,IPC_RMID,NULL);
  // CREA COLA DE MENSAJES
  msgqueue_id=msgget(clave_cola,IPC_CREAT|IPC_EXCL|0660);
  
  if (msgqueue_id==-1){
    printf("Error al iniciar la cola.\n");
    exit(1);
  }

  shmid=shmget(clave_mem,sizeof(int)*(MAX_MOVIMIENTOS+2),0);
  if(shmid!=-1)
    shmctl(shmid,IPC_RMID,NULL);

  // CREA SEMAFOROS
  for(int i=0;i<3;i++){
    semID[i]=sem_open(nom_sem[i],O_CREAT,0600,valor[i]);
    if(semID[i]==NULL)
      printf("Error en la creación del semáforo '%s'.\n",nom_sem[i]);
  }

    
  // CREA MEMORIA COMPARTIDA
  if((shmid=shmget(clave_mem,sizeof(int)*(MAX_MOVIMIENTOS+2),IPC_CREAT|IPC_EXCL|0660))==-1){
    printf("El segmento de memoria compartida ya existe.\n");
  }
}

void eliminaRecursos(){
  char *nom_sem[3]={"huecos","elementos","mutex"};
  int sem_elim[3]={0,0,0};
  key_t clave_cola=ftok(RUTA_CLAVE,COLA); 
  key_t clave_mem=ftok(RUTA_CLAVE,MEMORIA); 
  int shmid;
  int elimina_mem=0,elimina_cola=0;

  //NOTA: ESCOGER SI QUEREMOS QUE SE MUESTREN LOS MENSAJES DE QUE SE ELIMINA CADA COSA

  // CIERRA SEMAFOROS
  for(int i=0;i<3;i++){
    sem_elim[i]=sem_unlink(nom_sem[i]);
    if(sem_elim[i]==0)
       printf("Se ha eliminado el semaforo '%s' correctamente.\n",nom_sem[i]);
    else
      printf("Error al eliminar el semaforo '%s'.\n",nom_sem[i]);
  }

  // BORRA MEMORIA COMPARTIDA
  if((shmid=shmget(clave_mem,sizeof(int)*(MAX_MOVIMIENTOS+2),0))!=-1){
    elimina_mem=shmctl(shmid,IPC_RMID,NULL);
    if(elimina_mem!=-1)
      printf("Segmento de memoria marcado para borrar.\n");
    else
      printf("[ERROR] No se ha podido eliminar el segmento de memoria correctamente.\n");
  }else
    printf("Segmento de memoria marcado no existe.\n");

  // BORRA COLA DE MENSAJES
  int msgqueue_id=msgget(clave_cola,0660);
  if(msgqueue_id!=-1){
    elimina_cola=msgctl(msgqueue_id,IPC_RMID,NULL);
    if(elimina_cola!=-1)
      printf("Cola marcada para borrar.\n");
    else
      printf("[ERROR] No se ha podido eliminar la cola de mensajes correctamente.\n");
  }else
    printf("Cola marcada no existe.\n");
}

void cierreBanco(int){
  printf("\nINFORMACION\nSeñal SIGINT recibida. Cierre del sistema, liberando recursos...\n\n");
  eliminaRecursos();
  exit(0);
  //sleep(3); QUITAR CUANDO SE VAYA A PRESENTAR, ES PARA DAR "REALISMO"
}

void manejadorSIGINT(){
  struct sigaction act;
  act.sa_handler=cierreBanco;
  act.sa_flags=0;
  sigemptyset(&act.sa_mask);

  sigaction(SIGINT,&act,NULL);
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



int inicializarCuentas(Banco *b1,Cliente listaClientes[MAX_CLI]){
  /*IMPORTANTE, ASUMIMOS QUE CADA CLIENTE TIENE AL MENOS UNA CUENTA!!*/
  sqlite3 *db;
  sqlite3_stmt *stmt1;//Opera con clientes
  sqlite3_stmt *stmt2;//Opera con cuentas
  int num_cli=0;
  int cuentas;

  //b1->num_cuentas=0;

  if(sqlite3_open("data/clientes.db",&db)!=SQLITE_OK){
    //perror("[ERROR] Error con la base de datos");
    printf("[ERROR] Error con la base de datos: %s\n",sqlite3_errmsg(db));
    exit(1);
  }

  char *consulta_cli="SELECT id_cli,nombre FROM clientes;";
  if(sqlite3_prepare_v2(db,consulta_cli,-1,&stmt1,NULL)!=SQLITE_OK){
    //perror("[ERROR] Error con la consulta");
    printf("[ERROR] Error con la consulta Clientes: %s\n",sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }

  while(sqlite3_step(stmt1)==SQLITE_ROW && num_cli<MAX_CLI){
    int id_cli=sqlite3_column_int(stmt1,0);
    char *nombre_cliente=(char *)sqlite3_column_text(stmt1,1);
    listaClientes[num_cli].id_cli=id_cli;
    strcpy(listaClientes[num_cli].nombre,nombre_cliente);
    num_cli++;
  }


  char *consulta_cuentas="SELECT id_cliente,id_cuenta,saldo,num_cuenta FROM cuentas;";

  if(sqlite3_prepare_v2(db,consulta_cuentas,-1,&stmt2,NULL)!=SQLITE_OK){
    //perror("[ERROR] Error con la consulta");
    printf("[ERROR] Error con la consulta: %s\n",sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(1);
  }
  cuentas=b1->num_cuentas;

  printf("\n%-20s %-12s %-40s %-12s\n","Cliente","Cuenta","Número de cuenta","Saldo (€)");
  printf("----------------------------------------------------------------------------------\n");
  while(sqlite3_step(stmt2)==SQLITE_ROW && cuentas<MAX_CUENTAS){
    int id_cli=sqlite3_column_int(stmt2,0);
    int id_cuenta=sqlite3_column_int(stmt2,1);
    double saldo=sqlite3_column_double(stmt2,2);
    char *num_cuenta=(char *)sqlite3_column_text(stmt2,3);
    char nombreCliente[30]="";

    for(int i=0;i<num_cli;i++){
      if(listaClientes[i].id_cli==id_cli)
	strcpy(nombreCliente,listaClientes[i].nombre);
    }
    
    b1->cuentas[cuentas].id_cliente=id_cli;
    b1->cuentas[cuentas].id_cuenta=id_cuenta;
    b1->cuentas[cuentas].saldo=saldo;
    strcpy(b1->cuentas[cuentas].nombre,nombreCliente);
    strcpy(b1->cuentas[cuentas].num_cuenta,num_cuenta);

    printf("%-20s %-12d %-40s %-12.2f\n",nombreCliente,id_cuenta,num_cuenta,saldo);
    cuentas++;
    
    
  }
  b1->num_cuentas=cuentas;
  printf("\n----------------------------------------------------------------------------------\n");
  printf("Se va mostrar el flujo de cada cliente:\n");
  
  sqlite3_finalize(stmt1);
  sqlite3_finalize(stmt2);
  sqlite3_close(db);

  return num_cli;
}

Cuenta *buscaCuentaCliente(Banco *b1,int id_cli){ 
  for(int i=0;i<b1->num_cuentas;i++){
    if(b1->cuentas[i].id_cliente==id_cli)
      return &b1->cuentas[i];
  }
  return NULL;
}

Cuenta *buscaCuentaDestino(Banco *b1,int cuenta_dest){ 
  for(int i=0;i<b1->num_cuentas;i++){
    if(b1->cuentas[i].id_cuenta==cuenta_dest)
      return &b1->cuentas[i];
  }
  return NULL;
}

int procesaOperacion(Banco *b1,Mensaje *msg){
  Cuenta *c1=buscaCuentaCliente(b1,msg->id_cliente);

  switch(msg->op.tipo){
  case INGRESAR:
    c1->saldo+=msg->op.cantidad;
    printf("\nINGRESO:\nEl cliente '%s' ha realizado un ingreso de %.2f€ en su cuenta %s, saldo actualizado: %.2f€\n",c1->nombre,msg->op.cantidad,c1->num_cuenta,c1->saldo);
    break;
  case RETIRAR:
    c1->saldo-=msg->op.cantidad; //No comprueba que saldo sea positivo ya que lo hace cliente
    break;
  case TRANSFERIR:
    Cuenta *dest=buscaCuentaDestino(b1,msg->op.cuenta_dest);
    if(dest){
      c1->saldo-=msg->op.cantidad;
      dest->saldo+=msg->op.cantidad;
    }
    break;
  case MOVIMIENTOS:
    // Llama a muestraMovimientos();
    break;
  case SALIR:
    printf("\nSALIDA CLIENTE:\nCerrando hilo del cliente '%s' con Número de cuenta: %s\n",c1->nombre,c1->num_cuenta);
    return 1;
    break;
  }
  
  return 0;
}





