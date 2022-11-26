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

static solution_t solution_1;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

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

static bool is_viable (int position, int speed, int final_position)
{ 
  int braking_dist = speed*(speed + 1)/2;
  int space_to_brake = final_position - position;
  return braking_dist <= space_to_brake; // duvidas 1+ final_position - position
}

static bool no_execed_road_length (int position, int speed, int final_position)
{
  return position+speed <= final_position;
}


static void solution_3(int move_number,int position,int speed,int final_position)
{
  int new_speed;  
  
  // restrição de algumas possibilidades 

  while (position == final_position && speed == 1)
  { 
    
    if (speed*(speed + 1)/2 < final_position - position)
    {
        for(new_speed = speed+1;new_speed >= speed-1;new_speed--)
        {
            if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
            {
               if(no_execed_limit_speed(position,new_speed,final_position))
                {              
                    position+=new_speed;
                    speed=new_speed;
                    move_number+=1;
                    solution_1.positions[move_number] = position;
                    break; 
                } 
            }  
        }
        
        // a m t  
    }

    if (speed*(speed + 1)/2 == final_position - position)
    {
        for(new_speed = speed+1;new_speed >= speed;new_speed--)
        {
            if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
            {
               if(no_execed_limit_speed(position,new_speed,final_position))
                {              
                    position+=new_speed;
                    speed=new_speed;
                    move_number+=1;
                    solution_1.positions[move_number] = position;
                    break; 
                } 
            }
        }// m t    
    }

    if (speed*(speed + 1)/2 > final_position - position)
    {   
        new_speed = speed - 1;
        if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
        {
           if(no_execed_limit_speed(position,new_speed,final_position))
            {              
                position+=new_speed;
                speed=new_speed;
                move_number+=1;
                solution_1.positions[move_number] = position;
                     
            } 
        }        
        // t   
            
    }
    solution_1_count++;
  }   
    
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
  solution_1.n_moves = final_position + 100;  
  solution_3(0,0,0,final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}


//
// example of the slides
//

static void example(void)
{
  int i,final_position;
  const char* name = "example";
  const char* ext = ".pdf";
  char fps[4];
  char name_pdf[30];

  scanf("%d", &final_position);
  sprintf(fps, "%3d", final_position);
  strcat(strcpy(name_pdf,name),fps);
  strcat(name_pdf,ext);

  srandom(0xAED2022);
  init_road_speeds();  
  solve_3(final_position);  
  make_custom_pdf_file(name_pdf,final_position,&max_road_speed[0],solution_1.n_moves,&solution_1.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_1.n_moves;i++)
    printf(" %d",solution_1.positions[i]);
  printf("\n");
  printf("moves: %d", solution_1.n_moves);
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
      solve_3(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_1.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_1.n_moves,&solution_1.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
      }
      printf(" %3d %16lu %9.3e |",solution_1.n_moves,solution_1_count,solution_1_elapsed_time);
    }
    else
    {
      solution_1.n_moves = -1;
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