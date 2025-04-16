#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users.h"


struct user *USERS = NULL;

int check_value(char *value) {
    // Comprobación de longitud de un valor (no más de 255 caracteres)
    // Válido para filename, description y username
    int i=0;
    int end=0;

    while (i<255 && end==0){
        if(value[i] == '\0'){
            end=1;
        }
        i += 1;
    }

    if(end != 1){
        printf("Error de %s: \n\tmayor de 255 caracteres\n", value);
        return -1;
    }

    return 0;
}

int register_user(char *username) {
    if (check_value(username) < 0) {
        return 2;
    }

    int empty = 0;
    int find = 0;

    // caso de lista enlazada vacía
    if (NULL == USERS) {
        empty = 1;
        find = 1;
    }

    struct user *temp = USERS;

    // Bucle para recorrer la lista enlazada
    while (find == 0) {
        // Doble comprobación de que la lista enlazada esté vacía (¿necesaria?)
        if (temp == NULL) {
            find = 1;
            break;
        }

        // caso de usuario ya registrado
        if (temp->username == username) {
            printf("El usuario %s ya está registrado.\n", username);
            return 1;
        }

        if (temp->next == NULL) {
            find = 1;
        } else {
            temp = temp->next;
        }
    }

    struct user *new_user = (struct user*)malloc(sizeof(struct user));
    if (new_user == NULL) {
        printf("Error al reservar memoria dinámica.");
        return 2;
    }
    strcpy(new_user->username, username);

    if (empty) {
        TUPLE = new_user;
    } else {
        // insertar el nuevo elemento al inicio de la lista enlazada
        temp->next = new_user;
    }
    new_user->next = NULL;

    return 0;
};

int connect_user(char *username, char *host, int port) {
    // Comprobación de la corrección de los argumentos
    if (check_value(username) < 0) {
        return 3;
    }
    // TODO: check host y check port?

    // Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Se guarda el puerto donde está escuchando peticiones el usuario
    if (temp->port != NULL) {
        printf("El usuario %s ya está conectado.\n", username);
        return 2;
    } else {
        //strcpy(temp->host, host);
        temp->port = port;
    };

    return 0;
};

int publish_file(char *username, char *filename, char *description) {
    // Se busca el username del cliente que está haciendo la solicitud y se almacena el nuevo archivo

    // Comprobación de la corrección de los argumentos
    if (check_value(username) < 0 || check_value(filename) < 0 || check_value(description) < 0) {
        return 4;
    }

    // Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Comprobación de requisito de conexión del usuario
    if (temp->port == NULL) {   // TODO: cómo definir que está o no conectado? NULL u otro valor numérico?
        printf("El usuario %s no está conectado al servicio", username);
        return 2;
    }

    // Se hace una búsqueda de los archivos (si existen), comprobando que el archivo a guardar no está ya guardado
    // Si es así, se busca el final de la lista enlazada de archivos del usuario y se almacena el nuevo archivo

    struct file *new_file = (struct file*)malloc(sizeof(struct file));
    if (new_file == NULL) {
        printf("Error al reservar memoria dinámica.");
        return 4;
    }

    struct file *temp_file = temp->files;

    if (temp_file == NULL) {    // Caso de lista de archivos vacía
        temp_file = new_file;
    } else {
        find = 0;

        while (find == 0) {
            if (temp_file->filename == filename) {
                printf("El archivo %s ya está publicado en la lista del usuario %s.\n", filename, username);
                free(new_file);
                return 3;
            } else if (temp_file->next == NULL) {   // Caso de hallar el final de la lista de archivos
                find = 1;
            } else {
                temp_file = temp_file->next;
            }
        }
    }

    // Se asignan los valores indicados por el usuario a la nueva estructura
    strcpy(new_file->filename, filename);
    strcpy(new_file->description, description);
    new_file->next = NULL; //TODO: ¿?

    return 0;
};

/*
int get_user(char *username, char *host, int *port) { // añadir puntero a struct file podría aligerar el código
    // ¿¿Meter en .h??
    // Búsqueda de un elemento de la lista enlazada dado su identificador (el username)
    if (check_value(username) < 0) {
        return -1;
    }

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return -1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return -1;
        } else {
            temp = temp->next;
        }
    }

    strcpy(host, temp->host);
    *port = temp->port;

    return 0;
};
 */

int delete_file(char *username, char *filename) {
    // Comprobación de la corrección de los argumentos
    if (check_value(username) < 0 || check_value(filename) < 0 || check_value(description) < 0) {
        return 4;
    }

    // Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Comprobación de requisito de conexión del usuario
    if (temp->port == NULL) {   // TODO: cómo definir que está o no conectado? NULL u otro valor numérico?
        printf("El usuario %s no está conectado al servicio", username);
        return 2;
    }

    // Búsqueda de fichero
    struct file *temp_file = temp->files;
    struct file *prev_file = NULL;

    if (temp_file == NULL) {    // Caso de lista de archivos vacía
        printf("Lista vacía: el archivo %s no está publicado en la lista del usuario %s.\n", filename, username);
        return 3;
    }

    find = 0;

    while (find == 0) {
        if (temp_file->filename == filename) {
            find = 1;
        } else if (temp_file->next == NULL) {   // Caso de hallar el final de la lista de archivos
            printf("El archivo %s no está publicado en la lista del usuario %s.\n", filename, username);
            return 3;
        } else {
            prev_file = temp_file;
            temp_file = temp_file->next;
        }
    }

    prev_file->next = temp_file->next;
    free(temp_file);

    return 0;
};

