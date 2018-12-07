#include <iostream>
#include <array>

#include "MCCScheduler.cc"

using namespace std;
int job_count = 10;
int core_count = 3;

void print_matrix(int **matrix, int rows, int cols)
{

  for(int i=0; i<rows; i++){
    for(int j=0; j<cols; j++){
      cout<< matrix[i][j] << "\t";
    }
    cout<<"\n";
  }
  
}


void example_graph(int **graph)
{

  for(int i=0;i<job_count; i++){
    graph[i] = new int[job_count];
  }

  graph[0][1] = graph[0][2] = graph[0][3] = graph[0][4]
              = graph[0][5] = graph[1][7] = graph[1][8]
              = graph[2][6] = graph[3][7] = graph[3][8]
              = graph[4][8] = graph[5][7] = graph[6][9]
              = graph[7][9] = graph[8][9] = 1;
  
}


void example_graph2(int **graph)
{

  for(int i=0;i<job_count; i++){
    graph[i] = new int[job_count];
  }

  graph[0][1] = graph[0][2] = graph[0][3] = graph[0][4]
              = graph[1][5] = graph[2][5] = graph[2][6]
              = graph[2][7] = graph[3][5] = graph[3][6]
              = graph[3][7] = graph[4][8] = graph[5][9]
              = graph[6][9] = graph[7][9] = graph[8][9] = 1;
  
}


std::array<std::array<int,3>, 10> local_execution_core_cycles()
{

  std::array<std::array<int,3>,10> core_table = {{{9,7,5},             
  	       {8,6,5},
  	       {6,5,4},
  	       {7,5,3},
  	       {5,4,2},
  	       {7,6,4},
  	       {8,5,3},
  	       {6,4,2},
  	       {5,3,2},
  	       {7,4,2}
  }};

  return core_table;
  
}


int main()
{

  int job_count = 10;
  int core_count = 3;

  int **graph = new int*[job_count];
  example_graph(graph);

  int **core_table = new int*[job_count];
  std::array<std::array<int,3>, 10> cores_table = local_execution_core_cycles();

  print_matrix(graph, 10, 10);

  std::cout<<"Tasks submitted to the scheduler for optimizing schedule....\n";

  CloudTask c_task;
  c_task.t_send = 3;
  c_task.t_c_exec = 1;
  c_task.t_recv = 1;

  execute(graph,
	  cores_table,
	  job_count,
	  core_count,
	  c_task);
  
}
