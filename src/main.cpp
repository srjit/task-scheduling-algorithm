#include <iostream>

using namespace std;


void print_matrix(int **matrix, int rows, int cols){

  for(int i=0; i<rows; i++){
    for(int j=0; j<cols; j++){
      cout<< matrix[i][j] << "\t";
    }
    cout<<"\n";
  }
  
}

void example_graph(int **graph, int jobCount){

  for(int i=0;i<jobCount; i++){
    graph[i] = new int[jobCount];
  }

  graph[0][1] = graph[0][2] = graph[0][3] = graph[0][4]
              = graph[0][5] = graph[1][7] = graph[1][8]
              = graph[2][6] = graph[3][7] = graph[3][8]
              = graph[4][8] = graph[5][7] = graph[6][9]
              = graph[7][9] = graph[8][9] = 1;
}

int main(){


  int jobCount = 10;

  int **graph = new int*[jobCount];

  example_graph(graph, jobCount);

  print_matrix(graph, 10, 10);

  
}
