//
// AED, November 2022 (Tomás Oliveira e Silva)
//
// Second practical assignement (speed run)
//
// Place your student numbers and names here
//   N.Mec. XXXXXX  Name: XXXXXXX
//
// Do as much as you can
//   1) MANDATORY: complete the hash table code
//      *) hash_table_create
//      *) hash_table_grow
//      *) hash_table_free
//      *) find_word
//      +) add code to get some statistical data about the hash table
//   2) HIGHLY RECOMMENDED: build the graph (including union-find data) -- use the similar_words function...
//      *) find_representative
//      *) add_edge
//   3) RECOMMENDED: implement breadth-first search in the graph
//      *) breadh_first_search
//   4) RECOMMENDED: list all words belonginh to a connected component
//      *) breadh_first_search
//      *) list_connected_component
//   5) RECOMMENDED: find the shortest path between to words
//      *) breadh_first_search
//      *) path_finder
//      *) test the smallest path from bem to mal
//         [ 0] bem
//         [ 1] tem
//         [ 2] teu
//         [ 3] meu
//         [ 4] mau
//         [ 5] mal
//      *) find other interesting word ladders
//   6) OPTIONAL: compute the diameter of a connected component and list the longest word chain
//      *) breadh_first_search
//      *) connected_component_diameter
//   7) OPTIONAL: print some statistics about the graph
//      *) graph_info
//   8) OPTIONAL: test for memory leaks
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//
// static configuration
//

#define _max_word_size_  32
#define _inicial_size_ 1000


//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s  adjacency_node_t;
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s      hash_table_t;

struct adjacency_node_s
{
  adjacency_node_t *next;            // link to the next adjacency list node
  hash_table_node_t *vertex;         // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  char word[_max_word_size_];        // the word
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
  node->head=NULL;
  node->next=NULL;
  node->number_of_edges=0;
  node->number_of_vertices=0;
  node->representative=node;
  node->visited=0;
  node->previous=NULL;
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}


//
// hash table stuff (mostly to be done)
//

unsigned int crc32(const char *str)
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
}

unsigned int hash_function (const char *str, unsigned int size)
{
  return crc32(str) % size;
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
  hash_table->heads = (hash_table_node_t**)malloc(hash_table->hash_table_size *sizeof(hash_table_node_t*));
  hash_table->number_of_edges=0;
  hash_table->number_of_entries=0;

  for ( i = 0; i < size; i++) 
  {
    hash_table->heads[i] = NULL;
  }

  return hash_table;
}

