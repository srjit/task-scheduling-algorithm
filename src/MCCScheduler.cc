#include <vector>
#include <numeric>

#include "Task.cc"


using namespace std;

void primary_assignment(std::vector<Task>& tasks,
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
  int task_mintime_cloud[job_count];

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
    tasks[i].set_type(task_type[i]);

    int cost = 0;

    if (task_type[i] == 'l'){
    
      //    std::array<int, 3> tmp =
      int total_local_cost = accumulate(begin(core_table[i]),
					end(core_table[i]),
					0,
					plus<int>());

      cost = total_local_cost / core_count;
    } else if (task_type[i] == 'c'){
      cost = task_mintime_cloud[i];
    }

  tasks[i].set_cost(cost);

  }

  std::cout<<"\n";
  std::cout<<"Primary assignment to either local/cloud done...\n";

  // for(int i=0; i< job_count; i++){
  //   std::cout<<task_type[i]<<"\n";
  // }
  
}


std::vector<Task> construct_tasks(int **graph,
				  int job_count,
				  std::vector<int> exit_tasks)
{

  /**
   *  Construct the task graph
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

    for(int k=0; k<=exit_tasks.size(); k++){
      tasks[exit_tasks[k]].set_is_exit(true);
    }
	
  return tasks;
}


void task_prioritizing(std::vector<Task> tasks)
{

  /**
   *  priority(Vi)   = Wi + Max priority of a successor
   *  Ref: Equation 15, page 195
   * 
   */
  for (std::vector<Task>::reverse_iterator i = tasks.rbegin(); 
        i != tasks.rend(); ++i ) {
    
  } 
  
}

void initial_scheduling(int **graph,
			std::array<std::array<int,3>, 10> core_table,
			int job_count,
			int core_count,
	CloudTask c_task_attributes)
{

  std::vector<int> exit_task_ids = {9};
  std::vector<Task> tasks = construct_tasks(graph,
					    job_count,
					    exit_task_ids);

  
  primary_assignment(tasks,
		     core_table,
		     job_count,
		     core_count,
		     c_task_attributes);

  //  task_prioritizing();
  
}
