//
// AED, August 2022 (Tomás Oliveira e Silva)
//
// First practical assignement (speed run)
//
// Compile using either
//   cc -Wall -O2 -D_use_zlib_=0 solution_speed_run.c -lm
// or
//   cc -Wall -O2 -D_use_zlib_=1 solution_speed_run.c -lm -lz
//
// Place your student numbers and names here
//   N.Mec. XXXXXX  Name: XXXXXXX
//


//
// static configuration
//

#define _max_road_size_  800  // the maximum problem size
#define _min_road_speed_   2  // must not be smaller than 1, shouldnot be smaller than 2
#define _max_road_speed_   9  // must not be larger than 9 (only because of the PDF figure)


//
// include files --- as this is a small project, we include the PDF generation code directly from make_custom_pdf.c
//

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "../P02/elapsed_time.h"
#include "make_custom_pdf.c"


//
// road stuff
//

static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

static void init_road_speeds(void)
{
  double speed;
  int i;

  for(i = 0;i <= _max_road_size_;i++)
  {
    speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
    max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
    if(max_road_speed[i] < _min_road_speed_)
      max_road_speed[i] = _min_road_speed_;
    if(max_road_speed[i] > _max_road_speed_)
      max_road_speed[i] = _max_road_speed_;
  }
}


//
// description of a solution
//

typedef struct
{
  int n_moves;                         // the number of moves (the number of positions is one more than the number of moves)
  int positions[1 + _max_road_size_];  // the positions (the first one must be zero)
}
solution_t;

typedef struct node
{
  int position;
  int speed;
  int level;
  bool visited;
  node_t* father;
  node_t* left;
  node_t* midel;
  node_t* right;
  
}
node_t;

static node_t new_node(int position, int speed, int level, bool visited, node_t* father)
{
  node_t* n;
  n->position=position;
  n->speed=speed;
  n->level=level;
  n->visited=visited;
  n->father=father;
  n->left=NULL;
  n->midel=NULL;
  n->right=NULL;
  return n;
}

static bool no_execed_limit_speed (int position, int speed)
{  
  int min = max_road_speed[position];

  for (int index = position+1; index <= position + speed; index++)
  {
    if (max_road_speed[index] == _min_road_speed_)
    {
      min = _min_road_speed_;
      break;
    }
    
    if (min > max_road_speed[index])
    {
      min = max_road_speed[index];
    }    
  }
  return speed <= min; 
}

static void new_ramification(node_t children[],node_t *n)
{    
  int new_speed;
  int index=0;
  bool cut = n->prev.speed != 1 || n->prev.position == 1; 

  if (n->speed==1 && n->position !=1)
  {
    if (no_execed_limit_speed(n->position,n->speed) && cut)    
    {              
      children[index]=new_node(n->position+1,1,n->level+1,false,n);                
    }
  }
  else
  {
    for(new_speed = n->speed - 1;new_speed <= n->speed + 1;new_speed++)
    {
      if (new_speed >= 1 && new_speed <= _max_road_speed_)
      {          
        if (no_execed_limit_speed(n->position,new_speed))
        {                                
          children[index]=new_node(n->position+new_speed,new_speed,n->level+1,false,n);                
        }
      }      
      index++;
    }
  }           
}

typedef struct
{
  node_t *top, *bottom;
  unsigned int length;
} 
Stack;

Stack new_stack()
{
  Stack stack;
  stack.top=NULL;
  stack.bottom=NULL;
  stack.length=0;
  return stack;
}

// inserção no início da stack
void push(Stack *stack, node_t n)
{
  node_t *new = (node_t*)malloc(sizeof(node_t)); // cria um novo nó
  new->n = n;// (*novo).valor = valor

  if(stack->top == NULL) { // a stack está vazia
    new->next = NULL;
    stack->top = new;
    stack->bottom = new;
  } else { // a stack não está vazia
    new->next = stack->top;
    stack->top = new;
  }
  stack->length++;
}

// remover um elemento da stack
void pop(Stack *stack) 
{
  
  node_t * noARemover = NULL; // ponteiro para o nó a ser removido

  // remover 1º elemento
  noARemover = stack->top;
  stack->top = noARemover->next;

  if(stack->top == NULL)
    stack->bottom = NULL;   
  
  if(noARemover) {
      free(noARemover); // libera a memória do nó
      stack->length--; // decrementa o tamanho da stack
  }
}

//
// the (very inefficient) recursive solution given to the students
//

static solution_t solution_1,solution_1_best;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

static void solution_1_recursion(int move_number,int position,int speed,int final_position)
{
  int i,new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if(position == final_position && speed == 1)
  {
    // is it a better solution?
    if(move_number < solution_1_best.n_moves)
    {
      solution_1_best = solution_1;
      solution_1_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  for(new_speed = speed - 1;new_speed <= speed + 1;new_speed++)
    if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for(i = 0;i <= new_speed && new_speed <= max_road_speed[position + i];i++);
      if(i > new_speed)
        solution_1_recursion(move_number + 1,position + new_speed,new_speed,final_position);
    }
}

static void solve_1(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_1_recursion(0,0,0,final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}

static solution_t solution_2;
static double solution_2_elapsed_time; // time it took to solve the problem
static unsigned long solution_2_count; // effort dispended solving the problem


static void solution_2(int move_number,int position,int speed,int final_position)
{
  Stack stack = new_stack();
  node_t roat = new_node(0,0,0,true,NULL);
  node_t* children = calloc(3,sizeof(node_t));
  push(&stack,roat);

  while (position < final_position)
  {
    bool cond = stack.top->father->speed==1
    if ()
    {
      /* code */
    }
    
    new_ramification(children,stack.top);
    
    for (int i = 2; i => 0; i--)
    {
      if (children[i].position!=0)
      {
        push(stack,children[i])
      }      
    }
         
  }
  



}
//
// example of the slides
//

static void example(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();    
  
  final_position = 30;
  solve_1(final_position);
  make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_1_best.n_moves;i++)
    printf(" %d",solution_1_best.positions[i]);
  printf("\n");
}


//
// main program
//

int main(int argc,char *argv[argc + 1])
{
# define _time_limit_  3600.0
  int n_mec,final_position,print_this_one;
  char file_name[64];

  

  // generate the example data
  if(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x')
  {
    example();
    return 0;
  }
  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  // run all solution methods for all interesting sizes of the problem
  final_position = 1;
  solution_1_elapsed_time = 0.0;
  printf("    + --- ---------------- --------- +\n");
  printf("    |                plain recursion |\n");
  printf("--- + --- ---------------- --------- +\n");
  printf("  n | sol            count  cpu time |\n");
  printf("--- + --- ---------------- --------- +\n");
  while(final_position <= _max_road_size_/* && final_position <= 20*/)
  {
    print_this_one = (final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
    printf("%3d |",final_position);
    // first solution method (very bad)
    if(solution_1_elapsed_time < _time_limit_)
    {
      solve_1(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_1.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_1_best.n_moves,solution_1_count,solution_1_elapsed_time);
    }
    else
    {
      solution_1_best.n_moves = -1;
      printf("                                |");
    }
    // second solution method (less bad)
    // ...

    // done
    printf("\n");
    fflush(stdout);
    // new final_position
    if(final_position < 50)
      final_position += 1;
    else if(final_position < 100)
      final_position += 5;
    else if(final_position < 200)
      final_position += 10;
    else
      final_position += 20;
  }
  printf("--- + --- ---------------- --------- +\n");
  return 0;
# undef _time_limit_
}