//
// Created by crist on 18/04/2025.
//

#ifndef MESSAGE_H
#define MESSAGE_H

// Estructura de peticiones del servidor
struct peticion {
    char username [256];
    char user_wanted[256];
    int port;
    char host [20];
    char filename [256];
    char description [256];
};

// Estructura manejo de usuarios y ficheros
struct data{
    int counter;
    struct user_data_item *user_list;
    struct file *file_list;
};

/*
// Estructura envío de logs del servidor al servidor RPC
struct log_data{
    char username [256];
    char op [20]; // LIST_CONTENT es la operación más larga, con 12 caracteres
                  // pero se ponen 20 caracteres siendo consistente con el diseño de server.c
    char fileName [256];
    char date_time [20];
};
 */

#endif //MESSAGE_H
