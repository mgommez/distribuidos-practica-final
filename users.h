
#ifndef USERS_H
#define USERS_H


// Estructura de cada archivo de la lista enlazada de archivos de un usuario
struct file {
    char filename[256];
    char description[256];
    struct file *next;
};

// Estructura de cada elemento de la lista enlazada del servidor
struct user {
    char username[256];
    char host[20]; // xxx.xxx.xxx.xxx\0 ó localhost
    int port;
    struct file *files;
    struct user *next;
};

int register_user(char *username);

int connect_user(char *username, char*host, int port);

int publish_file(char *filename, char *description);

int list_users(void);

int list_content(char *username);

int delete_file(char *filename);

// get_file() iría en el cliente

int disconnect_user(char *username);

int unregister_user(char *username);

// int destroy(void); ¿?



/**
 * @brief Esta llamada permite inicializar el servicio de elementos clave-valor1-valor2-valor3.
 * Mediante este servicio se destruyen todas las tuplas que estuvieran almacenadas previamente.
 *
 * @return int La función devuelve 0 en caso de éxito y -1 en caso de error.
 * @retval 0 en caso de exito.
 * @retval -1 en caso de error.
 */
int destroy(void);


#endif