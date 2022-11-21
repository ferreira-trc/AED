#include <stdio.h>


typedef struct No
{
  int valor;
  struct No *next;
} 
No_t;

typedef struct {
  No_t *head, *tail;
  unsigned int length;
} 
Lista;

Lista inicializar(){
    Lista lista;
    lista.head=NULL;
    lista.tail=NULL;
    lista.length=0;
    return lista;
}

// inserção no início da lista
void push(Lista *lista, int valor) {
  No_t *new = (No_t*)malloc(sizeof(No_t)); // cria um novo nó
  new->valor = valor;// (*novo).valor = valor

  if(lista->head == NULL) { // a lista está vazia
    new->next = NULL;
    lista->head = new;
    lista->tail = new;
  } else { // a lista não está vazia
    new->next = lista->head;
    lista->head = new;
  }
  lista->length++;
}

// remover um elemento da lista
void pop(Lista *lista) {
  No_t *head = lista->head; // ponteiro para o início da lista
  No_t * noARemover = NULL; // ponteiro para o nó a ser removido

  // remover 1º elemento
  noARemover = lista->head;
  lista->head = noARemover->next;

  if(lista->head == NULL)
    lista->tail = NULL;
   
  
  if(noARemover) {
      free(noARemover); // libera a memória do nó
      lista->length--; // decrementa o tamanho da lista
  }
}

// imprimir a lista
void imprimir(Lista *lista) {
    No_t *head = lista->head;
    printf("Tamanho da lista: %d\n", lista->length);
    while(head != NULL) {
        printf("%d ", head->valor);
        head = head->next;
    }
    printf("\n\n");
}



int main(int argc, char const *argv[])
{
    Lista lista = inicializar();

    for (int i = 0; i < 10; i++)
    {
        push(&lista,i);
        
        
    }

    imprimir(&lista);

    pop(&lista);
    pop(&lista);

    imprimir(&lista);

    push(&lista,45);
    push(&lista,50);

    imprimir(&lista);

    return 0;
}
