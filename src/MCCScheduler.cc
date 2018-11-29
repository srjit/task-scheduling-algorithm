#include <vector>

#include "Task.cc"


using namespace std;

void primary_assignment(std::array<std::array<int,3>, 10> core_table,
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

  for(int i=0; i<job_count; i++){
    task_mintime_cloud[i] = cloud_task_time;
  }

  /**
   *  Assign where this task has to be executed - local/cloud
   */
  char task_type[job_count];
  for(int i=0; i<job_count; i++){
    task_type[i] = task_mintime_local[i] <= task_mintime_cloud[i] ? 'l' : 'c';
  }

  std::cout<<"\n";
  std::cout<<"Primary assignment to either local/cloud done...\n";

  // for(int i=0; i< job_count; i++){
  //   std::cout<<task_type[i]<<"\n";
  // }
  
}


std::vector<Task> construct_tasks(int **graph,
					     int job_count)
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

  std::cout<<"\n---------------------\n";

  
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
	
	
  return tasks;
    
}


void task_prioritizing()
{
  
}

void initial_scheduling(int **graph,
			std::array<std::array<int,3>, 10> core_table,
			int job_count,
			int core_count,
			CloudTask c_task_attributes)
{

  primary_assignment(core_table,
		     job_count,
		     core_count,
		     c_task_attributes);

  std::vector<Task> tasks = construct_tasks(graph,
					    job_count);
  //task_prioritizing();
  
}