void add_node(hash_table_node_t** head, const char *word)
{
  hash_table_node_t* new_node = allocate_hash_table_node();
  strcpy(new_node->word,word);
  

  if (*head == NULL)
  {    
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
  //printf("-> ");  
}

void insert_into_table(hash_table_t *hash_table, const char *word, unsigned int index)
{  
    
  hash_table_node_t** heads = hash_table->heads;  

  if (heads[index] == NULL)
  {    
    hash_table_node_t* head = NULL;
    add_node(&head, word);
    heads[index]= head;
  }
  else
  {
    hash_table_node_t* head = heads[index];
    add_node(&head, word);
    heads[index] = head;
  }
  hash_table->number_of_entries+=1;
  //printf("-> ");
}

static void hash_table_grow(hash_table_t *hash_table)
{
  hash_table_node_t **old_heads = hash_table->heads;
  unsigned int old_size = hash_table->hash_table_size;
  unsigned int new_size = old_size*10;

  hash_table->hash_table_size = new_size;
  hash_table->heads = (hash_table_node_t**) malloc(new_size * sizeof(hash_table_node_t*));

  for (unsigned int i = 0; i < new_size; i++)
  {
    hash_table->heads[i] = NULL;
  }

  for (unsigned int i = 0; i < old_size; i++)
  {
    hash_table_node_t *current = old_heads[i];
    while (current != NULL)
    {
      hash_table_node_t *next = current->next;
      unsigned int index = hash_function(current->word,new_size);
      current->next = hash_table->heads[index];
      hash_table->heads[index] = current;
      current = next;
    }
  }
  free(old_heads);
}

static void hash_table_free(hash_table_t *hash_table)
{
  for (unsigned int i = 0; i < hash_table->hash_table_size; i++)
  {
    hash_table_node_t* head = hash_table->heads[i];
    
    if (head != NULL)
    {
      hash_table_node_t *current_node = head;     

      while (current_node != NULL)
      {
        adjacency_node_t *current_node_adjs = head->head;


        while (current_node_adjs != NULL)
        {
          adjacency_node_t * aux = current_node_adjs;
          current_node_adjs = current_node_adjs->next;
          free(aux);
        }
        hash_table_node_t *aux = current_node;
        current_node = current_node->next;
        free(aux); 
      }           
    }    
  }

  free(hash_table->heads);
  free(hash_table);
}

static hash_table_node_t *find_word(hash_table_t *hash_table,const char *word,int insert_if_not_found)
{
  hash_table_node_t *node;
  unsigned int i;  

  if (hash_table->number_of_entries> 5* hash_table->hash_table_size)
  {
    hash_table_grow(hash_table);
  }  
  
  i = hash_function(word,hash_table->hash_table_size);

  if (insert_if_not_found)
  {
    //printf("%s %d",word,i);
    insert_into_table(hash_table,word,i);
    //printf(" entrie: %d\n",hash_table->number_of_entries);
    node = NULL;
  }
  else
  {
    //printf("%s\n",word);
    //printf("entrou no find_word\n%d\n",i);
    node = hash_table->heads[i];
    printf("%s\n", node->word);

    while (node->next != NULL && strcmp(node->word, word) != 0)
    {
      //printf("while\n");
      node=node->next;
    }  
  } 

  //printf("%s\n", node->word);

  return node;
}



static int average(int n[], int length)
{
  int sum = 0;

  for (int i = 0; i < length; i++)
  {
    sum+=n[i];
  }  

  return sum/length;
}

static double standard_deviation(int n[], int length, int average)
{
  int sum = 0;

  for (int i = 0; i < length; i++)
  {
    sum+= pow(n[i]-average,2);
  }

  return sqrt(sum/length);
}

int max_value(int n[], int length)
{   
  int max_value = 0;

  for (int i = 1; i < length; i++)
  {
    if (n[i]>max_value)
    {
      max_value=n[i];
    }
  }
  return max_value;
}

static int mode(int n[], int length)
{
  int appearances_length = max_value(n,length);
  int appearances[appearances_length];

  for (int i = 0; i < appearances_length; i++)
  {
    appearances[i]=0;
  }  

  for (int i = 0; i < length; i++)
  {
    appearances[n[i]]+=1;
  }

  return max_value(appearances,appearances_length);
  
}

static void statistics_hash_table(hash_table_t *hash_table)
{
  //  code to show hash table chaining
  /*
  index - n of chains
      0 - x
      1 - x
      2 - x
  */
  int number_nodes[hash_table->hash_table_size];  

  printf("Index   -   n chains\n");
  hash_table_node_t** current_node = hash_table->heads; 

  for (unsigned int i = 0; i < hash_table->hash_table_size; i++)
  {
    int n = 0;
    printf("%4d   -", i);    

    if (current_node[i] == NULL)
    {
      printf("NULL\n");
      number_nodes[i] = 0;
    } 
    else 
    {
      while (current_node[i] != NULL)
      {
        n++;
        current_node[i] = current_node[i]->next;
      }

      printf("%4d\n", n);
    }

    number_nodes[i] = n;
        
  }
  int x_bar = average(number_nodes, hash_table->hash_table_size); 
  double s_dev = standard_deviation(number_nodes, hash_table->hash_table_size,average);
  int max = max_value(number_nodes, hash_table->hash_table_size);
  int mod = mode(number_nodes, hash_table->hash_table_size);
}

static void print_table(hash_table_t *hash_table)
{
  hash_table_node_t** current_node = hash_table->heads; 

  for (unsigned int i = 0; i < hash_table->hash_table_size; i++)
  {
    
    printf("%8d   -", i);    

    if (current_node[i] == NULL)
    {
      printf(" NULL\n");
    } 
    else 
    {
      hash_table_node_t* itr = current_node[i];

      while (itr != NULL)
      { 
        printf("%s -> ",itr->word);       
        itr = itr->next;        
      }
      printf("\n");
    }        
  }
}


//
// add edges to the word ladder graph (mostly do be done)
//

static hash_table_node_t *find_representative(hash_table_node_t *node)
{
  hash_table_node_t *representative,*next_node;
  int number_of_vertices, number_of_edges;               

  
  representative = node;
  
  while (representative->representative != representative) {    
    next_node = representative->representative;   
    representative->representative = next_node->representative;    
    representative = next_node;
    next_node->number_of_edges;
  }  
  
  return representative;
}

static void add_edge(hash_table_t *hash_table,hash_table_node_t *from,const char *word)
{
  if (hash_table->heads[hash_function(word,hash_table->hash_table_size)] != NULL)
  {
    //printf("add_edge -> %s\n", from->word);
    hash_table_node_t *to,*from_representative,*to_representative;
    adjacency_node_t *link;
    //printf("antes find_word\n");
    to = find_word(hash_table,word,0); // O problema esta aqui
    //printf("depois find_word\n");
    printf("%s\n", to->word); // o erro aparece aqui

    if (to != NULL)
    {
      link = allocate_adjacency_node();
      //printf("Aqui\n");
      //printf("%s\n", to->word);
      link->vertex=to;
      link->next=NULL;  

      // add the edge from->to
      
      if (from->head == NULL)
      {
        from->head=link;
      }
      else
      {
        adjacency_node_t *current_node = from->head;

        while (current_node != NULL && current_node->vertex != to)
        {
          current_node = current_node->next;
        }
        // assigment if not there 
        if (current_node->vertex != to)
        {
          current_node->next=link;
        }
      }
      
      hash_table->number_of_edges+=1;

      from_representative = find_representative(from);
      to_representative = find_representative(to);

      // union 
      if (from_representative != to_representative)
      {
        from->representative=to_representative;
      }
    } 
  } 
}


//
// generates a list of similar words and calls the function add_edge for each one (done)
//
// man utf8 for details on the uft8 encoding
//

static void break_utf8_string(const char *word,int *individual_characters)
{
  int byte0,byte1;

  while(*word != '\0')
  {
    byte0 = (int)(*(word++)) & 0xFF;
    if(byte0 < 0x80)
      *(individual_characters++) = byte0; // plain ASCII character
    else
    {
      byte1 = (int)(*(word++)) & 0xFF;
      if((byte0 & 0b11100000) != 0b11000000 || (byte1 & 0b11000000) != 0b10000000)
      {
        fprintf(stderr,"break_utf8_string: unexpected UFT-8 character\n");
        exit(1);
      }
      *(individual_characters++) = ((byte0 & 0b00011111) << 6) | (byte1 & 0b00111111); // utf8 -> unicode
    }
  }
  *individual_characters = 0; // mark the end!
}

static void make_utf8_string(const int *individual_characters,char word[_max_word_size_])
{
  int code;

  while(*individual_characters != 0)
  {
    code = *(individual_characters++);
    if(code < 0x80)
      *(word++) = (char)code;
    else if(code < (1 << 11))
    { // unicode -> utf8
      *(word++) = 0b11000000 | (code >> 6);
      *(word++) = 0b10000000 | (code & 0b00111111);
    }
    else
    {
      fprintf(stderr,"make_utf8_string: unexpected UFT-8 character\n");
      exit(1);
    }
  }
  *word = '\0';  // mark the end
}

static void similar_words(hash_table_t *hash_table,hash_table_node_t *from)
{
  static const int valid_characters[] =
  { // unicode!
    0x2D,                                                                       // -
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,           // A B C D E F G H I J K L M
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,           // N O P Q R S T U V W X Y Z
    0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,           // a b c d e f g h i j k l m
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,           // n o p q r s t u v w x y z
    0xC1,0xC2,0xC9,0xCD,0xD3,0xDA,                                              // Á Â É Í Ó Ú
    0xE0,0xE1,0xE2,0xE3,0xE7,0xE8,0xE9,0xEA,0xED,0xEE,0xF3,0xF4,0xF5,0xFA,0xFC, // à á â ã ç è é ê í î ó ô õ ú ü
    0
  };
  int i,j,k,individual_characters[_max_word_size_];
  char new_word[2 * _max_word_size_];

  break_utf8_string(from->word,individual_characters);
  for(i = 0;individual_characters[i] != 0;i++)
  {
    k = individual_characters[i];
    for(j = 0;valid_characters[j] != 0;j++)
    {
      individual_characters[i] = valid_characters[j];
      make_utf8_string(individual_characters,new_word);
      // avoid duplicate cases
      if(strcmp(new_word,from->word) > 0)
      {
        printf("new_word -> %s\n", new_word);
        printf("similar_word -> %s\n", from->word);
        add_edge(hash_table,from,new_word);
      }
        
    }
    individual_characters[i] = k;
  }
}


//
// breadth-first search (to be done)
//
// returns the number of vertices visited; if the last one is goal, following the previous links gives the shortest path between goal and origin
//

static int breadh_first_search(int maximum_number_of_vertices,hash_table_node_t **list_of_vertices,hash_table_node_t *origin,hash_table_node_t *goal)
{
  //
  // complete this
  //
  return -1;
}


//
// list all vertices belonging to a connected component (complete this)
//

static void list_connected_component(hash_table_t *hash_table,const char *word)
{
  //
  // complete this
  //
}


//
// compute the diameter of a connected component (optional)
//

static int largest_diameter;
static hash_table_node_t **largest_diameter_example;

static int connected_component_diameter(hash_table_node_t *node)
{
  int diameter;

  //
  // complete this
  //
  return diameter;
}


//
// find the shortest path from a given word to another given word (to be done)
//

static void path_finder(hash_table_t *hash_table,const char *from_word,const char *to_word)
{
  //
  // complete this
  //
}


//
// some graph information (optional)
//

static void graph_info(hash_table_t *hash_table)
{
  //
  // complete this
  //
  //statistics_hash_table(hash_table);
}


//
// main program
//

int main(int argc,char **argv)
{
  char word[100],from[100],to[100];
  hash_table_t *hash_table;
  hash_table_node_t *node;
  unsigned int i;
  int command;
  FILE *fp;

  // initialize hash table
  hash_table = hash_table_create(_inicial_size_);
  // read words
  fp = fopen((argc < 2) ? "wordlist-big-latest.txt" : argv[1],"rb");
  if(fp == NULL)
  {
    fprintf(stderr,"main: unable to open the words file\n");
    exit(1);
  }
  while(fscanf(fp,"%99s",word) == 1)
    (void)find_word(hash_table,word,1);
  fclose(fp);
  //statistics_hash_table(hash_table);
  
  print_table(hash_table);
  //printf("%s\n",hash_table->heads[999999]->word);
  // find all similar words
  //hash_table_node_t *nod = find_word(hash_table,"hebraizar-lhe-ias",0);
  //printf("%s\n", nod->word);
  for(i = 0u;i < hash_table->hash_table_size;i++)
  {    
    for(node = hash_table->heads[i];node != NULL;node = node->next)
    {
      //printf("%d %s size: %d\n",hash_function(node->word,hash_table->hash_table_size), node->word, hash_table->hash_table_size);
      //printf("idx: %d size: %d crc32: %d\n", crc32(node->word)%hash_table->hash_table_size, hash_table->hash_table_size, crc32(node->word));
      //char n = scanf(" carrega enter %c");
      similar_words(hash_table,node);
    }      
  }
  //char n = scanf("%c");  
  graph_info(hash_table);
  // ask what to do
  for(;;)
  {
    fprintf(stderr,"Your wish is my command:\n");
    fprintf(stderr,"  1 WORD       (list the connected component WORD belongs to)\n");
    fprintf(stderr,"  2 FROM TO    (list the shortest path from FROM to TO)\n");
    fprintf(stderr,"  3            (terminate)\n");
    fprintf(stderr,"> ");
    if(scanf("%99s",word) != 1)
      break;
    command = atoi(word);
    if(command == 1)
    {
      if(scanf("%99s",word) != 1)
        break;
      list_connected_component(hash_table,word);
    }
    else if(command == 2)
    {
      if(scanf("%99s",from) != 1)
        break;
      if(scanf("%99s",to) != 1)
        break;
      path_finder(hash_table,from,to);
    }
    else if(command == 3)
      break;
  }
  // clean up
  hash_table_free(hash_table);
  return 0;
}
