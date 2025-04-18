//
// Created by crist on 18/04/2025.
//

#ifndef MESSAGE_H
#define MESSAGE_H

//Estructura de peticiones del servidor
struct peticion {
    char username[256];
    int port;
    char filename [256];
    char description [256];
};

//Estructura de respuestas del servidor
struct respuesta {
    char username[256];
    int port;
    char filename [256];
    char description [256];
};
#endif //MESSAGE_H
