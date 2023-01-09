#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
// static configuration
//

#define _max_word_size_  32
#define _inicial_size_ 10


//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s  adjacency_node_t;
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s      hash_table_t;

struct adjacency_node_s
{
  adjacency_node_t *next;            // link to th enext adjacency list node
  hash_table_node_t *vertex;         // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  //char word[_max_word_size_];        // the word
  int value;
  hash_table_node_t *next;           // next hash table linked list node
  // the vertex data
  adjacency_node_t *head;            // head of the linked list of adjancency edges
  int visited;                       // visited status (while not in use, keep it at 0)
  hash_table_node_t *previous;       // breadth-first search parent
  // the union find data
  hash_table_node_t *representative; // the representative of the connected component this vertex belongs to
  int number_of_vertices;            // number of vertices of the conected component (only correct for the representative of each connected component)
  int number_of_edges;               // number of edges of the conected component (only correct for the representative of each connected component)
};

struct hash_table_s
{
  unsigned int hash_table_size;      // the size of the hash table array
  unsigned int number_of_entries;    // the number of entries in the hash table
  unsigned int number_of_edges;      // number of edges (for information purposes only)
  hash_table_node_t **heads;         // the heads of the linked lists
};


//
// allocation and deallocation of linked list nodes (done)
//

static adjacency_node_t *allocate_adjacency_node(void)
{
  adjacency_node_t *node;

  node = (adjacency_node_t *)malloc(sizeof(adjacency_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_adjacency_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_adjacency_node(adjacency_node_t *node)
{
  free(node);
}

static hash_table_node_t *allocate_hash_table_node(void)
{
  hash_table_node_t *node;

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_hash_table_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}


//
// hash table stuff (mostly to be done)
//

/*unsigned int crc32(const char *str)
{
  static unsigned int table[256];
  unsigned int crc;

  if(table[1] == 0u) // do we need to initialize the table[] array?
  {
    unsigned int i,j;

    for(i = 0u;i < 256u;i++)
      for(table[i] = i,j = 0u;j < 8u;j++)
        if(table[i] & 1u)
          table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
        else
          table[i] >>= 1;
  }
  crc = 0xAED02022u; // initial value (chosen arbitrarily)
  while(*str != '\0')
    crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
  return crc;
}*/

unsigned int hash_function (int value, unsigned int size)
{
  return value % size;
}

static hash_table_t *hash_table_create(unsigned int size)
{
  
  hash_table_t *hash_table;
  unsigned int i;

  hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));
  if(hash_table == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }

  hash_table->hash_table_size=size;
  hash_table->heads = (hash_table_node_t**)malloc(size *sizeof(hash_table_node_t*));
  hash_table->number_of_edges=0;
  hash_table->number_of_entries=0;

  for ( i = 0; i < size; i++)
  {
    hash_table->heads[i] = NULL;
  }
  

  return hash_table;
}

void add_node(hash_table_node_t** head, int value)
{
  hash_table_node_t* new_node = malloc(sizeof(hash_table_node_t));
  new_node->value=value;
  new_node->head=NULL;
  new_node->next=NULL;
  new_node->number_of_edges=0;
  new_node->number_of_vertices=0;
  new_node->representative=NULL;
  new_node->visited=0;

  if (*head == NULL)
  {
    new_node->previous=NULL;
    *head=new_node;  
     
  }
  else
  {
    hash_table_node_t* current_node = *head;

    while (current_node->next != NULL)
    {
      current_node=current_node->next;
    }
    
    current_node->next = new_node;
  }  
}

void insert_into_table(hash_table_t *hash_table, int value, unsigned int index)
{  
  hash_table_node_t** heads = hash_table->heads;  

  if (heads[index] == NULL)
  {
    heads[index] = allocate_hash_table_node();
    hash_table_node_t* head = NULL;
    add_node(&head, value);
    heads[index]= head;
    printf("entrada: %d\n",head->value); 
  }
  else
  {
    hash_table_node_t* head = heads[index];
    add_node(&head, value);
    heads[index] = head;
    printf("entrada: %d\n",head->value); 
  }
  
}

static void hash_table_grow(hash_table_t *hash_table)
{
  hash_table_t* new_hash_table = hash_table_create(hash_table->hash_table_size*10);

  for (unsigned int i = 0; i < hash_table->hash_table_size; i++)
  {
    hash_table_node_t* current_node = hash_table->heads[i];

    if (current_node != NULL)
    {
      printf("t");
    }
    
  }
  


}

static void hash_table_free(hash_table_t *hash_table)
{
  //
  // complete this
  //
  free(hash_table);
}

/*static hash_table_node_t *find_word(hash_table_t *hash_table,const char *word,int insert_if_not_found)
{
  hash_table_node_t *node;
  unsigned int i;

  i = hash_function(word,hash_table->hash_table_size);

  if (insert_if_not_found)
  {
    printf("%d\n",i);
    insert_into_table(hash_table,word,i);
  }
  else
  {
    hash_table_node_t *current_node = hash_table->heads[i];

    while (strcmp(word,current_node->word))
    {
      current_node=current_node->next;
    }  
  }
  
  
  return node;
}*/



void print_table(hash_table_t * hash_table)
{
	for (int i = 0; i < hash_table->hash_table_size; i++)
    {
        hash_table_node_t * head = hash_table->heads[i];
        printf("%d: ",i);

        if (head == NULL)
        {
          printf("NULL");
        }
        else
        {
          hash_table_node_t* current = head;

          while (current->next != NULL)
          {
            printf("%d ->",&current->value); 
            current->next;
          }
        } 
        printf("\n");        
    }    
}



int main(void)
{
 
  
  hash_table_t *hash_table = hash_table_create(_inicial_size_);
  
  for (int i = 10; i < 40; i++)
  {
    int idx = hash_function(i,hash_table->hash_table_size);
    printf("idx: %d value: %d\n", idx,i);
    insert_into_table(hash_table,i,idx);
  }  

  print_table(hash_table);
  return 0;
}
