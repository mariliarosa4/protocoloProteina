#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<time.h>

#define PORTA 1337
char gerarAminoacido() {
  //  char aminoacidos[61]= "ARRNNNNDCQEGGGGHHHHIIIIIIIILKMMMMMMMMFPPPPSTWWWWWWWWYYYYYYYYV";
  char aminoacidos[20]= "ARNDCQEGHILKMFPSTWYV";

    int num = rand()%20;//61
    printf("%d", num);
    printf("%c", aminoacidos[num]);
    return aminoacidos[num];
}
typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;
int main() {
	    srand(time(NULL));
    /* Variaveis Locais */
    int loc_sockfd, loc_newsockfd, tamanho;

    /* Estrutura: familia + endereco IP + porta */
    struct sockaddr_in loc_addr;


    /* Cria o socket para enviar e receber datagramas */
    /* parametros(familia, tipo, protocolo) */
    loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (loc_sockfd < 0) {
        perror("Criando stream socket");
        exit(1);
    }

    /* Construcao da estrutura do endereco local */
    /* Preenchendo a estrutura socket loc_addr (família, IP, porta) */
    loc_addr.sin_family = AF_INET; /* familia do protocolo*/
    loc_addr.sin_addr.s_addr = INADDR_ANY; /* endereco IP local */
    loc_addr.sin_port = htons(PORTA); /* porta local  */
    bzero(&(loc_addr.sin_zero), 8);

    /* Bind para o endereco local*/
    /* parametros(descritor socket, estrutura do endereco local, comprimento do endereco) */
    if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
        perror("Ligando stream socket");
        exit(1);
    }

    /* parametros(descritor socket,
    numeros de conexões em espera sem serem aceites pelo accept)*/
    while(1) {
        listen(loc_sockfd, 5);
        printf("> aguardando conexao\n");

        tamanho = sizeof(struct sockaddr_in);
        /* Accept permite aceitar um pedido de conexao, devolve um novo "socket" ja ligado ao emissor do pedido e o "socket" original*/
        /* parametros(descritor socket, estrutura do endereco local, comprimento do endereco)*/
        loc_newsockfd =	accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);

        aatp_msg recv_buffer;
        size_t recv_buffer_len = sizeof(recv_buffer);
        /* Recebendo mensagem */
        recv(loc_newsockfd, &recv_buffer, recv_buffer_len, 0);

        /* Inicializando mensagem */
        aatp_msg m = { 0 };
        /* Preenchendo dados */
        m.method = 'R'; /* resposta */
        m.size = recv_buffer.size; /* Enviar a quantidade que foi solicitada */
        /* Zerando payload para evitar enviar lixo
           caso seja feita uma solicitação de menos de 5 aminoácidos */
        memset(&m.payload, 0, sizeof m.payload);
        /* Preencher com os dados */
        int k;
        //sorteia os aminoacidos conforme a quantidade que o usuário pediu
        if ((int)m.size>5) {
            m.payload[0] =gerarAminoacido();
        } else {
            for (k=0; k<(int)m.size; k++) {
                m.payload[k] =gerarAminoacido();
            }
        }
printf("\t Method: %c\n", m.method);
printf("\t Size: %d\n", m.size);
printf("\t Payload: %s\n", m.payload);
        /* Enviando solicitação */
        int r = send(loc_newsockfd, &m, sizeof(m), 0);

        close(loc_newsockfd);
    }

    close(loc_sockfd);
}
