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
#define N_CLIENTES 6 //N CLIENTES = N SERVIDORES um cliente pra cada servidor disponivel

void *thread_result_cliente;
void *thread_result_servidor;
pthread_t cliente[N_CLIENTES];
pthread_t servidor;
pthread_mutex_t lock;
int contConexoes;
int modo;
int posicaoRNA;
char fileProximo[610];
char copiaArquivoTranscricao[610];
char ipLinha[N_CLIENTES][20];
int flagClienteServidorAtivo[N_CLIENTES];

FILE *fModoTranscricao;
FILE *fmodoAlbum;


//struct protocolo aatp
typedef struct {
    uint8_t size;
    char method;
    char payload[5];
} aatp_msg;
aatp_msg recv_buffer[N_CLIENTES];


//funcao de procurar aminoacido modo album de figuras
char *procuraAmino(char *proteina, char *amino, char *repl) {

    static char buffer[610];
    char *p;
    if (!(p = strstr(proteina, amino))) {
        return "xx";
    } else {
        strncpy(buffer, proteina, p-proteina);
        buffer[p-proteina] = 0;
        sprintf(buffer+(p-proteina), "%s%s", repl, p+strlen(amino));
        return buffer;
    }


}

//funcao de procurar aminoacido modo transcricao
void  procuraAminoProximo(char amino) {
//	printf("amino %c \n", amino);
    //printf(" file %c \n", fileProximo[posicaoRNA]);
    //  sleep(2);
    fflush(stdin);
    if (fileProximo[posicaoRNA]==amino) {
        fputc(amino, fModoTranscricao);
        copiaArquivoTranscricao[posicaoRNA]=amino;
        posicaoRNA++;
    }
}



void *clienteFuncao(void *j) {
    int iCliente = *(int *)j;
    flagClienteServidorAtivo[iCliente] = 1;
    while(flagClienteServidorAtivo[iCliente]==1) {
        char *rem_hostname;
        int rem_port;
        struct sockaddr_in rem_addr;
        int rem_sockfd;

        rem_hostname = (ipLinha[iCliente]);
        rem_port = atoi("1337");
        rem_addr.sin_family = AF_INET; /* familia do protocolo*/
        rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
        rem_addr.sin_port = htons(rem_port); /* porta local  */

        rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (rem_sockfd < 0) {
            //  perror("Criando stream socket");
            exit(1);
        }

        if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
            // perror("Conectando stream socket");
            flagClienteServidorAtivo[iCliente] = 0;
            //	exit(1);
        } else {
            contConexoes++;
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

            if (modo==1) {
                int c ;
                char *proteinaMudada;
                for(c=0; c<recv_buffer[iCliente].size; c++) {
                    if (recv_buffer[iCliente].payload[c]) {
                        rewind(fmodoAlbum);
                        char file[610];
                        while(fgets(file, 610, fmodoAlbum) != NULL) {

                            proteinaMudada = procuraAmino(file,&recv_buffer[iCliente].payload[c],"-");

                            if (proteinaMudada!="xx") {
                                system("@cls||clear");
                                printf("\n %s \n Conexoes: %d \n", proteinaMudada, contConexoes);
                                rewind(fmodoAlbum);
                                fprintf(fmodoAlbum,"%s \n", proteinaMudada);
                                // sleep(1);
                            }
                        }

                    }
                }

            } else {


                int c ;

                for(c=0; c<recv_buffer[iCliente].size; c++) {
                    procuraAminoProximo(recv_buffer[iCliente].payload[c]);
                }
                system("@cls||clear");
                printf("\n %s \n Conexoes: %d \n", copiaArquivoTranscricao, contConexoes);
                if (posicaoRNA>=609) {
                    flagClienteServidorAtivo[iCliente] = 2;
                }
            }
            pthread_mutex_unlock(&lock);

            /* fechamento do socket remota */
            close(rem_sockfd);
        }

    }
    if (flagClienteServidorAtivo[iCliente] ==2) {
        printf("Proteina completa");
sleep(10);
	exit(1);
    }
}
    main() {
        pthread_mutex_init(&lock, NULL);
        contConexoes =0;
        FILE *copiaProteina;
        if ((copiaProteina = fopen("proteina_original.txt", "r")) == NULL) {
            printf("Error! opening file proteina_original.txt");
            return 1;
        }

        if ((fmodoAlbum = fopen("modoAlbum.txt", "r+")) == NULL) {
            printf("Error! opening file");
            return 1;
        }
        char k;
        k = fgetc(copiaProteina);
        while (k != EOF)
        {
            fputc(k, fmodoAlbum);
            k = fgetc(copiaProteina);
        }

        modo=9;

        while(modo!=2 && modo !=1) {
            printf("Escolha o modo para tratar os aminoacidos: \n \t 1-Modo album de figuras; \n \t 2-Modo Transcricao;");
            scanf("%d", &modo);
            printf("%d", modo);

            if (modo!=2 && modo !=1) {
                printf("digite uma opcao valida");
            }
        }

        FILE *fips;
        if ((fips = fopen("ips_servidores.txt", "r")) == NULL) {
            printf("Error! opening file");
            return 1;
        }

        rewind(copiaProteina);
        if(modo==2) {
            if ((fModoTranscricao = fopen("modoTranscricao.txt", "a")) == NULL) {
                printf("Error! opening file modoTranscricao.txt");
                return 1;
            }
            fgets(fileProximo, 610, copiaProteina);
          //  printf("teste %s", fileProximo);
            posicaoRNA=0;

        }
        int i;
        i=0;


        while((fgets(ipLinha[i], 19, fips) != NULL )&& (i<N_CLIENTES)) {
            pthread_create(&cliente[i], NULL, clienteFuncao, &i);
            sleep(1);
            i++;
        }

        for(i=0; i<N_CLIENTES; i++) {
            pthread_join(cliente[i],&thread_result_cliente);
        }
        fclose(fmodoAlbum);
        fclose(fModoTranscricao);
        return 0;
    }
