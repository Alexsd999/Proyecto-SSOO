#include "../include/cab.h"
#include "../include/funciones_banco.h"

typedef struct{
  Banco *banco;
  int id_cli;
} DATOS;

void *hilo_cliente(void *arg){
  DATOS *datos=(DATOS *)arg;
  Banco *b=datos->banco;
  Mensaje msg;
  Mensaje resp;
  int id_cli=datos->id_cli;
  key_t clave_cola=ftok(RUTA_CLAVE,COLA);
  int msgqueue_id;
  
  msgqueue_id=msgget(clave_cola,0660);
  while(1){
    if(leer_msg(msgqueue_id,id_cli,&msg)!=-1){
      
      int salir=procesaOperacion(b,&msg);

      if(salir==1)
	pthread_exit(NULL);
      
      resp.mtype=msg.id_cliente;
      resp.id_cliente=msg.id_cliente;
      resp.op.tipo=msg.op.tipo;
      resp.op.cantidad=buscaCuentaCliente(b,msg.id_cliente)->saldo;
  
      if(escr_msg(msgqueue_id,&resp)==-1){
	printf("[ERROR] No se pudo enviar su solicitud al cliente.\n");
	exit(1);
      }
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  Banco b1;
  DATOS datos[MAX_CLI];
  pthread_t thid[MAX_CLI];
  Cliente listaClientes[MAX_CLI];
  int num_cli;

  b1.num_cuentas=0;
  
  if(argc!=1){
    printf("[ERROR] Uso: %s\n",argv[0]);
    exit(1);
  }
  manejadorSIGINT();
  creaRecursos();
  printf("\n\n\n=====================================================\nHa accedido al sistema de ETSIBank\n");
  printf("\nLos clientes que pertenecen a este banco son:\n");
  // Muestra los clientes del banco
  num_cli=inicializarCuentas(&b1,listaClientes);

  // Crea hilos para todos los clientes asignando uno por cliente
  for(int i=0;i<num_cli;i++){
    datos[i].banco=&b1;
    datos[i].id_cli=listaClientes[i].id_cli;
    pthread_create(&thid[i],NULL,hilo_cliente,&datos[i]);
    pthread_detach(thid[i]);
  }
  while(1)
    pause();
  return 0;
}
