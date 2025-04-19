//
// Created by crist on 18/04/2025.
//

#ifndef MESSAGE_H
#define MESSAGE_H

//Estructura de peticiones del servidor
struct peticion {
    char username [256];
    char user_wanted[256];
    int port;
    char host [20];
    char filename [256];
    char description [256];
};

//Estrcutura manejo de usuarios y ficheros
struct data{
    int counter;
    struct user_data_item *user_list;
    struct file *file_list;
};

#endif //MESSAGE_H
