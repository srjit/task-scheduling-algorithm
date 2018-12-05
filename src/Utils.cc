#include <vector>
#include <algorithm>
#include <unistd.h>
#include <array>

#include "Task.cc"
#include "RunInfo.cc"

using namespace std;

struct CloudTask{

  int t_send;
  int t_c_exec;
  int t_recv;
  
};


std::vector<ExecutionUnit*> get_execution_units(int core_count){

  std::vector<ExecutionUnit*> cpus;

  /**
   * One unit for every CPU core
   * Order the processor in descending order of power
   * - this might be important
   */
  ExecutionUnit* eu1 = new ExecutionUnit(1, 'l', 1.0);
  cpus.push_back(eu1);

  ExecutionUnit* eu2 = new ExecutionUnit(2, 'l', 2.0);
  cpus.push_back(eu2);

  ExecutionUnit* eu3 = new ExecutionUnit(3, 'l', 4.0);
  cpus.push_back(eu3);
   
  /**
   * 1 Execution Unit for the cloud
   */
  ExecutionUnit* c = new ExecutionUnit(4, 'c', 0.0);
  cpus.push_back(c);

  return cpus;
  
}


bool compare_by_priority(Task* t1, Task* t2)
{

  /**
   *  Comparator for sorting the tasks by priority
   */
  return t1->get_priority() > t2->get_priority();
   
}


void sort_by_priority(std::vector<Task*> &tasks)
{

  /**
   *  Sorting tasks by priority in descending order
   */
  std::sort (tasks.begin(), tasks.end(), compare_by_priority);
  
}


std::vector<Task*> construct_tasks(int **graph,
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
  std::vector<Task*> tasks;

  /**
   * Find the parent(s) of each task
   */
  for(int j=0; j<job_count; j++){
    Task* task = new Task(j+1);

    for(int i=0; i<j; i++){
      if (graph[i][j] == 1){
	task->add_parent(tasks[i]);
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
	tasks[i]->add_child(tasks[j]);
      }
    }
  }

  for(int k=0; k<exit_tasks.size(); k++){
    tasks[exit_tasks[k]]->set_is_exit(true);
  }
	
  return tasks;
  
}


void calculate_and_set_priority(Task *task){

  /**
   *  Recursive implementation to compute the 
   *  initial priority of tasks
   */
  if(task->get_is_exit()){
    task->set_priority(task->get_cost());
  } else{

    std::vector<Task*> children = task->get_children();
    std::vector<float> child_priorities;

    /*
     * Get the calculated total priority of children
     */
    for(int j=0; j<children.size(); j++){
      calculate_and_set_priority(children[j]);
      child_priorities.push_back((children[j])->get_priority());
    }

    float max_child_priority =
      *std::max_element(std::begin(child_priorities),
			std::end(child_priorities));

    float cost = task->get_cost();
    float priority = cost + max_child_priority;
    task->set_priority(priority);
  }

}


ExecutionUnit* get_free_cpu_random(std::vector<ExecutionUnit*> &cpus){

  /**
   *  Priority of CPUS
   *  If most powerful local core (CPU 3) is available return it
   *  Else try cloud cpu (CPU 4)
   *  Else try local cpu (CPU 2)
   *  Else try local cpu (CPU 1)
   *
   */
  ExecutionUnit* free_unit = NULL;
  
  if(cpus[cpus.size()-1]->get_available()){
    free_unit = cpus[cpus.size()-1];
  } else if (cpus[cpus.size()-2]->get_available()){
    free_unit = cpus[cpus.size()-2];
  } else if (cpus[cpus.size()-3]->get_available()){
    free_unit = cpus[cpus.size()-3];
  } else if (cpus[cpus.size()-4]->get_available()){
    free_unit = cpus[cpus.size()-4];
  }

  return free_unit;
  
}




ExecutionUnit* get_preallocated_cpu(std::vector<ExecutionUnit*> &cpus,
				    Task* task,
				    std::vector<int> allocation)
{				    

  ExecutionUnit* free_unit = NULL;

  int task_id = task->get_id();
  int index = task_id - 1;

  int cpu_id = allocation[index];
  int cpu_id_index = cpu_id -1;

  if(cpus[cpu_id_index]->get_available()){
    free_unit = cpus[cpu_id_index];
  }

  return free_unit;
}


