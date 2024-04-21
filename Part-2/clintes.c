#include "cliente.h"

// Implementação das funções do cliente
void checkExistsFifoServidor_C1(char *serverFifo) {
    struct stat st;
    if (stat(serverFifo, &st) != 0 || !S_ISFIFO(st.st_mode)) {
        so_error("C1", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("C1", "");
    }
}

void triggerSignals_C2() {
    if (signal(SIGUSR1, trataSinalSIGUSR1_C8) == SIG_ERR || signal(SIGHUP, trataSinalSIGHUP_C9) == SIG_ERR ||
        signal(SIGINT, trataSinalSIGINT_C10) == SIG_ERR || signal(SIGALRM, trataSinalSIGALRM_C11) == SIG_ERR) {
        so_error("C2", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("C2", "");
    }
}

CheckIn getDadosPedidoUtilizador_C3_C4() {
    CheckIn clientRequest;
    printf("IscteFlight: Check-in Online\n");
    printf("----------------------------\n");
    printf("Introduza o NIF do passageiro: ");
    so_geti(&(clientRequest.nif));
    printf("Introduza a Senha do passageiro: ");
    so_gets(clientRequest.senha, sizeof(clientRequest.senha));

    // Preenche o PID do cliente
    clientRequest.pidCliente = getpid();

    so_success("C4", "%d %s %d", clientRequest.nif, clientRequest.senha, clientRequest.pidCliente);
    return clientRequest;
}

void writeRequest_C5(CheckIn request, char *serverFifo) {
    int fd;
    if ((fd = open(serverFifo, O_WRONLY)) == -1) {
        so_error("C5", "");
        exit(EXIT_FAILURE);
    }

    dprintf(fd, "%d\n%s\n%d\n", request.nif, request.senha, request.pidCliente);

    if (close(fd) == -1) {
        so_error("C5", "");
        exit(EXIT_FAILURE);
    } else {
        so_success("C5", "");
    }
}

void configureTimer_C6(int maxWaitTime) {
    alarm(maxWaitTime);
    so_success("C6", "Espera resposta em %d segundos", maxWaitTime);
}

void waitForEvents_C7() {
    pause();
}

void trataSinalSIGUSR1_C8(int sig) {
    so_success("C8", "Check-in concluído com sucesso");
    exit(EXIT_SUCCESS);
}

void trataSinalSIGHUP_C9(int sig) {
    so_success("C9", "Check-in concluído sem sucesso");
    exit(EXIT_FAILURE);
}

void trataSinalSIGINT_C10(int sig) {
    so_success("C10", "Cliente: Shutdown");
    exit(EXIT_SUCCESS);
}

void trataSinalSIGALRM_C11(int sig) {
    so_error("C11", "Cliente: Timeout");
    exit(EXIT_FAILURE);
}
