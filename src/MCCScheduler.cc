#include <vector>
#include <numeric>
#include <algorithm>
#include <queue>

//#include "Utils.cc"
#include "SchedulerUtils.cc"

using namespace std;

void primary_assignment(std::vector<Task*> &tasks,
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

     tasks[i]->set_cost(cost);
  }

}

  
void task_prioritizing(std::vector<Task*> &tasks)
{

  /**
   *  priority(Vi)   = Wi + Max priority of a successor
   *  Ref: Equation 15, page 195
   * 
   */

  calculate_and_set_priority(tasks[0]);
  std::cout<<"\nInitial priorities of tasks have been computed...";
  
}

void execution_unit_selection(std::vector<Task*> &tasks,
			      std::array<std::array<int,3>, 10> core_table,
			      int core_count)
{

  sort_by_priority(tasks);

  std::vector<ExecutionUnit*> cpus = get_execution_units(core_count);
  std::cout<<"\nThere are "<< cpus.size() <<" execution units available\n";

  
  std::vector<Task*> tasks_in_pool;

  // adding every task from 1 to 9 into pool -
  // task with index 0 is ready
  for(int k=1; k< tasks.size();k++){
    tasks_in_pool.push_back(tasks[k]);
  }
  
  std::vector<Task*> ready_queue;

  /**
   * Add first task to ready queue
   */
  tasks[0]->set_is_unlocked(true);
  ready_queue.push_back(tasks[0]);

  run_scheduler(tasks_in_pool,
  		ready_queue,
  		cpus,
  		core_table);
}


void optimize(std::vector<Task*> &tasks,
	      std::array<std::array<int,3>, 10> core_table,
	      int core_count)	      
{

  std::vector<Task*> tasks_in_pool;

  /**
   * Resetting tasks
   */
  for(int k=0; k< tasks.size();k++){
    tasks[k]->set_is_unlocked(false);
    tasks[k]->set_is_running(false);
    tasks[k]->set_is_finished(false);
    tasks[k]->set_progress(0.0);
  }

  // adding every task from 1 to 9 into pool -
  // task with index 0 is ready
  for(int k=1; k<tasks.size();k++){
    tasks_in_pool.push_back(tasks[k]);    
  }

  std::vector<ExecutionUnit*> cpus = get_execution_units(core_count);

  std::vector<Task*> ready_queue;  
  tasks[0]->set_is_unlocked(true);
  ready_queue.push_back(tasks[0]);

  run_scheduler(tasks_in_pool,
  		ready_queue,
  		cpus,
  		core_table,
		true);
  
  
}


void execute(int **graph,
	     std::array<std::array<int,3>, 10> core_table,
	     int job_count,
	     int core_count,
	     CloudTask c_task_attributes)
{

  /**
   *  Core of the initial component of  the MCC Scheduler
   *
   *  Initializing the components before beginning execution 
   */
  std::vector<int> exit_task_ids;
  exit_task_ids.push_back(9);

  std::vector<Task*> tasks = construct_tasks(graph,
					    job_count,
					    exit_task_ids);

  /*
   *************************************************
   *         Part I:  Initial Scheduling           *
   *                                               *
   *************************************************
   */ 
  primary_assignment(tasks,
  		     core_table,
  		     job_count,
  		     core_count,
  		     c_task_attributes);
  task_prioritizing(tasks);
  execution_unit_selection(tasks,
  			   core_table,
  			   core_count);

  /**
   *************************************************     
   *        Part II: Task Migration                *
   *                                               *
   *************************************************
   */

    compute_prerequisites_for_optimization(tasks);
    optimize(tasks,
	     core_table,
	     core_count);
	     
    
}