ExecutionUnit* get_free_cpu(std::vector<ExecutionUnit*> &cpus,
			    Task* task,
			    std::vector<Task*> &ready_queue,
			    std::vector<int> allocation){

  if (task->get_type() == 'c'){

    ExecutionUnit* cloud_cpu = cpus[cpus.size() - 1];
    if (cloud_cpu->get_available()){
      return cloud_cpu;
    }
    return NULL;
    
  } else {

    ExecutionUnit* free_unit = NULL;


    if(allocation.empty()){

      if(ready_queue.size() == 1 && cpus[cpus.size()-2]->get_available()){
	free_unit = cpus[cpus.size()-2];
      } else {
  	free_unit = get_free_cpu_random(cpus);
      }

    } else {
      
      free_unit = get_preallocated_cpu(cpus, task, allocation);
    }

    return free_unit;
    
  }
  return NULL;
  
}


void print_ready_tasks(std::vector<Task> &ready_queue){

  // std::cout<<"Tasks in ready queue: ";
  // for(int i=0; i<ready_queue.size(); i++){
  //   std::cout<<ready_queue[i].get_id()<<"\t";
  // }
  // std::cout<<"\n";
  
}


void show_free_units(std::vector<ExecutionUnit*> &cpus){

  // std::cout<<"Free CPUs: ";
  // for(int k=0; k<cpus.size(); k++){
  //   if(cpus[k]->get_available()){
  //     std::cout<<cpus[k]->get_id()<<"\t";
  //   }
  // }
  // std::cout<<"\n";
  
}


void compute_ready_times(std::vector<Task*> &tasks){

  
  for(int i=0; i<tasks.size(); i++){

    std::vector<float> finish_times;
    std::vector<Task*> parents = tasks[i]->get_parents();
    
    for(int j=0; j<parents.size(); j++){
      float finish_time = parents[j]->get_finish_time();
      finish_times.push_back(finish_time);
    }

    float max_finish_time = 0;
    if(finish_times.size() > 0){
      max_finish_time = *std::max_element(std::begin(finish_times),
					  std::end(finish_times));
    }
    
    tasks[i]->set_ready_time(max_finish_time);
  }
  
}




bool compare_power_difference(RunInfo r1, RunInfo r2)
{

  /**
   *  Comparator for sorting the tasks by priority
   */
  return r1.get_energy_reduction() > r2.get_energy_reduction();
   
}

bool compare_with_power_time_ratio(RunInfo r1, RunInfo r2)
{
  return r1.get_energyinc_timeinc_ratio() > r2.get_energyinc_timeinc_ratio();
}


RunInfo* find_optimal_from_power_and_time(std::vector<RunInfo> run_informations)
{

  /**
   *  Sorting runs by minimal power consumed
   */
  RunInfo *optimal = NULL;

  for(int i=0; i<run_informations.size(); i++){

    RunInfo tmp = run_informations[i];

    if((tmp.get_energy_reduction() >0) && (tmp.get_time_difference() > 0)){
      optimal = &tmp;
      break;
    }
    
  }

  if(optimal == NULL){

    std::vector<RunInfo> lesser_energy_runs;
    for(int i=0; i<run_informations.size(); i++){
      if(run_informations[i].get_energy_reduction() > 0){
	lesser_energy_runs.push_back(run_informations[i]);
      }
    }


  std::sort(lesser_energy_runs.begin(),
  	    lesser_energy_runs.end(),
  	    compare_with_power_time_ratio);


  optimal = &lesser_energy_runs[0];
  }


  return optimal;
  
}



RunInfo* find_optimal_run(std::vector<RunInfo> run_informations,
			  float baseline_power_consumed,
			  float baseline_finish_time){

  RunInfo* optimal_run = find_optimal_from_power_and_time(run_informations);

  std::cout<<"\n%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
  std::cout<<optimal_run->get_time_difference()<<"\n";
  std::cout<<optimal_run->get_time_taken();
  //  int foo[10] = optimal_run->get_cpus();
    std::cout<<"\n%%%%%%%%%%%%%%%%%%%%%%%%%%\n";  
  
  return optimal_run;
  
}
