#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ESPERA 10

// Funções auxiliares
void so_error(char *step, char *message) {
    fprintf(stderr, "Erro no passo %s: %s\n", step, message);
    exit(1);
}

void so_success(char *step, char *message) {
    fprintf(stderr, "Sucesso no passo %s: %s\n", step, message);
}

void readClientRequest(char *clientRequest, char *fifoName) {
    int fd;
    if ((fd = open(fifoName, O_RDONLY)) < 0) {
        so_error("S4", "Erro ao abrir FIFO do servidor para leitura");
    }

    if (read(fd, clientRequest, sizeof(clientRequest)) < 0) {
        so_error("S4", "Erro ao ler do FIFO do servidor");
    }

    if (close(fd) < 0) {
        so_error("S4", "Erro ao fechar o FIFO do servidor");
    }
}

void writeSuccess(char *message, char *fifoName) {
    int fd;
    if ((fd = open(fifoName, O_WRONLY)) < 0) {
        so_error("S5", "Erro ao abrir FIFO do cliente para escrita");
    }

    if (write(fd, message, strlen(message)) < 0) {
        so_error("S5", "Erro ao escrever no FIFO do cliente");
    }

    if (close(fd) < 0) {
        so_error("S5", "Erro ao fechar o FIFO do cliente");
    }
}

// Handlers para os sinais
void sigintHandler(int sig) {
    so_success("S6", "Servidor: Start Shutdown");
    // Implementar restante do tratamento do sinal SIGINT
}

void sigchldHandler(int sig) {
    // Implementar tratamento do sinal SIGCHLD
}

// Função principal
int main() {
    char fifoName[] = "server.fifo";
    char clientRequest[256];

    // Verificar a existência e permissões do ficheiro bd_passageiros.dat

    // Criar FIFO do servidor
    if (mkfifo(fifoName, 0666) < 0) {
        so_error("S2", "Erro ao criar FIFO do servidor");
    } else {
        so_success("S2", "");
    }

    // Armar e tratar os sinais SIGINT e SIGCHLD
    if (signal(SIGINT, sigintHandler) == SIG_ERR || signal(SIGCHLD, sigchldHandler) == SIG_ERR) {
        so_error("S3", "Erro ao armar os sinais");
    } else {
        so_success("S3", "");
    }

    while (1) {
        int fd;
        // Abrir FIFO do servidor para leitura
        if ((fd = open(fifoName, O_RDONLY)) < 0) {
            so_error("S4", "Erro ao abrir FIFO do servidor para leitura");
        }

        // Ler informações do cliente do FIFO
        readClientRequest(clientRequest, fifoName);

        // Criar processo filho (Servidor Dedicado)
        pid_t pidServidorDedicado = fork();
        if (pidServidorDedicado < 0) {
            so_error("S5", "Erro ao criar processo filho (Servidor Dedicado)");
        } else if (pidServidorDedicado == 0) {
            // Processo filho (Servidor Dedicado)
            // Implementar lógica do Servidor Dedicado
        } else {
            // Processo pai
            printf("Servidor: Iniciei SD %d\n", pidServidorDedicado);
        }
    }

    return 0;
}

