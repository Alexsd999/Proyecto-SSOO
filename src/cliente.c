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

  key_t clave=ftok(RUTA_CLAVE,PROYECTO_CLAVE);
  int msgqueue_id=msgget(clave,IPC_CREAT|0600);
  
  if(msgqueue_id==-1){
    printf("[ERROR] No se pudo iniciar la cola.\n");
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
      // Añadir, si puede ser, que se haga transferencia entre bancos
      op.tipo=TRANSFERIR;
      op.cuenta_orig=id_cuenta;
      printf("Indique el número de cuenta destino: ");
      scanf("%31s",op.cuenta_dest);
      printf("¿Qué cantidad desea transferir?");
      op.cantidad=muestraImportes();
      break;
    case SALIR:
      printf("Saliendo del sistema, gracias por confiar en nosotros.\n");
      break;
    default:
      printf("\n[ERROR] Operación inválida, inténtelo de nuevo.\n");
      opcion=menuOpciones();
      break;
    }
  }while(opcion!=4); // CAMBIAR SI SE AÑADEN MAS OPCIONES!!
  msg.mtype=1;
  msg.id_cliente=c1.id_cli; // Para que el banco identifique al cliente
  msg.op=op;

  if(msgsnd(msgqueue_id,&msg,MSG_SIZE,0)==-1)
    perror("[ERROR] Error al enviar mensaje al banco.\n");
  else
    printf("Operación enviada al banco.\n");
  
  return 0;
}