int list_users(char *username, int *counter) {  // TODO: Hay que enviar el número de usuarios guardados para que el cliente sepa cuántos va a recibir
                                                // La pregunta es cómo se va a pasar toda la información leída ¿leerlos de uno en uno desde el servidor?
    // Comprobación de la corrección de los argumentos
    if (check_value(username) < 0 || check_value(filename) < 0 || check_value(description) < 0) {
        return 3;
    }

    // Se recorre toda la lista enlazada de usuarios.
    // TODO: de momento se imprimen, pero ¿se envían, se devuelven, se guardan...?

    // Se comprueba que el usuario que realiza la petición está registrado y conectado (username)

    // Búsqueda del usuario
    // Caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Comprobación de requisito de conexión del usuario
    if (temp->port == NULL) {   // TODO: cómo definir que está o no conectado? NULL u otro valor numérico?
        printf("El usuario %s no está conectado al servicio", username);
        return 2;
    }

    // Se reinicia el contador y la variable auxiliar para recorrer la lista enlazada de usuarios entera
    counter = 0;
    temp = USERS;

    while (temp != NULL) {
        printf("User: %s\tHost: %s\tPort: %d\n", temp->username, temp->host, temp->port);
        temp = temp->next;
        counter++;
    }

    return 0;
};

int list_content(char *username, char *searched_username, int * counter) {     // username es el que realiza la petición
                                                                // searched_username el usuario del que quiere conocer
                                                                // los archivos publicados
                                                                // TODO: mismo problema de list_users() con el envío de la info + también hay que devolver un contador como en list_users()
    // Comprobación de la corrección de los argumentos
    if (check_value(username) < 0 || check_value(searched_username) < 0) {
        return 4;
    }

    // Se comprueba que el usuario que realiza la petición está registrado y conectado (username)
    // Búsqueda del usuario
    // Caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Comprobación de requisito de conexión del usuario
    if (temp->port == NULL) {   // TODO: cómo definir que está o no conectado? NULL u otro valor numérico?
        printf("El usuario %s no está conectado al servicio", username);
        return 2;
    }

    // Búsqueda del usuario cuyo contenido se quiere conocer
    // Se resetea la búsqueda
    find = 0;
    temp = USERS;

    while (find == 0) {
        if (temp->username == searched_username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s cuyo contenido se quiere conocer no está en la lista.\n", searched_username);
            return 3;
        } else {
            temp = temp->next;
        }
    }

    // Una vez hallado el usuario se buscan sus archivos publicados
    counter = 0;
    // Caso de usuario sin archivos publicados
    if (!temp->files) {
        printf("El usuario %s no ha publicado ningún archivo.\n", searched_username);
    } else {
        printf("User: %s\n", temp->username);

        // Búsqueda y devolución de los archivos del usuario
        struct file *temp_file = temp->files;

        while (temp_file != NULL) {
            printf("File: %s\tDescription: %s\n", temp_file->filename, temp_file->description);
            temp_file = temp_file->next;
            counter ++;
        }
    }

    return 0;
};

int disconnect_user(char *username) {
    // Al desconectarse un usuario, deja de escuchar en el puerto almacenado

    // Comprobación de la corrección de los argumentos
    if (check_value(username) < 0) {
        return 3;
    }

    // Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Comprobación de conexión previa
    if (temp->port == NULL) {   // TODO: arreglar ¿?
        printf("El usuario %s no estaba conectado.\n", username);
        return 2;
    }

    //temp->host = NULL; // strcpy() en cualquier caso
    temp->port = NULL; // TODO: poner un número de puerto imposible para saber que no está conectado ¿?

    return 0;
};

int unregister_user(char *username) {
    // Se elimina el usuario de la lista

    // Comprobación del valor de usuario
    if (check_value(username) < 0) {
        return 2;
    }

    // Comprobación de corrección del nombre de usuario
    // Caso de lista vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;
    struct user *prev = NULL;

    while (find == 0) {
        if (temp->username == username) {
            find=1;
        } else if (temp->next == NULL) {
            // Caso de usuario no registrado
            printf("El usuario %s no está registrado.\n", username);
            return 1;
        } else {
            prev = temp;
            temp = temp->next;
        }
    }

    // Caso de que se borre el primer elemento de la lista enlazada
    if (prev == NULL) {
        USERS = temp->next;
    } else {
        prev->next = temp->next;
    }

    // Liberar memoria dinámica
    // Liberar la memoria dinámica de los archivos en caso de haberlos
    if (temp->files) {
        struct file *current_file = temp->files;
        while (current_file != NULL) {
            struct file *next_file = current_file->next;
            free(current_file);
            current_file = next_file;
        }
    }
    free(temp);

    return 0;
};
