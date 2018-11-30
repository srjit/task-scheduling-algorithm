#include <vector>
#include <numeric>
#include <algorithm>

#include "Task.cc"

using namespace std;

void primary_assignment(std::vector<Task> &tasks,
			std::array<std::array<int,3>, 10> core_table,
			int job_count,
			int core_count,
			CloudTask c_task_attributes)
{

  /**
   * This is where we primarily decide if a task has to be executed
   * on the cloud or on one of the local cores.
   */
  int task_mintime_local[job_count];
  float task_mintime_cloud[job_count];

  for(int i=0; i<job_count; i++){
    task_mintime_local[i] = core_table[i][2];
  }

  std::cout<<"Local time on the fastest core calculated.\
 Calculating time on the cloud";

  int cloud_task_time = c_task_attributes.t_send +
    c_task_attributes.t_c_exec +
    c_task_attributes.t_recv;

  /**
   *  Assign where this task has to be executed - local/cloud
   */
  char task_type[job_count];
  for(int i=0; i<job_count; i++){

    task_mintime_cloud[i] = cloud_task_time;
    
    task_type[i] = task_mintime_local[i] <= task_mintime_cloud[i] ? 'l' : 'c';
    float cost = 0;

    if (task_type[i] == 'l'){
      int total_local_cost = accumulate(begin(core_table[i]),
      					end(core_table[i]),
      					0,
      					plus<int>());

      cost = total_local_cost / float(core_count);
    } else if (task_type[i] == 'c'){
      cost = task_mintime_cloud[i];
    }

     tasks[i].set_cost(cost);
  }

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
    Task task(j);
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

  
void task_prioritizing(std::vector<Task> &tasks)
{

  /**
   *  priority(Vi)   = Wi + Max priority of a successor
   *  Ref: Equation 15, page 195
   * 
   */

  calculate_and_set_priority(tasks[0]);
  std::cout<<"\nInitial priorities of tasks have been computed...";
  
}


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


void execution_unit_selection(std::vector<Task> &tasks)
{

  sort_by_priority(tasks);
}


void initial_scheduling(int **graph,
			std::array<std::array<int,3>, 10> core_table,
			int job_count,
			int core_count,
	CloudTask c_task_attributes)
{

  /**
   *  Core of the initial component of  the MCC Scheduler
   *
   */
  std::vector<int> exit_task_ids;
  exit_task_ids.push_back(9);

  std::vector<Task> tasks = construct_tasks(graph,
					    job_count,
					    exit_task_ids);
  primary_assignment(tasks,
		     core_table,
		     job_count,
		     core_count,
		     c_task_attributes);


  task_prioritizing(tasks);
  execution_unit_selection(tasks);

  
  std::cout<<"\n Current order of execution with priorities  \n";
  for(int i=0; i<10; i++){
    std::cout<<"\n"<<i<<"\t"<<tasks[i].get_priority();
  }
  std::cout<<"\n";
  
}
