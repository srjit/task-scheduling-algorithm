#include <vector>
#include <algorithm>
#include <unistd.h>

#include "Task.cc"

using namespace std;

struct CloudTask{

  int t_send;
  int t_c_exec;
  int t_recv;
  
};


bool compare_by_priority(Task t1, Task t2)
{

  /**
   *  Comparator for sorting the tasks by priority
   */
  return t1.get_priority() > t2.get_priority();
   
}


void sort_by_priority(std::vector<Task> &tasks)
{

  /**
   *  Sorting tasks by priority in descending order
   */
  std::sort (tasks.begin(), tasks.end(), compare_by_priority);
  
}


std::vector<Task> construct_tasks(int **graph,
				  int job_count,
				  std::vector<int> exit_tasks)
{

  /**
   *  Construct the task graph from the adjacency matrix.
   *
   *  Each job becomes an Object of type 'Task'.
   *
   *  It would have pointers to its parent tasks
   *  as well as child tasks for easy traversal.
   *
   */
  std::vector<Task> tasks;

  /**
   * Find the parent(s) of each task
   */
  for(int j=0; j<job_count; j++){
    Task task(j+1);
    for(int i=0; i<j; i++){
      if (graph[i][j] == 1){
	task.add_parent(&tasks[i]);
      }
    }
    tasks.push_back(task);
  }

  /**
   * Enrich the graph with children of each graph
   */
  for(int i=0; i<job_count; i++){
    for(int j=0; j<job_count; j++){
      if(graph[i][j] == 1){
	tasks[i].add_child(&tasks[j]);
      }
    }
  }

  for(int k=0; k<exit_tasks.size(); k++){
    tasks[exit_tasks[k]].set_is_exit(true);
  }
	
  return tasks;
  
}





void calculate_and_set_priority(Task &task){

  /**
   *  Recursive implementation to compute the 
   *  initial priority of tasks
   */

  if(task.get_is_exit()){
    task.set_priority(task.get_cost());
  } else{

    std::vector<Task*> children = task.get_children();
    std::vector<float> child_priorities;

    /*
     * Get the calculated total priority of children
     */
    for(int j=0; j<children.size(); j++){
      calculate_and_set_priority(*children[j]);
      child_priorities.push_back((*children[j]).get_priority());
    }

    float max_child_priority =
      *std::max_element(std::begin(child_priorities), std::end(child_priorities));

    float cost = task.get_cost();
    float priority = cost + max_child_priority;
    task.set_priority(priority);
    
  }

}


void execute(std::vector<Task> &ready_queue){
}


void print_ready_tasks(std::vector<Task> &ready_queue){

  std::cout<<"Tasks in ready queue: ";
  for(int i=0; i<ready_queue.size(); i++){
    std::cout<<ready_queue[i].get_id()<<"\t";
  }
  std::cout<<"\n";
}

void assign_core_or_cloud(std::vector<Task> &tasks_in_pool,
			  std::vector<Task> &ready_queue){

  /**
   *  Every time it goes into this loop,
   *  it is one time step (shown in Fig. 3)
   */
  do{

    std::cout<<"Polling for tasks in the ready queue...\n";
    if (ready_queue.size() > 0){
      print_ready_tasks(ready_queue);
      execute(ready_queue);
      sleep(1);
    }
	
  }while(tasks_in_pool.size() > 0);

}
