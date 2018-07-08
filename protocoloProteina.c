
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define N_CLIENTES 1
void *thread_result_cliente;
void *thread_result_servidor;

pthread_t cliente[N_CLIENTES];
pthread_t servidor;

//struct mensagem
typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;

void *clienteFuncao() {
	char *rem_hostname;
	int rem_port;
	struct sockaddr_in rem_addr;
	int rem_sockfd;
	char linha[81];	


	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (fam�lia, IP, porta) */
	rem_hostname = "127.0.0.1";
	rem_port = atoi("123");
	rem_addr.sin_family = AF_INET; /* familia do protocolo*/
	rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
	rem_addr.sin_port = htons(rem_port); /* porta local  */

   	/* Cria o socket para enviar e receber datagramas */
	/* parametros(familia, tipo, protocolo) */
	rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (rem_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}
	printf("> Conectando no servidor '%s:%d'\n", rem_hostname, rem_port);

   	/* Estabelece uma conex�o remota */
	/* parametros(descritor socket, estrutura do endereco remoto, comprimento do endereco) */
	if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
		perror("Conectando stream socket");
		exit(1);
	}
	//------------------------------------------------------------------------//
/* Inicializando mensagem */
aatp_msg m = { 0 };
/* Preenchendo dados */
m.method = 'S'; /* Solicita��o */
m.size = 5;
/* Zerando payload */
memset(&m.payload, 0, sizeof m.payload);
/* Enviando solicita��o */
int r = send(rem_sockfd, &m, sizeof(m), 0);
/* Criando buffer */
aatp_msg recv_buffer;
size_t recv_buffer_len = sizeof(recv_buffer);
/* Recebendo mensagem */
recv(rem_sockfd, &recv_buffer, recv_buffer_len, 0);
/* Imprimindo resultados */
printf("\t Method: %c\n", recv_buffer.method);
printf("\t Size: %d\n", recv_buffer.size);
printf("\t Payload: %s\n", recv_buffer.payload);

//----------------------------------------------------------------------------//
	/* fechamento do socket remota */ 	
	close(rem_sockfd);
}
 
void *servidorFuncao(){
		/* Variaveis Locais */
	int loc_sockfd, loc_newsockfd, tamanho;
	char linha[81];		
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
	/* Preenchendo a estrutura socket loc_addr (fam�lia, IP, porta) */
	loc_addr.sin_family = AF_INET; /* familia do protocolo*/
	loc_addr.sin_addr.s_addr = INADDR_ANY; /* endereco IP local */
	loc_addr.sin_port = htons(123); /* porta local  */
	bzero(&(loc_addr.sin_zero), 8);

   	/* Bind para o endereco local*/
	/* parametros(descritor socket, estrutura do endereco local, comprimento do endereco) */
	if (bind(loc_sockfd, (struct sockaddr *) &loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}
	
	/* parametros(descritor socket,
	numeros de conex�es em espera sem serem aceites pelo accept)*/
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
m.method = 'R'; /* Requisi��o */
m.size = 5; /* Enviar a quantidade que foi solicitada */
/* Zerando payload para evitar enviar lixo
   caso seja feita uma solicita��o de menos de 5 amino�cidos */
memset(&m.payload, 0, sizeof m.payload);
/* Preencher com os dados */
m.payload[0] = 'A';
m.payload[1] = 'B';
m.payload[2] = 'C';
m.payload[3] = 'D';
m.payload[4] = 'E';
/* Enviando solicita��o */
int r = send(loc_newsockfd, &m, sizeof(m), 0);

close(loc_sockfd);
	close(loc_newsockfd);
}
 main(){
 
     
    pthread_create(&servidor,NULL, servidorFuncao, NULL);
    pthread_join(servidor, &thread_result_servidor);
    sleep(2);
    int i;
    for(i=0; i<N_CLIENTES; i++) {
       pthread_create(&cliente[i], NULL, clienteFuncao, NULL);
   	}
    for(i=0; i<N_CLIENTES; i++) {
        pthread_join(cliente[i],&thread_result_cliente);
	}

}	
