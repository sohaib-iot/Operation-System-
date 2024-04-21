#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define FILE_DATABASE "bd_passageiros.dat"
#define FILE_REQUESTS "server.fifo"

// Define CheckIn struct if it's not already defined
#ifndef CHECKIN_STRUCT_DEFINED
#define CHECKIN_STRUCT_DEFINED
typedef struct {
    int nif;
    char senha[40];
    char nome[60];
    char nrVoo[8];
    int pidCliente;
    int pidServidorDedicado;
} CheckIn;
#endif

void _student_checkExistsDB_S1(char *nameDB) {
    so_debug("< [@param nameDB:%s]", nameDB);

    // Check if the database file exists
    if (access(nameDB, F_OK) == -1) {
        so_error("Database file does not exist");
        exit(EXIT_FAILURE);
    }

    so_success("Database file exists");
}

void _student_createFifo_S2(char *nameFifo) {
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Create FIFO for server requests
    if (mkfifo(nameFifo, 0666) == -1) {
        so_error("Error creating FIFO for server requests");
        exit(EXIT_FAILURE);
    }

    so_success("FIFO created successfully");
}

void _student_triggerSignals_S3(char *nameFifo) {
    so_debug("<");

    // Implement signal handling for server

    so_debug(">");
}

CheckIn _student_readRequest_S4(char *nameFifo) {
    CheckIn request;
    request.nif = -1;   // Default value indicates error
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Read request from FIFO
    int fd = open(nameFifo, O_RDONLY);
    if (fd == -1) {
        so_error("Error opening FIFO for reading");
        request.nif = -1;
        return request;
    }

    if (read(fd, &request, sizeof(CheckIn)) == -1) {
        so_error("Error reading from FIFO");
        request.nif = -1;
    }

    close(fd);
    so_success("Request read successfully", request.nif, request.senha, request.pidCliente);
    return request;
}

int _student_createServidorDedicado_S5() {
    int pid_filho = -1;    // Default value indicates error
    so_debug("<");

    // Create dedicated server process
    pid_filho = fork();
    if (pid_filho == -1) {
        so_error("Error creating dedicated server process");
        exit(EXIT_FAILURE);
    }

    if (pid_filho == 0) {
        // Child process
        so_success("Dedicated server process created successfully");
    }

    so_debug("> [@return:%d]", pid_filho);
    return pid_filho;
}

void _student_triggerSignals_SD9() {
    so_debug("<");

    // Implement signal handling for dedicated server

    so_debug(">");
}

int _student_searchClientDB_SD10(CheckIn request, char *nameDB, CheckIn *itemDB) {
    int indexClient = -1;    // Default value indicates error
    so_debug("< [@param request.nif:%d, request.senha:%s, nameDB:%s, itemDB:%p]", request.nif,
             request.senha, nameDB, itemDB);

    // Search for client in the database file
    FILE *file = fopen(nameDB, "rb");
    if (file == NULL) {
        so_error("Error opening database file for reading");
        return indexClient;
    }

    CheckIn temp;
    indexClient = 0;
    while (fread(&temp, sizeof(CheckIn), 1, file) == 1) {
        if (temp.nif == request.nif) {
            *itemDB = temp;
            fclose(file);
            so_success("Client found in database", indexClient, temp.nome, temp.nrVoo);
            return indexClient;
        }
        indexClient++;
    }

    fclose(file);
    so_error("Client not found in database");
    return -1;
}

void _student_checkinClientDB_SD11(CheckIn *request, char *nameDB, int indexClient, CheckIn itemDB) {
    so_debug("< [@param request:%p, nameDB:%s, indexClient:%d, itemDB.pidServidorDedicado:%d]",
             request, nameDB, indexClient, itemDB.pidServidorDedicado);

    // Update client information in the database
    FILE *file = fopen(nameDB, "rb+");
    if (file == NULL) {
        so_error("Error opening database file for writing");
        return;
    }

    fseek(file, indexClient * sizeof(CheckIn), SEEK_SET);
    fwrite(request, sizeof(CheckIn), 1, file);
    fclose(file);

    so_success("Client checked in successfully", request->nome, request->nrVoo, request->pidServidorDedicado);
}

void _student_sendAckCheckIn_SD12(int pidCliente) {
    so_debug("< [@param pidCliente:%d]", pidCliente);

    // Send acknowledgment to client
    kill(pidCliente, SIGUSR1);

    so_success("Acknowledgment sent to client");
}

void _student_closeSessionDB_SD13(CheckIn clientRequest, char *nameDB, int indexClient) {
    so_debug("< [@param clientRequest:%p, nameDB:%s, indexClient:%d]", &clientRequest, nameDB,
             indexClient);

    // Close session and update client information in the database
    clientRequest.pidServidorDedicado = getpid();
    checkinClientDB_SD11(&clientRequest, nameDB, indexClient, clientRequest);

    so_success("Session closed and database updated", clientRequest.pidCliente, 
                clientRequest.pidServidorDedicado);
}

void _student_trataSinalSIGUSR2_SD14(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Implement signal handling for SIGUSR2

    so_debug(">");
}

void _student_main() {
    // S1
    checkExistsDB_S1(FILE_DATABASE);
    // S2
    createFifo_S2(FILE_REQUESTS);
    // S3
    triggerSignals_S3(FILE_REQUESTS);

    int indexClient;       // Index of the client that made the request to the server/dedicated server in the DB

    // S4: CICLO1
    while (1) {
        // S4
        CheckIn clientRequest = readRequest_S4(FILE_REQUESTS); 
        if (clientRequest.nif < 0)
            continue;

        // S5
        int pidServidorDedicado = createServidorDedicado_S5();
        if (pidServidorDedicado > 0)
            continue;

        // SD9
        triggerSignals_SD9();
        // SD10
        CheckIn itemBD;
        indexClient = searchClientDB_SD10(clientRequest, FILE_DATABASE, &itemBD);
        // SD11
        checkinClientDB_SD11(&clientRequest, FILE_DATABASE, indexClient, itemBD);
        // SD12
        sendAckCheckIn_SD12(clientRequest.pidCliente);
        // SD13
        closeSessionDB_SD13(clientRequest, FILE_DATABASE, indexClient);
    }

    so_exit_on_error(-1, "ERROR: The server should never reach this point");
}
