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

#define N_CLIENTES 13 //N CLIENTES = N SERVIDORES um cliente pra cada servidor disponivel
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


typedef struct node {
    char  aminoacido;
    struct node* next;
} Node;

typedef struct lista {
    int size;
    Node* head;
} List;
List *listaAmino;

Node* posicao(List* list, int index);


List* iniciarLista() {
    List* list = (List*) malloc(sizeof (List));
    list->size = 0;
    list->head = NULL;
    return list;
}

void inserir(List *list, char amino) {
    Node* node = (Node*) malloc(sizeof (Node));
    //printf("%c ----\n ", amino);
    node->aminoacido = amino;
    node->next = list->head;
    list->head = node;
    list->size++;
}

void mostrarLista(List* list) {
    Node *pointer = list->head;
    if (pointer == NULL) {
      //  printf("Lista vazia");
    }
    while (pointer != NULL) {
        printf(" %c - ", pointer->aminoacido);
        pointer = pointer->next;
    }
}

int indexOf(List* list, Node* node) {
    if (node != NULL) {
        Node* pointer = list->head;
        int index = 0;

        while (pointer != node && pointer != NULL) {
            pointer = pointer->next;
            index++;
        }
        if (pointer != NULL) {
            return index;
        }
    }
    return -1;
}



Node* posicao(List* list, int index) {
    if (index >= 0 && index < list->size) {
        Node* node = list->head;
        int i;
        for (i = 0; i < index; i++) {
            node = node->next;
        }
        return node;
    }
}

bool isEmpty(List* list) {
    return (list->size == 0);
}

Node  *apagarPrimeiroNodo(List* list) {
    if (!isEmpty(list)) {

        Node *pointer = list->head;
        list->head = pointer->next;
        list->size--;
        return pointer;
    }
}



char *procuraAmino(char *proteina, char *amino, char *repl) {
//	printf("\n \n procurando %s", amino);
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
    flagClienteServidorAtivo[iCliente] = 1;
    while(flagClienteServidorAtivo[iCliente]==1) {
        char *rem_hostname;
        int rem_port;
        struct sockaddr_in rem_addr;
        int rem_sockfd;

        /* Construcao da estrutura do endereco local */
        /* Preenchendo a estrutura socket loc_addr (família, IP, porta) */

        rem_hostname = (ipLinha[iCliente]);
       // printf("cliente %d", iCliente);
        //printf("%s ip \n", ipLinha[iCliente]);
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
      //  printf("> Conectando no servidor '%s:%d'\n", rem_hostname, rem_port);

        /* Estabelece uma conexão remota */
        /* parametros(descritor socket, estrutura do endereco remoto, comprimento do endereco) */
        if (connect(rem_sockfd, (struct sockaddr *) &rem_addr, sizeof(rem_addr)) < 0) {
            perror("Conectando stream socket");
            //	exit(1);
        } else {

            //------------------------------------------------------------------------//
            /* Inicializando mensagem */
            aatp_msg m = { 0 };
            /* Preenchendo dados */
            m.method = 'S'; /* Solicitação */
            m.size = 5;
            /* Zerando payload */
            memset(&m.payload, 0, sizeof m.payload);
            /* Enviando solicitação */
            int r = send(rem_sockfd, &m, sizeof(m), 0);
            /* Criando buffer */

            size_t recv_buffer_len = sizeof(recv_buffer);
            /* Recebendo mensagem */
            recv(rem_sockfd, &recv_buffer[iCliente], recv_buffer_len, 0);
            /* Imprimindo resultados */

//printf("\t Method: %c\n", recv_buffer[iCliente].method);
//printf("\t Size: %d\n", recv_buffer[iCliente].size);
//printf("\t Payload: %s\n", recv_buffer[iCliente].payload);

            pthread_mutex_lock(&lock);
            int c ;

            for(c=0; c<recv_buffer[iCliente].size; c++) {
                if (recv_buffer[iCliente].payload[c]) {
                    inserir(listaAmino, (recv_buffer[iCliente].payload[c]));
                }
            }
//printf("mostrando lista \n");
 sleep(1);
            pthread_mutex_unlock(&lock);
            mostrarLista(listaAmino);
            sleep(5);
//----------------------------------------------------------------------------//
            /* fechamento do socket remota */
            close(rem_sockfd);
            //	flagClienteServidorAtivo[iCliente] = 0;
        }
    }
}
void *ribossomoFuncao() {

    while(1) {

        if(!isEmpty(listaAmino)) {

            Node *tmp;
            tmp = apagarPrimeiroNodo(listaAmino);

            char c[700];
            rewind(fProteina);
            while(fgets(c, 700, fProteina) != NULL) {
                char *proteinaMudada;
                proteinaMudada = procuraAmino(c,&tmp->aminoacido,"-");
                
                if (proteinaMudada!="xx"){
				 system("@cls||clear");
                printf("\n %s", proteinaMudada);
              	rewind(fProteina);
                fprintf(fProteina,"%s \n", proteinaMudada);
                sleep(1);
            }

               
            }
        }
        //sleep(5);
    }
}

main() {
    pthread_mutex_init(&lock, NULL);
    listaAmino=iniciarLista();
   // pthread_create(&servidor,NULL, servidorFuncao, NULL);
    pthread_create(&ribossomo, NULL, ribossomoFuncao, NULL);
    sleep(2);

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
    while((fgets(ipLinha[i], 13, fips) != NULL )&& (i<N_CLIENTES)) {
        //		printf("CLIENTE %d", i);
        pthread_create(&cliente[i], NULL, clienteFuncao, &i);
        sleep(1);
        i++;
    }

    //pthread_join(servidor, &thread_result_servidor);
    pthread_join(ribossomo, &thread_result_ribossomo);

    for(i=0; i<N_CLIENTES; i++) {
        pthread_join(cliente[i],&thread_result_cliente);
    }
    fclose(fProteina);

}
