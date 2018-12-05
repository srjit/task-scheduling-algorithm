#include <vector>
#include <numeric>
#include <algorithm>
#include <queue>

#include "SchedulerUtils.cc"
//#include "RunInfo.cc"

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
      tasks[i]->set_type('l');
      int total_local_cost = accumulate(begin(core_table[i]),
      					end(core_table[i]),
      					0,
      					plus<int>());

      cost = total_local_cost / float(core_count);
    } else if (task_type[i] == 'c'){
      tasks[i]->set_type('c');      
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


std::vector<int> get_baseline_allocation(std::vector<Task*> &tasks,
					 std::array<std::array<int,3>, 10> core_table,
					 int core_count)	      
{

  std::vector<Task*> tasks_in_pool;
  std::vector<int> _primary_allocation;

  for(int j=0; j<tasks.size(); j++){
    int tmp = tasks[j]->get_execution_unit_id();
    _primary_allocation.push_back(tmp);
  }

  return _primary_allocation;
  
}


float total_power_consumed(std::vector<Task*> &tasks)
{

  float power_consumed = 0.0;
  for(int i=0; i<tasks.size(); i++){
    power_consumed += tasks[i]->get_power_consumed();
  }
  return power_consumed;
  
}


float total_time_taken(std::vector<Task*> &tasks)
{

  // this is possible only if we have only one
  // end task
  return tasks[tasks.size()-1]->get_finish_time();
  
}


void reset_tasks(std::vector<Task*> &tasks){

  /**
   * Resetting tasks
   */

  for(int k=0; k< tasks.size();k++){
    tasks[k]->set_is_unlocked(false);
    tasks[k]->set_is_running(false);
    tasks[k]->set_is_finished(false);
    tasks[k]->set_progress(0.0);

    tasks[k]->set_power_consumed(0.0);

    tasks[k]->set_ready_time(0);
    tasks[k]->set_finish_time(0.0);
    tasks[k]->set_ticks_to_finish(0);
      
  }
  
}

vector<RunInfo> optimize_schedule(std::vector<Task*> &tasks,
				  std::vector<int> baseline_allocation,
				  std::array<std::array<int,3>, 10> core_table,
				  int core_count,
				  float baseline_power,
				  float baseline_time)
{

  vector<RunInfo> run_informations;

  /**
   * Outer loop
   */
  for(int i=0; i<tasks.size(); i++){
    vector<int> schedule;
    for(int j=1; j<=core_count+1; j++){

	reset_tasks(tasks);
	std::vector<int> new_allocation(baseline_allocation);

	new_allocation.at(i) = j;
	
	RunInfo run_information(new_allocation);

	std::vector<Task*> tasks_in_pool;
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
		      new_allocation);

	float power_consumed = total_power_consumed(tasks);
	run_information.set_power_consumption(power_consumed);

	// std::cout<<"Power consumed:"<<power_consumed<<"\n";

	float time_taken = total_time_taken(tasks);
	run_information.set_time_taken(time_taken);

	// std::cout<<"Time taken:"<<time_taken<<"\n";

	run_information.calculate_energy_reduction(baseline_power);
	run_information.calculate_time_difference(baseline_time);
	run_information.calculate_energyinc_timeinc_ratio();

	// if (i==0 && j==4){
	//   std::cout<<"\n==============================\n";
	//   std::vector<int> boo = run_information.get_assignment();
	//   for(int k=0; k<boo.size(); k++){
	//     std::cout<<boo[k]<<"\t";
	//   }
	//   std::cout<<"\nPower:"<<run_information.get_power_consumption();
	//   std::cout<<"\n Time:"<<run_information.get_time_taken();
	//   std::cout<<"\n==============================\n"; 	  
	// }
	  
	run_informations.push_back(run_information);
    }
  }

  return run_informations;
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


  

    std::vector<int> schedule_to_optimize =
      get_baseline_allocation(tasks,
			      core_table,
			      core_count);

    float power_consumed = total_power_consumed(tasks);
    float finish_time = tasks[9]->get_finish_time();

    float t_max = 30;
    
    std::cout<<"Finish time: "<<finish_time;
    std::cout<<"t_max: " <<t_max;

    
    while(true){

      vector<RunInfo> run_informations =  optimize_schedule(tasks,
							    schedule_to_optimize,
							    core_table,
							    core_count,
							    power_consumed,
							    finish_time);
  
      RunInfo optimal_run = find_optimal_run(run_informations,
					      power_consumed,
					      finish_time);

      if(t_max < optimal_run.get_time_taken()){

	std::cout<<"The allocation cannot be optimized further under the given parameters...";
	break;
      } else{

	std::cout<<"\n Information about the optimal assignment in this sequence..."<<"\n";
	
	std::vector<int> optimal_assignment = optimal_run.get_assignment();
	
	for(int k=0;k<optimal_assignment.size(); k++){
	  std::cout<<optimal_assignment[k]<<"\t";
	}
	std::cout<<"\n";
	std::cout<<"Power: "<<optimal_run.get_power_consumption()<<"\n";
	std::cout<<"Time for optimal run: "<< optimal_run.get_time_taken()<<"\n";
	

	schedule_to_optimize = optimal_run.get_assignment();
	power_consumed = total_power_consumed(tasks);
	  
      }
    }

    
}
