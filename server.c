// Archivo para el manejo de la recepción de mensajes de clientes
// y llamadas a las funciones de "claves.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
#include "comm.h"
#include "users.h"
#include "message.h"

// Se elimina busy, pthread_cond y su mutex ya que no son necesarios
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// Declaración variable de puerto
int assigned_port = 0;
char *port;

void *tratar_peticion(void *sd_client_void) {

    // 1. Extración del descriptor de socket y liberación de memoria dinámica
    int sd_client = *(int *)sd_client_void;
    free(sd_client_void);

    int ret;
    int r;
    char op[20];
    char buffer[350];
    char *endptr;

    struct peticion *p = (struct peticion *) malloc(sizeof(struct peticion));
    if (NULL == p) {
        perror("Error al asignar memoria para la petición");
        closeSocket(sd_client);
        pthread_exit((void *) -1);
    }

    // Lectura del código de operación
    if (readLine(sd_client, buffer, sizeof(buffer)) < 0) {
        perror("Error al leer la petición del cliente: código de operación");
        free(p);
        closeSocket(sd_client);
        pthread_exit((void *) -2);
    }

    op = strcpy(op, buffer);
    memset(buffer, '\0', sizeof(buffer));

    // Llamada a la operación según el código recibido
    switch (op) {
        case "REGISTER":
          break;
        case "UNREGISTER":
          break;
        case "CONNECT":
          break;
        case "PUBLISH":
          break;
        case "DELETE":
          break;
        case "LIST_USERS";
          break;
        case "LIST_CONTENT":
          break;
        case "DISCONNECT":
          break;
    default:
        printf("Operación desconocida: %d\n", op);
        break;


    }


    //Final de ejecución
    printf("OPERATION %s FROM %s\n", op, p->username);

    free(p);
    closeSocket(sd_client);  // El hilo cierra el socket al final
    pthread_exit(NULL);

}

int do_exit = 0;

void sigHandler() {
    do_exit = 1;
}

int main(int argc, char *argv[]) {

    //1. Inicialización del servidor: puerto e ip

    //Argumento: puerto
    int sd_server, sd_client;
    char *endptr;

    if (argc != 3) {
        printf("Puerto no especificado o paso de demasiados argumentos. Exit.\nIniciar con ./server -p <puerto>.\n");
        return -1;
    }

    int port_num = (int)strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || port_num <= 0 || port_num > 65535) {
        perror("Error: puerto inválido.");
        return -1;
    }


    //Obtención de IP (host)
    char hostname[256];
    char *ip_address;

    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("Error al obtener el nombre del host");
        return 1;
    }

    struct hostent *host_entry;
    host_entry = gethostbyname(hostname);
    if (host_entry == NULL) {
        perror("Error al obtener información del host");
        return 1;
    }

    // Convertir la dirección IP al formato string
    ip_address = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    printf("init server %s: %d \n", ip_address, port_num);


    //2. Configuración del servidor

    //inicialización del socket
    sd_server = serverSocket(INADDR_ANY, port_num, SOCK_STREAM);
    if (sd_server < 0) {
        perror("Error al abrir serverSocket");
        return -2;
    }

    //interupción de ejecución
    struct sigaction new_action, old_action;
    new_action.sa_handler = sigHandler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGINT, &new_action, NULL);
    }

    //para la gestión de hilos
    pthread_attr_t t_attr;
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);


    //3. Bucle de peticiones
    while (do_exit == 0) {
        sd_client = serverAccept(sd_server);
        if (sd_client < 0) {
            perror("Error en serverAccept");
            break;
        }

        // Reserva de memoria para el descriptor y lo pasamos al hilo
        int *p_sd = malloc(sizeof(int)); //TODO: este malloc es necesario??
        if (p_sd == NULL) {
            perror("Error en malloc para descriptor");
            closeSocket(sd_client);
            continue;
        }
        *p_sd = sd_client;

        //crear e inicializar el hilo
        pthread_t id_thread;
        if (pthread_create(&id_thread, &t_attr, tratar_peticion, (void *)p_sd) != 0) {
            perror("Error en pthread_create");
            free(p_sd);
            closeSocket(sd_client);
        }
    }

    closeSocket(sd_server);
    printf("The End.\n");


    return 0;
}
