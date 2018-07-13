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
    char aminoacidos[61]= "ARRNNNNDCQEGGGGHHHHIIIIIIIILKMMMMMMMMFPPPPSTWWWWWWWWYYYYYYYYV";
// char aminoacidos[20]= "ARNDCQEGHILKMFPSTWYV";
    int num = rand()%61;//61
    return aminoacidos[num];
}
typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;



void *servidorFuncao(void *socket){
	int socketCliente = *(int*)socket;
	int read_size;
        aatp_msg recv_buffer;
        size_t recv_buffer_len = sizeof(recv_buffer);
        /* Recebendo mensagem */
        while((read_size = recv(socketCliente, &recv_buffer, recv_buffer_len, 0) > 0)){

        /* Inicializando mensagem */
       	aatp_msg m = { 0 };
        /* Preenchendo dados */
        m.method = 'R'; /* resposta */
        m.size = recv_buffer.size; 

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
        
		printf("\t Method: %c     |    Size: %d    | Payload: %s  \n", m.method,  m.size, m.payload);

        /* Enviando solicitação */
        int r = send(socketCliente, &m, sizeof(m), 0);
	}

        close(socketCliente);
        return 0;
}
int main() {
	    srand(time(NULL));
    int loc_sockfd, socket_cliente, tamanho;

    struct sockaddr_in loc_addr , cli;

    loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (loc_sockfd < 0) {
        perror("Criando stream socket");
        exit(1);
    }

    loc_addr.sin_family = AF_INET; /* familia do protocolo*/
    loc_addr.sin_addr.s_addr = INADDR_ANY;
    loc_addr.sin_port = htons(atoi("1337")); 
    bzero(&(loc_addr.sin_zero), 8);

  
    if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
        perror("Ligando stream socket");
        exit(1);
    }

            listen(loc_sockfd, 5);
             tamanho = sizeof(struct sockaddr_in);
             printf("> aguardando conexao\n");
               int cont = 0;
             pthread_t thread_id;
          
    while((socket_cliente =	accept(loc_sockfd, (struct sockaddr *)&cli, &tamanho))) {
  		printf("-------------------Connection accepted %d ----------------- \n  ", cont);

       	if (pthread_create(&thread_id,NULL, servidorFuncao, &socket_cliente)<0){
       		perror("Nao foi possivel criar a thread");
       		return 1;
	   }
	cont++;


    }
    close(loc_sockfd);
return 0;

}
