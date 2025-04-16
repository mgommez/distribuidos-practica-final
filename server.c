// Archivo para el manejo de la recepción de mensajes de clientes
// y llamadas a las funciones de "claves.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
#include "comm.h"
#include "claves.h"
#include "message.h"

// Se elimina busy, pthread_cond y su mutex ya que no son necesarios
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// Declaración variable de puerto
int assigned_port = 0;
char *port;

void *tratar_peticion(void *sd_client_void) {
    // Extraemos el descriptor de socket y liberamos la memoria reservada
    int sd_client = *(int *)sd_client_void;
    free(sd_client_void);

    int ret;
    int op, r;
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
        perror("Error al leer la petición del cliente.");
        free(p);
        closeSocket(sd_client);
        pthread_exit((void *) -2);
    }

    op = strtol(buffer, &endptr, 10);
    if (endptr[0] != '\0') {
        printf("Error: %s no es un número en base %d\n", buffer, 10);
        free(p);
        closeSocket(sd_client);
        pthread_exit((void *) -1);
    }
    memset(buffer, '\0', sizeof(buffer));

    // Llamada a la operación según el código recibido
    switch (op) {
        case 0: // destroy()
            r = destroy();
            sprintf(buffer, "%d", r);
            ret = writeLine(sd_client, buffer);
            if (ret < 0) {
                perror("Error en writeLine del servidor case 0");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            break;

        case 1: // set_value()
            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de key
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: key");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->key = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            ret = readLine(sd_client, p->value1, sizeof(buffer));
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: value1");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }

            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de N_value2
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: N_value2");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->N_value2 = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            for (int i = 0; i < p->N_value2; i++) {
                ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de V_value2
                if (ret < 0) {
                    perror("Error al leer parámetro del cliente: V_value2");
                    free(p);
                    closeSocket(sd_client);
                    pthread_exit((void *) -2);
                }
                p->V_value2[i] = strtod(buffer, NULL);
                memset(buffer, '\0', sizeof(buffer));
            }

            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de value3.x
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: value3.x");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->value3.x = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }

            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de value3.y
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: value3.y");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->value3.y = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            r = set_value(p->key, p->value1, p->N_value2, p->V_value2, p->value3);
            sprintf(buffer, "%d", r);
            ret = writeLine(sd_client, buffer);
            if (ret < 0) {
                perror("Error en writeLine del servidor case 1");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            break;

        case 2: // get_value()
            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de key
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: key");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->key = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            r = get_value(p->key, p->value1, &(p->N_value2), p->V_value2, &(p->value3));
            sprintf(buffer, "%d", r);
            ret = writeLine(sd_client, buffer);
            if (ret < 0) {
                perror("Error en writeLine del servidor case 2 - r");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            memset(buffer, '\0', sizeof(buffer));

            if (r == 0) {
                // Envío de value1
                ret = writeLine(sd_client, p->value1);
                if (ret < 0) {
                    perror("Error en writeLine del servidor case 2 - value1");
                    free(p);
                    closeSocket(sd_client);
                    pthread_exit((void *) -2);
                }
                // Envío de N_value2
                sprintf(buffer, "%d", p->N_value2);
                ret = writeLine(sd_client, buffer);
                if (ret < 0) {
                    perror("Error al escribir información relativa a get_value()");
                    free(p);
                    closeSocket(sd_client);
                    pthread_exit((void *) -2);
                }
                memset(buffer, '\0', sizeof(buffer));
                // Envío de V_value2
                for (int i = 0; i < p->N_value2; i++) {
                    sprintf(buffer, "%.1f", p->V_value2[i]);
                    ret = writeLine(sd_client, buffer);
                    if (ret < 0) {
                        perror("Error al escribir información relativa a get_value()");
                        free(p);
                        closeSocket(sd_client);
                        pthread_exit((void *) -2);
                    }
                    memset(buffer, '\0', sizeof(buffer));
                }
                // Envío de value3.x y value3.y
                sprintf(buffer, "%d", p->value3.x);
                ret = writeLine(sd_client, buffer);
                if (ret < 0) {
                    perror("Error al escribir información relativa a get_value()");
                    free(p);
                    closeSocket(sd_client);
                    pthread_exit((void *) -2);
                }
                memset(buffer, '\0', sizeof(buffer));
                sprintf(buffer, "%d", p->value3.y);
                ret = writeLine(sd_client, buffer);
                if (ret < 0) {
                    perror("Error al escribir información relativa a get_value()");
                    free(p);
                    closeSocket(sd_client);
                    pthread_exit((void *) -2);
                }
                memset(buffer, '\0', sizeof(buffer));
            }
            break;

        case 3: // modify_value()
            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de key
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: key");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->key = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            ret = readLine(sd_client, p->value1, sizeof(buffer));
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: value1");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }

            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de N_value2
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: N_value2");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->N_value2 = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            for (int i = 0; i < p->N_value2; i++) {
                ret = readLine(sd_client, buffer, sizeof(buffer));
                if (ret < 0) {
                    perror("Error al leer parámetro del cliente: V_value2");
                    free(p);
                    closeSocket(sd_client);
                    pthread_exit((void *) -2);
                }
                p->V_value2[i] = strtod(buffer, NULL);
                memset(buffer, '\0', sizeof(buffer));
            }

            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de value3.x
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: value3.x");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->value3.x = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de value3.y
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: value3.y");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->value3.y = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            r = modify_value(p->key, p->value1, p->N_value2, p->V_value2, p->value3);
            sprintf(buffer, "%d", r);
            ret = writeLine(sd_client, buffer);
            if (ret < 0) {
                perror("Error en writeLine del servidor case 3");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            break;

        case 4: // delete_key()
            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de key
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: key");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->key = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            r = delete_key(p->key);
            sprintf(buffer, "%d", r);
            ret = writeLine(sd_client, buffer);
            if (ret < 0) {
                perror("Error en writeLine del servidor case 4");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            break;

        case 5: // exist()
            ret = readLine(sd_client, buffer, sizeof(buffer)); // Lectura de key
            if (ret < 0) {
                perror("Error al leer parámetro del cliente: key");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            p->key = strtol(buffer, &endptr, 10);
            if (endptr[0] != '\0') {
                printf("Error: %s no es un número en base %d\n", buffer, 10);
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -1);
            }
            memset(buffer, '\0', sizeof(buffer));

            r = exist(p->key);
            sprintf(buffer, "%d", r);
            ret = writeLine(sd_client, buffer);
            if (ret < 0) {
                perror("Error en writeLine del servidor case 5");
                free(p);
                closeSocket(sd_client);
                pthread_exit((void *) -2);
            }
            break;

        default:
            printf("Operación desconocida: %d\n", op);
            break;
    }

    free(p);
    closeSocket(sd_client);  // El hilo cierra el socket al final
    pthread_exit(NULL);
}

int do_exit = 0;

void sigHandler() {
    do_exit = 1;
}

int main(int argc, char *argv[]) {
    printf("Servidor iniciado...\n");

    int sd_server, sd_client;
    char *endptr;

    if (argc != 2) {
        printf("Puerto no especificado o paso de demasiados argumentos. Exit.\n");
        return -1;
    }

    int port_num = (int)strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || port_num <= 0 || port_num > 65535) {
        perror("Error: puerto inválido.");
        return -1;
    }

    sd_server = serverSocket(INADDR_ANY, port_num, SOCK_STREAM);
    if (sd_server < 0) {
        perror("Error al abrir serverSocket");
        return -2;
    }

    pthread_attr_t t_attr;
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    struct sigaction new_action, old_action;
    new_action.sa_handler = sigHandler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGINT, &new_action, NULL);
    }

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
