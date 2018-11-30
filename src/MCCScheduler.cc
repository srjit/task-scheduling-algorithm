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

      // int total_local_cost = core_table[i][0] + core_table[i][1] + core_table[i][2];
      //    std::array<int, 3> tmp =
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

  //   for(int i=0; i<tasks.size(); i++){
  //     std::cout<<"Initial cost of "<<i<< "th task"<< tasks[i].get_cost()<<"\n";
  //   }

  //   std::cout<<"=================\n";

  // std::cout<<"\n";
  // std::cout<<"Primary assignment to either local/cloud done...\n";

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

    for(int k=0; k<exit_tasks.size(); k++){
      tasks[exit_tasks[k]].set_is_exit(true);
    }
	
  return tasks;
}




void calculate_and_set_priority(Task &task){

  if(task.get_is_exit()){
    task.set_priority(task.get_cost());
  } else{

    std::vector<Task*> children = task.get_children();
    std::vector<float> child_priorities;

    // for(int j=0; j<children.size(); j++){
    //   std::cout<<(*children[j]).get_id()<<"\t";
    // }

    /*
      Get the calculated total priority of children
    */

    for(int j=0; j<children.size(); j++){
      calculate_and_set_priority(*children[j]);
      child_priorities.push_back((*children[j]).get_priority());
    }

    // for (std::vector<Task*>::iterator it = children.begin(); 
    // 	 it != children.end(); ++it ) {
    //   //      std::cout<<"\n---->"<<(*it)->get_id();
    //   std::cout<<"task id: "<< (**it).get_id()<<"\n";
    //   calculate_and_set_priority(**it);
    //   std::cout<<"\n--->"<<(**it).get_priority();
    //   //      child_priorities.push_back(it->get_priority());
    // }

    float max_child_priority =
      *std::max_element(std::begin(child_priorities), std::end(child_priorities));

    float cost = task.get_cost();
    float priority = cost + max_child_priority;

    // std::cout<<"\n----->>"<<cost;

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

  // for(int i=0; i<tasks.size(); i++){
  //   std::cout<<"Priority of task "<<i<<" = "<<tasks[i].get_priority()<<"\n";
  // }

  std::cout<<"\n";
  for(int i=0; i<10; i++){
    std::cout<<"\n"<<i<<"\t"<<tasks[i].get_priority();
  }
  std::cout<<"\n";
  // for (std::vector<Task>::iterator i = tasks.begin(); 
  //       i != tasks.end(); ++i ) {

  //   std::cout<<(*i).get_id()<<"\n";
    
  // } 
  
}

void initial_scheduling(int **graph,
			std::array<std::array<int,3>, 10> core_table,
			int job_count,
			int core_count,
	CloudTask c_task_attributes)
{

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


    for(int i=0; i<tasks.size(); i++){
      std::cout<<"\nInitial cost of "<<i<< "th task"<< tasks[i].get_cost();
    }
  
  task_prioritizing(tasks);
  
}
