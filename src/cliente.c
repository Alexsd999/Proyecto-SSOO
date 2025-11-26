#include "../include/cab.h"
#include "../include/funciones.h"

int main(int argc, char *argv[]){
  Cliente c1;
  Operacion op;
  Mensaje msg;
  int existe=0;
  int opcion;
  int id_cuenta;
  
  if(argc!=1){
    printf("[ERROR] Uso: %s\n",argv[0]);
    exit(1);
  }
  
  login(c1.nombre,&c1.pin);
  existe=verificaCliente(&c1);
  
  if(!existe){
    printf("[ERROR] Las credenciales no son correctas o usuario no existe.\n");
    exit(1);
  }

  printf("Bienvenido %s, por favor indique la cuenta a la que desea acceder:\n",c1.nombre);
  id_cuenta=cuentasCliente(&c1.id_cli);

  key_t clave=ftok(RUTA_CLAVE,COLA);
  int msgqueue_id=msgget(clave,0660);
  long tipo=c1.id_cli;
  
  if(msgqueue_id==-1){
    printf("[ERROR] No se pudo acceder a la cola.\n");
    exit(1);
  }
  
  do{
    opcion=menuOpciones();
    switch(opcion){
    case INGRESAR:
      op.tipo=INGRESAR;
      op.cuenta_orig=id_cuenta;
      printf("¿Qué cantidad desea ingresar?");
      op.cantidad=muestraImportes();
      break;
    case RETIRAR:
      op.tipo=RETIRAR;
      op.cuenta_orig=id_cuenta;
      printf("¿Qué cantidad desea retirar?");
      op.cantidad=muestraImportes();
      break;
    case TRANSFERIR:
      op.tipo=TRANSFERIR;
      op.cuenta_orig=id_cuenta;
      printf("Indique el número de cuenta destino: ");
      scanf("%d",&op.cuenta_dest);// Hacer conversion a int id cliente cuentadestino
      printf("¿Qué cantidad desea transferir?");
      op.cantidad=muestraImportes();
      break;
    case MOVIMIENTOS:
      op.tipo=MOVIMIENTOS;
      op.cuenta_orig=id_cuenta;
      break;
    case SALIR:
      op.tipo=SALIR;
      printf("Saliendo del sistema, gracias por confiar en nosotros.\n");
      break;
    default:
      printf("\n[ERROR] Operación inválida, inténtelo de nuevo.\n");
      opcion=menuOpciones();
      break;
    }
    msg.mtype=tipo;
    msg.id_cliente=c1.id_cli; // Para que el banco identifique al cliente
    msg.op=op;

    if(escr_msg(msgqueue_id,&msg)==-1){
      printf("[ERROR] No se pudo enviar su solicitud al banco, inténtelo de nuevo.\n");
      exit(1);
    }else if(opcion!=SALIR)
      printf("Enviando solicitud al banco, por favor espere a ser atendido.\n");
    if(opcion!=SALIR){
      if(leer_msg(msgqueue_id,tipo,&msg)==-1){
	printf("Error al recibir el mensaje.\n");
	exit(1);
      }
      if(opcion!=MOVIMIENTOS)
	printf("\nSe ha realizado la operacion con éxito, su nuevo saldo es de %.2f€\n",msg.op.cantidad);
    } 
  }while(opcion!=SALIR); // CAMBIAR SI SE AÑADEN MAS OPCIONES!!, (en el cab.h)
  
  return 0;
}
