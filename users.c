#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users.h"


struct user *USERS = NULL;

//funciones de validación de argumentos
int check_size256(char *value) {
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

int check_blanks(char *value){
  for (int i = 0; value[i] != '\0'; i++) {
    if (value[i] == ' ') {
      printf("Error de %s: \n\tcontiene espacios en blanco en la posición %d\n", value, i);
      return -1;
    }
  }
  return 0;
}

//funcionalidades servicio de tuplas

int register_user(char *username) {
    //1. Comprobaciones: nombre de usuario válido
    if (check_size256(username) < 0) {
      return 2;
    }

    int empty = 0;
    int find = 0;

    //2. Búsqueda
    // caso de lista enlazada vacía
    if (NULL == USERS) {
      empty = 1;
      find = 1;
    }

    struct user *temp = USERS;

    // Bucle para recorrer la lista enlazada
    while (find == 0) {

        //si el usuario existe retornamos
        if(strcmp(username, temp-> username)){
          printf("El usuario %s ya está registrado.\n", username);
          return 1;
        }

        //final de lista: clave no encontrada, podemos registrar
        if (temp->next == NULL) {
          find = 1;
          break;
        }
        temp = temp->next;
    }

    //3. Insercción
    //reserva de memoria dinámica
    struct user *new_user = (struct user*)malloc(sizeof(struct user));

    if (new_user == NULL) {
        printf("Error al reservar memoria dinámica.");
        return 2;
    }

    //copia de parámetros
    strcpy(new_user->username, username);
    new_user->N_files = 0;

    if (empty) {
        USERS = new_user;
    } else {
        // insertar el nuevo elemento al final de la lista enlazada
        temp->next = new_user;
    }
    //new_user->next = NULL; //TODO: necesario? creo que se hace por defecto

    // TODO: DEBUGGING
    temp = USERS;
    // Bucle para recorrer la lista enlazada
    while (temp != NULL) {
        printf("Usuario: %s", temp->username);
        temp = temp->next;
    }

    return 0;
};

int connect_user(char *username, char *host, int port) {
    //1. Comprobación de la corrección de los argumentos
    if (check_size256(username) < 0) {
        return 3;
    }
    // TODO: check host y check port?

    //2. Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (strcmp(temp->username, username)) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    //3. Se guarda el puerto donde está escuchando peticiones el usuario
    if (temp->port != -1) {
        printf("El usuario %s ya está conectado.\n", username);
        return 2;
    } else {
        strcpy(temp->host, host);
        temp->port = port;
    };

    return 0;
};

int publish_file(char *username, char *filename, char *description) {
    // Se busca el username del cliente que está haciendo la solicitud y se almacena el nuevo archivo

    //1. Comprobación de la corrección de los argumentos
    if (check_size256(username) < 0 || check_size256(filename) < 0 || check_size256(description) < 0 || check_blanks(filename) < 0) {
        printf("publish_file: error en los argumentos de entrada\n");
        return 4;
    }


    //2. Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("publish_file: lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (strcmp(temp->username, username)) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    //3. Comprobación de conexión del usuario
    if (temp->port == -1) {   // TODO: cómo definir que está o no conectado? NULL u otro valor numérico?
        printf("El usuario %s no está conectado al servicio", username);
        return 2;
    }

    //4. Inserción del fichero

    // Se hace una búsqueda de los archivos (si existen), comprobando que el archivo a guardar no está ya guardado
    // Si es así, se busca el final de la lista enlazada de archivos del usuario y se almacena el nuevo archivo

    struct file *new_file = (struct file*)malloc(sizeof(struct file));
    if (new_file == NULL) {
        printf("Error al reservar memoria dinámica.");
        return 4;
    }

    struct file *temp_file = temp->files;


    find = 0;
    if (temp_file == NULL) {    // Caso de lista de archivos vacía
        temp_file = new_file;
        find= 1;
    }

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

    // Se asignan los valores indicados por el usuario a la nueva estructura
    strcpy(new_file->filename, filename);
    strcpy(new_file->description, description);
    temp_file->next = new_file;
    temp->N_files++;
    //new_file->next = NULL; //TODO: ¿?

    return 0;
};

/*
int get_user(char *username, char *host, int *port) { // añadir puntero a struct file podría aligerar el código
    // ¿¿Meter en .h??
    // Búsqueda de un elemento de la lista enlazada dado su identificador (el username)
    if (check_size256(username) < 0) {
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
        if (strcmp(temp->username, username)) {
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
    //1. Comprobación de la corrección de los argumentos
    if (check_size256(username) < 0 || check_size256(filename) < 0 || check_blanks(filename) < 0) {
        return 4;
    }

    // 2. Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (strcmp(temp->username, username)) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    //3. Comprobación de conexión del usuario
    if (temp->port == -1) {   // TODO: cómo definir que está o no conectado? NULL u otro valor numérico?
        printf("El usuario %s no está conectado al servicio", username);
        return 2;
    }

    // 4. Búsqueda de fichero
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

    //5. Borrado de fichero
    if(prev_file == NULL){
      //se elimina el primer elemento
      temp->files = temp_file->next;
    }else{
      //no es el primer elemento
      prev_file->next = temp_file->next;
    }
    temp->N_files--;
    free(temp_file);
    return 0;
};

int list_users(char *username, int *counter, struct user_data_item* user_list) {
                                                // La pregunta es cómo se va a pasar toda la información leída ¿leerlos de uno en uno desde el servidor?
    // 1. Comprobación de la corrección de los argumentos
    if (check_size256(username) < 0) {
        return 3;
    }


    //2. Comprobación de usuario
    // Se comprueba que el usuario que realiza la petición está registrado y conectado (username)


    // Caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (strcmp(temp->username, username)) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Comprobación de requisito de conexión del usuario
    if (temp->port == -1) {   // TODO: cómo definir que está o no conectado? NULL u otro valor numérico?
        printf("El usuario %s no está conectado al servicio", username);
        return 2;
    }

    //3. Contabilizar usuarios
    // Declaración de variables auxiliares para recorrer la lista de usuarios
    int contador = 0;
    temp = USERS;
    struct user_data_item *temp_new_user = user_list;

    while (temp != NULL) {
        if(temp->port != -1){ //si el usuario está conectado...

            //creación de nodo de la lista resultado
            struct user_data_item *new_user = (struct user_data_item *)malloc(sizeof(struct user_data_item)); //liberada por el servidor -después de escribir en el socket

            if (new_user == NULL) {
                printf("Error al reservar memoria dinámica.");
                return 2;
            }

            strcpy(new_user->username, temp->username);
            strcpy(new_user->host, temp->host);
            new_user->port = temp->port;

            if (user_list == NULL) {
                //lista resultado vacía -insercción de primer elemento
                user_list = new_user;

            } else {
                // insertar el nuevo elemento al final de la lista enlazada
                temp_new_user->next = new_user;
            }
            contador++;
        }
        temp = temp->next;

    }

    *counter = contador;
    return 0;
};

int list_content(char *username, char *searched_username, int * counter, struct file* user_files) {     // username es el que realiza la petición
                                                                // searched_username el usuario del que quiere conocer
                                                                // los archivos publicados
                                                                // TODO: mismo problema de list_users() con el envío de la info + también hay que devolver un contador como en list_users()
    // 1. Comprobación de la corrección de los argumentos
    if (check_size256(username) < 0 || check_size256(searched_username) < 0) {
        return 4;
    }

    //2. Comprobación usuarios
    // Se comprueba que el usuario que realiza la petición está registrado y conectado (username)
    // En el mismo bucle se comprueba que exista el usuario buscado

    // Caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find_user = 0;
    int find_searched = 0;
    struct user *temp = USERS;

    while (find_user == 0 || find_searched==0) {
        if (strcmp(temp->username, username)) {
            if (temp->port == -1) {
                printf("El usuario %s no está conectado al servicio", username);
                return 2;
            }
            find_user =1;
        }
        if (temp->username == searched_username) {
          find_searched = 1;
        }

        if (temp->next == NULL) {
            if(find_user == 0){
                printf("El usuario %s no está en la lista.\n", username);
                return 1;
            }else if(find_searched == 0){
                printf("El usuario %s cuyo contenido se quiere conocer no está en la lista.\n", searched_username);
                return 3;
            }
        } else {
            temp = temp->next;
        }
    }


    // 3. Contabilizar archivos
    // Una vez hallado el usuario se devuelven sus archivos y el número registrado

    // Caso de usuario sin archivos publicados
    if (temp->files == NULL) {
        printf("El usuario %s no ha publicado ningún archivo.\n", searched_username);
    }

    *counter = temp->N_files;
    user_files = temp->files;
    return 0;
};

int disconnect_user(char *username) {
    // Al desconectarse un usuario, deja de escuchar en el puerto almacenado

    // 1. Comprobación de la corrección de los argumentos
    if (check_size256(username) < 0) {
        return 3;
    }

    // 2. Búsqueda del usuario

    // caso de lista enlazada vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;

    while (find == 0) {
        if (strcmp(temp->username, username)) {
            find=1;
        } else if (temp->next == NULL) {
            printf("El usuario %s no está en la lista.\n", username);
            return 1;
        } else {
            temp = temp->next;
        }
    }

    // Comprobación de conexión previa
    if (temp->port == -1) {
        printf("El usuario %s no estaba conectado.\n", username);
        return 2;
    }

    //3. Eliminar parámetros de conexión

    memset(temp->host, '\0', sizeof(temp->host)); //TODO: solución temporal -> pendiente verificar comportamiento.
    temp->port = -1;
    return 0;
};

int unregister_user(char *username) {
    // Se elimina el usuario de la lista

    // 1. Comprobación del valor de usuario
    if (check_size256(username) < 0) {
        return 2;
    }

    // 2. Comprobación existencia usuario
    // Caso de lista vacía
    if (USERS == NULL) {
        printf("Lista vacía; usuario %s no encontrado.\n", username);
        return 1;
    }

    int find = 0;
    struct user *temp = USERS;
    struct user *prev = NULL;

    while (find == 0) {
        if (strcmp(temp->username, username)) {
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
