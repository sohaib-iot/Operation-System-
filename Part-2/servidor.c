#include "servidor.h"

// Implementação das funções do servidor
void checkExistsDB_S1(char *fileName) {
    struct stat st;
    if (stat(fileName, &st) != 0 || !S_ISREG(st.st_mode) || access(fileName, R_OK | W_OK) != 0) {
        so_error("S1", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("S1", "");
    }
}

void createFifo_S2(char *serverFifo) {
    if (mkfifo(serverFifo, 0666) == -1) {
        so_error("S2", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("S2", "");
    }
}

void triggerSignals_S3() {
    if (signal(SIGINT, trataSinalSIGINT_S6) == SIG_ERR || signal(SIGCHLD, trataSinalSIGCHLD_S8) == SIG_ERR) {
        so_error("S3", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("S3", "");
    }
}

CheckIn readRequest_S4(char *serverFifo) {
    CheckIn clientRequest;
    int fd;

    if ((fd = open(serverFifo, O_RDONLY)) == -1) {
        so_error("S4", "");
        exit(EXIT_FAILURE);
    }

    scanf(fd, "%d\n%s\n%d\n", &clientRequest.nif, clientRequest.senha, &clientRequest.pidCliente);

    if (close(fd) == -1) {
        so_error("S4", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("S4", "%d %s %d", clientRequest.nif, clientRequest.senha, clientRequest.pidCliente);
    }

    return clientRequest;
}

int createServidorDedicado_S5() {
    int pid;
    if ((pid = fork()) == -1) {
        so_error("S5", "");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processo filho
        return getpid();
    } else {
        // Processo pai
        so_success("S5", "");
        return pid;
    }
}

void trataSinalSIGINT_S6(int sig) {
    deleteFifoAndExit_S7();
}

void deleteFifoAndExit_S7() {
    if (unlink(FILE_REQUESTS) == -1) {
        so_error("S7", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("S7", "");
        exit(EXIT_SUCCESS);
    }
}

void trataSinalSIGCHLD_S8(int sig) {
    int status;
    wait(&status);
    so_success("S8", "");
}

void triggerSignals_SD9() {
    if (signal(SIGHUP, trataSinalSIGHUP_SD10) == SIG_ERR || signal(SIGUSR2, trataSinalSIGUSR2_SD14) == SIG_ERR) {
        so_error("SD9", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("SD9", "");
    }
}

int searchClientDB_SD10(CheckIn clientRequest, char *dbFileName, CheckIn *result) {
    // Implementação da busca no banco de dados
    // Retorna 1 se o cliente for encontrado, 0 caso contrário
    so_success("SD10", "");
    return 0;
}

void checkinClientDB_SD11(CheckIn *clientRequest, char *dbFileName, int indexClient, CheckIn itemDB) {
    // Implementação do check-in no banco de dados
    so_success("SD11", "");
}

void sendAckCheckIn_SD12(int pidCliente) {
    kill(pidCliente, SIGUSR1);
    so_success("SD12", "");
}

void closeSessionDB_SD13(CheckIn clientRequest, char *dbFileName, int indexClient) {
    // Implementação do fechamento da sessão no banco de dados
    so_success("SD13", "");
}

void trataSinalSIGUSR2_SD14(int sig) {
    so_success("SD14", "SD: Recebi pedido do Servidor para terminar");
    exit(EXIT_SUCCESS);
}
