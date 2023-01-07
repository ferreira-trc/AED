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

typedef struct
{
  int moves;
  int position;
  int speed;
  char op;
} 
moves_t;

moves_t new_move(unsigned int moves,unsigned int position, int speed, int op){
  moves_t move;
  move.moves=moves;
  move.position=position;
  move.speed=speed;
  move.op=op;
  return move;
}

typedef struct No
{
  moves_t data;
  struct No *next;
} 
No_t;

typedef struct
{
  No_t *top, *bottom;
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
void push(Stack *stack, moves_t data) {
  No_t *new = (No_t*)malloc(sizeof(No_t)); // cria um novo nó
  new->data = data;// (*novo).valor = valor

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
void pop(Stack *stack) {
  
  No_t * noARemover = NULL; // ponteiro para o nó a ser removido

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





static bool braking (int position, int speed)
{ 
  return speed*(speed + 1)/2 > 1 + _max_road_size_ - position; 
}

static int limit_speed (int position, int speed)
{
  
  int max = max_road_speed[position];

  for (int index = position; index < speed; index++)
  {
    if (max < max_road_speed[index])
    {
      max = max_road_speed[index];
    }    
  } 

  return max; 
}


//
// the (very inefficient) recursive solution given to the students
//

static solution_t solution_1,solution_1_best;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

static void solution_2(int final_position)
{
  int speed=0, new_speed=0, move_number=0, position = 0;

  // record move
  int ops[3]  = {-1,0,1};
  Stack satck = new_stack();
  moves_t move = new_move(0,0,0,1);
  push(&satck, move);
  

  while (satck.top->data.position < final_position)
  {    
    int index = strlen(ops);
    while (index>=0)
    { 
      new_speed = speed + ops[index];
      if (index =! 0)
      {
        
        if (speed + ops[index] < limit_speed(position, new_speed) && braking(position, new_speed) == false) // acelarar ou manter
        {          
          move = new_move(move_number++,position+new_speed,new_speed,ops[index]);
          push(&satck, move);
          break;
        }
      }

      if (index == 0)
      {
        if (speed + ops[0] == limit_speed(position, new_speed)) // travar
        {
          move = new_move(move_number++,position+new_speed,new_speed,ops[0]);
          push(&satck, move);
          break;
        } else
        {
          int op = satck.top->data.op;
          pop(&satck);  
          index=op;
        }
        
      }
      solution_1_count++;
      index--;
    }
    solution_1_count++;
    
    /*
    if (speed + ops[2] < limit_speed(position, speed + ops[2]) && braking(position, speed + ops[2])) // acelarar
    {
      new_speed = speed + ops[2];
      move = new_move(move_number++,position+new_speed,new_speed,ops[2]);
      push(&satck, move);
    }
    else if (speed + ops[1] == limit_speed(position, speed + ops[1]) && braking(position, speed + ops[1])) // manter
    {
      new_speed = speed + ops[1];
      move = new_move(move_number++,position+new_speed,new_speed,ops[1]);
      push(&satck, move);
    }
    else if (speed + ops[0] == limit_speed(position, speed + ops[0]) || braking(position, speed + ops[0])) // travar
    {
      new_speed = speed + ops[0];
      move = new_move(move_number++,position+new_speed,new_speed,ops[0]);
      push(&satck, move);
    } 
    else // voltar atras 
    {
      int op = satck.top->data.op;
      pop(&satck);  
      op-=1;   
      
        
    }   
    */
    
  }
  
  solution_1.positions[move_number] = satck.top->data.position;
  solution_1.n_moves = satck.top->data.moves;
  solution_1_best = solution_1; 
  
}

static void solve_2(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_2: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_2(final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
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
  solve_2(final_position);
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
  printf("    |                plain     2     |\n");
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
      solve_2(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_1.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"my solution");
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
