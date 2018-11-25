#include <iostream>


void example_graph(int **graph, int jobCount){

  for(int i=0;i<jobCount; i++){
    graph[i] = new int[jobCount];
  }

  int a[10] = {1,1,1,0,1,1,1,1,0,1};
  graph[0] = a;
  
   
}

int main(){


  int jobCount = 10;

  int **graph = new int*[jobCount];

  example_graph(graph, jobCount);

  std::cout<<graph[0][8];
  
}
