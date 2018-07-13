
typedef struct node {
    char  aminoacido;
    struct node* next;
} Node;

Node* nodoAmino;

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
    node->aminoacido = amino;
    node->next = list->head;
    list->head = node;
    list->size++;
}

void mostrarLista(List* list) {
    Node* pointer = list->head;
    if (pointer == NULL) {
        printf("Lista vazia");
    }
    while (pointer != NULL) {
        printf(" %s \n", pointer->aminoacido);
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

char apagarPrimeiroNodo(List* list) {
    if (!isEmpty(list)) {

        Node* pointer = list->head;
		Node *tmp  = pointer;
        list->head = pointer->next;
        free(pointer);
        list->size--;
        return tmp->aminoacido;
    }
}




