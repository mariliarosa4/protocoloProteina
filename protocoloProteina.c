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
#define N_CLIENTES 8 //N CLIENTES = N SERVIDORES um cliente pra cada servidor disponivel
void *thread_result_cliente;
void *thread_result_servidor;
void *thread_result_ribossomo;
char ipLinha[N_CLIENTES][20];
int flagClienteServidorAtivo[N_CLIENTES];
pthread_t cliente[N_CLIENTES];
pthread_t servidor;
pthread_t ribossomo;
pthread_mutex_t lock;
FILE *fProteina;
//struct mensagem
typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;
aatp_msg recv_buffer[N_CLIENTES];


char *procuraAmino(char *proteina, char *amino, char *repl) {

    static char buffer[700];
    char *p;
    if (!(p = strstr(proteina, amino))){
     return "xx";	
	}else{
		strncpy(buffer, proteina, p-proteina);
    buffer[p-proteina] = 0;
    sprintf(buffer+(p-proteina), "%s%s", repl, p+strlen(amino));

    return buffer;
	}
       
    
}


void *clienteFuncao(void *j) {
    int iCliente = *(int *)j;
   // printf("comecando cliente %d \n", iCliente);
  
    flagClienteServidorAtivo[iCliente] = 1;
    while(flagClienteServidorAtivo[iCliente]==1) {
        char *rem_hostname;
        int rem_port;
        struct sockaddr_in rem_addr;
        int rem_sockfd;

        /* Construcao da estrutura do endereco local */
        /* Preenchendo a estrutura socket loc_addr (família, IP, porta) */

        rem_hostname = (ipLinha[iCliente]);
        rem_port = atoi("1337");
        rem_addr.sin_family = AF_INET; /* familia do protocolo*/
        rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
        rem_addr.sin_port = htons(rem_port); /* porta local  */

        rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (rem_sockfd < 0) {
            perror("Criando stream socket");
            exit(1);
        }

        if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
            perror("Conectando stream socket");
            flagClienteServidorAtivo[iCliente] = 0;
            //	exit(1);
        } else {

            //------------------------------------------------------------------------//
            /* Inicializando mensagem */
            aatp_msg m = { 0 };
            /* Preenchendo dados */
            m.method = 'S'; /* Solicitação */
            m.size = 5;

            memset(&m.payload, 0, sizeof m.payload);

            int r = send(rem_sockfd, &m, sizeof(m), 0);

            size_t recv_buffer_len = sizeof(recv_buffer);

            recv(rem_sockfd, &recv_buffer[iCliente], recv_buffer_len, 0);
           pthread_mutex_lock(&lock);
            int c ;

            for(c=0; c<recv_buffer[iCliente].size; c++) {
                if (recv_buffer[iCliente].payload[c]) {
                   rewind(fProteina);
                   char file[700];
            while(fgets(file, 700, fProteina) != NULL) {
                char *proteinaMudada;
                proteinaMudada = procuraAmino(file,&recv_buffer[iCliente].payload[c],"-");
                
                if (proteinaMudada!="xx"){
				 system("@cls||clear");
                printf("\n %s", proteinaMudada);
              	rewind(fProteina);
                fprintf(fProteina,"%s \n", proteinaMudada);
               // sleep(1);
            }
        }

                }
            }

            pthread_mutex_unlock(&lock);
            //mostrarLista(listaAmino);
            //sleep(5);
//----------------------------------------------------------------------------//
            /* fechamento do socket remota */
            close(rem_sockfd);
            //	flagClienteServidorAtivo[iCliente] = 0;
        }
    }
}
void *ribossomoFuncao() {

}

main() {
    pthread_mutex_init(&lock, NULL);


   // pthread_create(&ribossomo, NULL, ribossomoFuncao, NULL);
  //  sleep(2);

    FILE *fips;
    if ((fips = fopen("ips_servidores.txt", "r")) == NULL) {
        printf("Error! opening file");
        return 1;
    }
    if ((fProteina = fopen("proteina.txt", "r+")) == NULL) {
        printf("Error! opening file");
        return 1;
    }
    int i;
    i=0;
    while((fgets(ipLinha[i], 19, fips) != NULL )&& (i<N_CLIENTES)) {
        //		printf("CLIENTE %d", i);
        pthread_create(&cliente[i], NULL, clienteFuncao, &i);
        sleep(1);
        i++;
    }

 
  //  pthread_join(ribossomo, &thread_result_ribossomo);

    for(i=0; i<N_CLIENTES; i++) {
        pthread_join(cliente[i],&thread_result_cliente);
    }
    fclose(fProteina);
return 0;
}
