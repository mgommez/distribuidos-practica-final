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


//funcionalidad principal

void *tratar_peticion(void *sd_client_void) {

    // 1. Extración del descriptor de socket y liberación de memoria dinámica
    int sd_client = *(int *)sd_client_void;
    free(sd_client_void);

    int ret;
    int r;
    char op[20];
    char buffer[257]; //TODO: revisar tamaño del buffer
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

    // Comprobación de fin de cadena '\0'
    if (strnlen(buffer, sizeof(buffer)) == sizeof(buffer)) {
        perror("Petición mal formada: código de operación sin '\0'");
        free(p);
        closeSocket(sd_client);
        pthread_exit((void *) -2);
    }

    strcpy(op, buffer);
    memset(buffer, '\0', sizeof(buffer));

    // Llamada a la operación según el código recibido
    if(strcmp(op, "REGISTER") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        //2. Llamada a la función
        r = register_user(p->username);

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer);
        if(ret<0){
            perror("Error en writeLine del servidor REGISTER");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        memset(buffer, '\0', sizeof(buffer));

    } //end REGISTER

    else if( strcmp(op, "UNREGISTER") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        //2. Llamada a la función
        r = unregister_user(p->username);

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer);
        if(ret<0){
            perror("Error en writeLine del servidor UNREGISTER");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        memset(buffer, '\0', sizeof(buffer));

    } //end UNREGISTER

    else if( strcmp(op, "CONNECT") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        // obtención del puerto de escucha del cliente
        ret = readLine(sd_client, buffer, sizeof(buffer)); //lectura puerto
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: port");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        p->port = strtol(buffer, &endptr, 10);
        if (endptr[0] != '\0') {
            printf("Error: %s no es un número en base %d\n", buffer, 10);
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -1);
        }
        memset(buffer, '\0', sizeof(buffer));

        //obtención de la ip del cliente
        ret = getClientIp(sd_client, p->host);
        if (ret < 0) {
            perror("Error al obtener la ip del cliente");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -1);
        }

        //2. Llamada a la función
        r = connect_user(p->username, p->host, p->port);

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer);
        if(ret<0){
            perror("Error en writeLine del servidor CONNECT");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        memset(buffer, '\0', sizeof(buffer));

    } //end CONNECT

    else if( strcmp(op, "PUBLISH") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        ret = readLine(sd_client, p->filename, sizeof(p->filename)); //lectura filename (path)
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: filename");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        ret = readLine(sd_client, p->description, sizeof(p->description)); //lectura descripción
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: description");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        //2. Llamada a la función
        r = publish_file(p->username, p->filename, p->description);

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer);
        if(ret<0){
            perror("Error en writeLine del servidor PUBLISH");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        memset(buffer, '\0', sizeof(buffer));

    } //end PUBLISH

    else if( strcmp(op, "DELETE") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        ret = readLine(sd_client, p->filename, sizeof(p->filename)); //lectura filename (path)
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: filename");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        //2. Llamada a la función
        r = delete_file(p->username, p->filename);

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer);
        if(ret<0){
            perror("Error en writeLine del servidor DELETE");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        memset(buffer, '\0', sizeof(buffer));
    } //end DELETE

    else if( strcmp(op, "LIST_USERS") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        //2. Llamada a la función
        struct data *d = (struct data *) malloc(sizeof(struct data)); //TODO: revisar frees
        if (NULL == d) {
            perror("Error al asignar memoria para los datos");
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        r = list_users(p->username, &(d->counter), &(d->user_list)); //se recibe una lista de estructuras tipo user, ip, puerto, next con los usuarios conectados

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer); //escritura byte de resultado
        if(ret<0){
            perror("Error en writeLine del servidor LIST_USERS");
            free(p);
            free(d);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        memset(buffer, '\0', sizeof(buffer));

        if(r == 0) {
        //éxito de la operación: se envía la lista de usuarios
              sprintf(buffer, "%d", d->counter);
              ret = writeLine(sd_client, buffer); //escritura número de usuarios
              if(ret<0){
                  perror("Error en writeLine del servidor LIST_USERS");
                  free(p);
                  free(d);
                  closeSocket(sd_client);
                  pthread_exit((void *) -2);
              }
              memset(buffer, '\0', sizeof(buffer));
              struct user_data_item *current_user = d->user_list;
              struct user_data_item *prev_user;
              for(int i=0; i<d->counter; i++) {
                  //escritura de los datos de cada usuario conectado

                  ret = writeLine(sd_client, current_user->username); //escritura username
                  if(ret<0){
                      perror("Error en writeLine del servidor LIST_USERS");
                      free(p);
                      while (current_user != NULL) {
                          prev_user = current_user;
                          current_user = current_user->next;
                          free(prev_user);
                      }
                      free(current_user);
                      free(d);
                      closeSocket(sd_client);
                      pthread_exit((void *) -2);
                  }

                  ret = writeLine(sd_client, current_user->host); //escritura ip
                  if(ret<0){
                      perror("Error en writeLine del servidor LIST_USERS");
                      free(p);
                      free(d);
                      free(current_user);
                      closeSocket(sd_client);
                      pthread_exit((void *) -2);
                  }

                  sprintf(buffer, "%d", current_user->port);
                  ret = writeLine(sd_client, buffer); //escritura puerto
                  if(ret<0){
                      perror("Error en writeLine del servidor LIST_USERS");
                      free(p);
                      free(d);
                      free(current_user);
                      closeSocket(sd_client);
                      pthread_exit((void *) -2);
                  }
                  memset(buffer, '\0', sizeof(buffer));

                  //liberar memoria y avanzar en la lista
                  prev_user = current_user;
                  current_user = current_user -> next;
                  free(prev_user);

              } //end for
              free(d);

        } //end if

    } //end LIST_USERS

    else if( strcmp(op, "LIST_CONTENT") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username emisor
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }

        ret = readLine(sd_client, p->user_wanted, sizeof(p->user_wanted)); //lectura username solicitado
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: user_wanted");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        //2. Llamada a la función

        //Inicializar estrcuturas auxiliares
        struct data *d = (struct data *) malloc(sizeof(struct data)); //TODO: revisar frees
        d->file_list = NULL;

        //Llamada a la función
        r = list_content(p->username, p->user_wanted, &(d->counter), &(d->file_list)); //se recibe una lista de estructuras tipo filename, next con los ficheros del usuario
        //TODO: update users.c (qué es esto?)

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer); //escritura byte de resultado
        if(ret<0){
            perror("Error en writeLine del servidor LIST_CONTENT");
            free(p);
            free(d);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        memset(buffer, '\0', sizeof(buffer));

        if(r == 0) {
            //éxito de la operación: se envía la lista de ficheros
            sprintf(buffer, "%d", d->counter);
            ret = writeLine(sd_client, buffer); //escritura número de archivos
            if(ret<0){
                perror("Error en writeLine del servidor LIST_CONTENT");
                free(p);
                free(d);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            memset(buffer, '\0', sizeof(buffer));

            struct file *temp = d->file_list;

            for(int i = 0; i < d->counter; i++) {
                ret = writeLine(sd_client, temp->filename);
                if(ret < 0){
                    perror("Error en writeLine del servidor LIST_CONTENT");
                    free(p);
                    free(d);
                    closeSocket(sd_client);
                    pthread_exit((void *) -2);
                }
                temp = temp->next;

            }//end for

            free(d);
        }//end if

    }//end LIST_CONTENT

    else if( strcmp(op, "DISCONNECT") == 0){
        //1. Lectura de parámetros por socket
        ret = readLine(sd_client, p->username, sizeof(p->username)); //lectura username
        if (ret < 0) {
            perror("Error al leer parámetro del cliente: username");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
        //2. Llamada a la función
        r = disconnect_user(p->username);

        //3. Escritura del resultado por socket
        sprintf(buffer, "%d", r);
        ret = writeLine(sd_client, buffer);
        if(ret<0){
            perror("Error en writeLine del servidor DISCONNECT");
            free(p);
            closeSocket(sd_client);
            pthread_exit((void *) -2);
        }
    }//end DISCONNECT

    else{
      printf("Operación desconocida: %s\n", op);
    }

    //Final de ejecución
    printf("s> OPERATION %s FROM %s\n", op, p->username);

    free(p);
    closeSocket(sd_client);  // El hilo cierra el socket al final
    pthread_exit(NULL);

}//end tratar_peticion

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

    printf("s> init server %s: %d \n", ip_address, port_num);


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
        int *p_sd = malloc(sizeof(int));
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
