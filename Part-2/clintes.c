#include "common.h"

void checkExistsFifoServidor_C1(char *nameFifo) {
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Check if FIFO server exists
    if (access(nameFifo, F_OK) == -1) {
        so_error("O FIFO do Servidor não existe.");
        exit(EXIT_FAILURE);
    }

    so_debug(">");
}

void triggerSignals_C2() {
    so_debug("<");

    // Arm and handle signals
    if (signal(SIGUSR1, trataSinalSIGUSR1_C8) == SIG_ERR || 
        signal(SIGHUP, trataSinalSIGHUP_C9) == SIG_ERR || 
        signal(SIGINT, trataSinalSIGINT_C10) == SIG_ERR || 
        signal(SIGALRM, trataSinalSIGALRM_C11) == SIG_ERR) {
        so_error("Erro ao armar os sinais.");
        exit(EXIT_FAILURE);
    }

    so_debug(">");
}

CheckIn getAuthenticationData_C3() {
    so_debug("<");

    CheckIn request;
    request.nif = -1; // Default value indicates error

    // Prompt user to enter NIF
    printf("IscteFlight: Check-in Online\n");
    printf("----------------------------\n");
    printf("Introduza o NIF do passageiro: ");
    if (scanf("%d", &request.nif) != 1 || request.nif < 0 || request.nif > 999999999) {
        so_error("NIF inválido. Introduza um NIF válido (até 9 dígitos).");
        exit(EXIT_FAILURE);
    }

    // Prompt user to enter password
    printf("Introduza a Senha do passageiro: ");
    scanf("%s", request.senha);

    so_debug("> [@return nif:%d, senha:%s]", request.nif, request.senha);
    return request;
}

CheckIn createClientRequest_C4(int pidCliente, CheckIn authData) {
    so_debug("< [@param pidCliente:%d, authData.nif:%d, authData.senha:%s]", pidCliente, authData.nif, authData.senha);

    CheckIn clientRequest;
    clientRequest.nif = authData.nif;
    strcpy(clientRequest.senha, authData.senha);
    clientRequest.pidCliente = pidCliente;

    so_success("Dados do cliente:", clientRequest.nif, clientRequest.senha, clientRequest.pidCliente);

    so_debug("> [@return clientRequest.nif:%d, clientRequest.senha:%s, clientRequest.pidCliente:%d]", 
                clientRequest.nif, clientRequest.senha, clientRequest.pidCliente);
    return clientRequest;
}

void writeRequest_C5(CheckIn request, char *nameFifo) {
    so_debug("< [@param request.nif:%d, request.senha:%s, request.pidCliente:%d, nameFifo:%s]",
                                        request.nif, request.senha, request.pidCliente, nameFifo);

    // Open FIFO server
    int fd = open(nameFifo, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o FIFO do Servidor para escrita");
        exit(EXIT_FAILURE);
    }

    // Write client request to FIFO server
    if (write(fd, &request, sizeof(CheckIn)) == -1) {
        perror("Erro ao escrever no FIFO do Servidor");
        exit(EXIT_FAILURE);
    }

    close(fd);
    so_success("Dados enviados para o servidor:", request.nif, request.senha, request.pidCliente);

    so_debug(">");
}

void configureTimer_C6(int tempoEspera) {
    so_debug("< [@param tempoEspera:%d]", tempoEspera);

    alarm(tempoEspera);
    so_success("Espera resposta em", tempoEspera, "segundos");

    so_debug(">");
}

void waitForEvents_C7() {
    so_debug("<");

    pause();

    so_debug(">");
}

void trataSinalSIGUSR1_C8(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    so_success("Check-in concluído com sucesso");
    exit(EXIT_SUCCESS);

    so_debug(">");
}

void trataSinalSIGHUP_C9(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    so_success("Check-in concluído sem sucesso");
    exit(EXIT_SUCCESS);

    so_debug(">");
}

void trataSinalSIGINT_C10(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    so_success("Cliente: Shutdown");
    exit(EXIT_SUCCESS);

    so_debug(">");
}

void trataSinalSIGALRM_C11(int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    so_error("Cliente: Timeout");
    exit(EXIT_FAILURE);

    so_debug(">");
}

int main() {
    // Constants
    char *FIFO_SERVER = "server.fifo";
    int MAX_ESPERA = 60;

    // C1: Validate FIFO server existence
    checkExistsFifoServidor_C1(FIFO_SERVER);

    // C2: Arm and handle signals
    triggerSignals_C2();

    // C3 + C4: Get authentication data and create client request
    CheckIn authData = getAuthenticationData_C3();
    int pidCliente = getpid();
    CheckIn clientRequest = createClientRequest_C4(pidCliente, authData);

    // C5: Write client request to FIFO server
    writeRequest_C5(clientRequest, FIFO_SERVER);

    // C6: Configure timer
    configureTimer_C6(MAX_ESPERA);

    // C7: Wait for events
    waitForEvents_C7();

    so_exit_on_error(-1, "ERRO: O cliente nunca devia chegar a este ponto");
}
