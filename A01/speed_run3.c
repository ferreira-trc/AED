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


static bool no_execed_limit_speed (int position, int speed, int final_position)
{  
  int min = max_road_speed[position];

  for (int index = position+1; index <= position + speed && index <= final_position; index++)
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

static bool no_execed_road_length (int position, int speed, int final_position)
{
  return position+speed <= final_position;
}

/*static bool is_viable(int position, int speed)
{
  bool viable = NULL;
  int min = limit_speed(position+speed, speed-2);

  if (speed == 1)
  {
    for (int i = 0; i < 1; i++)
    {
      speed+=i;
      min = limit_speed(position+speed, speed);
      if (min<speed)
      {
      viable = false;
      break;
      }
    }
  } else if (speed == 2)
  {
    for (int i = -1; i < 1; i++)
    {
      speed+=i;
      min = limit_speed(position+speed, speed);
      if (min<speed)
      {
      viable = false;
      break;
      }
    }
  }
  else
  {
    for (int i = -2; i < 1; i++)
    {
      speed+=i;
      min = limit_speed(position+speed, speed);
      if (min<speed)
      {
        viable = false;
        break;
      }
    }
  }
  return viable;
}*/

static bool time_to_slow_down (int position, int speed, int final_position)
{ 
  return speed*(speed + 1)/2 <= final_position - position; // duvidas 1+ final_position - position
}

static int option (int position, int speed, int final_position)
{
  int c;
  
  if (no_execed_limit_speed(position,speed+1,final_position) && no_execed_road_length(position,speed+1,final_position) && position + speed+1 < final_position)
  {    
    c = 1;
  }
  else if (no_execed_limit_speed(position,speed,final_position) && no_execed_road_length(position,speed,final_position))
  {
    c = 0;
  }
  else if (no_execed_limit_speed(position,speed-1,final_position))
  {
    c = -1;
  }
  else
  {
    c = -2; // -2 sera voltar a tras
  }

  return c;
}

static void update_data(solution_t s,int move_number, int position, int speed, int new_speed)
{
  position+=new_speed;
  speed=new_speed;
  s.n_moves=move_number++;
  s.positions[move_number]=position;
}

static void go_back(solution_t s,int move_number, int position, int speed, int new_speed)
{
  s.positions[move_number]=0;
  move_number--;
  s.n_moves=move_number;
  position=s.positions[move_number];
  speed=s.positions[move_number]-s.positions[move_number-1];
}

/*static void solution_2(int move_number,int position,int speed,int final_position)
{
  //int i;
  int new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if(position == final_position && speed == 1)  
  {
    //printf("admisivel\n");
    // is it a better solution?
    if(move_number < solution_1_best.n_moves)
    {
      //printf("best\n");
      solution_1_best = solution_1;
      solution_1_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  if (speed + 1 < limit_speed(position, speed + 1) && time_to_slow_down(position, speed + 1) == false) // acelarar
  {
    new_speed = speed + 1;
    solution_1_recursion(move_number + 1,position + new_speed,new_speed,final_position);
  }
  else if (speed == limit_speed(position, speed) && time_to_slow_down(position, speed) == false) // manter
  {
    new_speed = speed;
    solution_1_recursion(move_number + 1,position + new_speed,new_speed,final_position);        
  }
  else if (speed - 1 == limit_speed(position, speed - 1) || time_to_slow_down(position, speed - 1) == true) // travar
  {
    new_speed = speed - 1;       
    solution_1_recursion(move_number + 1,position + new_speed,new_speed,final_position); 
  }
}*/

/*static void solve_2(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_2(0,0,0,final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}*/

solution_t solution_3_data;

static void solution_3(int move_number,int position,int speed,int final_position)
{ 
  solution_1_count=0;
  solution_3_data.n_moves=move_number;
  solution_3_data.positions[0]=position;

  while (position < final_position)
  { 
    //printf("%ld\n",solution_1_count);
    //printf("Nº de movimentos: %d posição: %d veloc: %d final: %d\n",move_number, position, speed, final_position);
    
    int new_speed;

    if (time_to_slow_down(position,speed, final_position))
      {
        //printf("time_to_slow_down = true\n");
        //printf("posição: %d veloc: %d\n",position, speed);
      }
      else
      {
        //printf("time_to_slow_down = false\n");
        //printf("posição: %d veloc: %d\n",position, speed);
      }
    
    if (time_to_slow_down(position,speed, final_position))
    { 
              
      //printf("Tempo de abrandar ->");   
      if (no_execed_limit_speed(position,speed-1,final_position) && speed*(speed + 1)/2 > final_position - position)
      {
        //printf("nao ultrupassa o limite -> seguir em frente\n");
        new_speed=speed-1;        
        position+=new_speed;
        speed=new_speed;
        solution_3_data.n_moves=++move_number;
        solution_3_data.positions[move_number]=position;
      }
      else if (no_execed_limit_speed(position,speed,final_position) && speed*(speed + 1)/2 == final_position - position)
      {
        new_speed=speed;        
        position+=new_speed;
        speed=new_speed;
        solution_3_data.n_moves=++move_number;
        solution_3_data.positions[move_number]=position;
      }      
      else
      {
        //printf("ultrupassa o limite -> Nao é possivel seguir em frente -> voltar atras\n");
        solution_3_data.positions[move_number]=0;
        move_number--;
        solution_3_data.n_moves=move_number;
        position=solution_3_data.positions[move_number];
        speed=solution_3_data.positions[move_number]-solution_3_data.positions[move_number-1];
        // mudificar a velocidade
        //printf("Temos de corrigir isto\n");
      }
      
    }
    else
    {
      //printf("Nao e Tempo de abrandar ->");
      switch (option(position,speed,final_position))
      {
      case 1:
        //printf("seguir em frente -> acelarar\n");
        new_speed=speed+1;        
        position+=new_speed;
        speed=new_speed;
        //printf("%d\n", solution_3_data.n_moves);
        //printf("%d\n", move_number);
        solution_3_data.n_moves=++move_number;
        //printf("%d\n", move_number);
        //printf("%d\n", solution_3_data.n_moves);        
        solution_3_data.positions[move_number]=position;        
        break;
      case 0:
        //printf("seguir em frente -> manter\n");
        new_speed=speed;        
        position+=new_speed;
        speed=new_speed;
        //printf("%d\n", solution_3_data.n_moves);
        solution_3_data.n_moves=++move_number;
        //printf("%d\n", solution_3_data.n_moves);
        solution_3_data.positions[move_number]=position;
        //printf("Nº de movimentos: %d posição: %d veloc: %d final: %d array: %d\n",move_number, position, speed, final_position, solution_3_data.positions[move_number]);
        break;
      case -1:
        //printf("seguir em frente -> travar\n");
        new_speed=speed-1;        
        position+=new_speed;
        speed=new_speed;
        solution_3_data.n_moves=++move_number;
        solution_3_data.positions[move_number]=position;
        
        break;
      
      default:
        //printf("Nao é possivel seguir em frente -> voltar atras\n");        
        solution_3_data.positions[move_number]=0;
        move_number--;
        solution_3_data.n_moves=move_number;
        position=solution_3_data.positions[move_number];
        speed=solution_3_data.positions[move_number]-solution_3_data.positions[move_number-1];
        // mudificar a velocidade
        //printf("Temos de corrigir isto\n");
        break;
      }   
      
    }   
    solution_1_count++;
    //printf("\n");
    //printf("Nº de movimentos: %d posição: %d veloc: %d final: %d array: %d\n",move_number, position, speed, final_position, solution_3_data.positions[move_number]);
    //printf("%d\n", solution_3_data.n_moves);
    //printf("---------------------------fim de ciclo---------------------------------------\n\n");
  }
  //printf("Nº de movimentos: %d posição: %d veloc: %d final: %d\n",move_number, position, speed, final_position);
    
}

static void solve_3(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_3(0,0,0,final_position);
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
  scanf("%d", &final_position);
  solve_3(final_position);
  make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_3_data.n_moves;i++)
    printf(" %d",solution_3_data.positions[i]);
  printf("\n");
  printf("moves: %d", solution_3_data.n_moves);
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
    //for (int i =2; i < 85; i++)
    //{
      /* code */
      example();
    //}
    
       
    
    return 0;
  }
  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  // run all solution methods for all interesting sizes of the problem
  final_position = 2;
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
      solve_3(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_1.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_3_data.n_moves,&solution_3_data.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_3_data.n_moves,solution_1_count,solution_1_elapsed_time);
    }
    else
    {
      solution_3_data.n_moves = -1;
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
