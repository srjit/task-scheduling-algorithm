#include <vector>
#include <array>

#include "Utils.cc"
#include "WirelessChannel.cc"

using namespace std;


void stop_execution(Task *task,
		    std::vector<Task*> &running_queue,
		    int finish_tick)
{

  task->set_is_running(false);
  task->set_is_finished(true);
  task->set_finish_time(finish_tick);

  std::cout<<"Finished execution of task "<<task->get_id()<<
  " and setting "<<task->get_cpu()->get_type()<<" CPU with ID "<<
  task->get_cpu()->get_id()<< " to available\n";
      
  task->get_cpu()->set_available(true);
  running_queue.erase(std::remove(running_queue.begin(),
  				  running_queue.end(), task),
  		      running_queue.end());

}


void remove_finished_tasks(std::vector<Task*> &running_queue,
			   int finish_tick)
{

  std::cout<<"\nElements in running queue: "<<running_queue.size()<<"\n";
  for(int i=0; i<running_queue.size(); i++){
    
    Task* task = running_queue[i];

    if(task->get_type() == 'c'){

      // cloud task stopping
      if(task->get_progress() >= task->get_ticks_to_finish_for_cloud_children()){
	task->set_can_cloud_children_start(true);
      }

      vector<Task*> children = task->get_children();
      bool has_only_cloud_children = true;

      for(int k=0;k<children.size();k++){
	if(children[k]->get_type() == 'l'){
	  has_only_cloud_children = false;
	}
      }

      if(has_only_cloud_children){
	stop_execution(task,
		       running_queue,
		       finish_tick);
      } else {
	if(task->get_progress() >= task->get_ticks_to_finish_for_local_children()){
	  stop_execution(task,
			 running_queue,
			 finish_tick);
	}
      }
      
    } else {

      // local task stopping
      if(task->get_progress() >= task->get_ticks_to_finish_for_local_children()){
	/**
	 * Ticks have reached the ticks to finish the task
	 * Remove the task from running queue
	 * Free the CPU
	 */
	stop_execution(task,
		       running_queue,
		       finish_tick);
      }
    }
  }
  
}


void try_unlocking(std::vector<Task*> &tasks_in_pool,
		   std::vector<Task*> &ready_queue)
{

  /**
   *  For every task in pool,
   * Check if all parents have finished execution.
   *
   * If they have, add it to ready queue and remove
   * from tasks_in_pool
   */
  //std::cout<<"\nTrying to unlock tasks in pool...\n";

  vector<Task*> to_remove;
  
  for(int i=0; i<tasks_in_pool.size(); i++){

    Task* task = tasks_in_pool[i];
    std::vector<Task*> parents =  task->get_parents();
    bool can_start = true;

    if(task->get_type() == 'l'){

      for(int j=0; j<parents.size(); j++){

	if(!(parents[j]->get_is_finished())){

	  can_start = false;
	  break;
	}
      }
    }  else {

      if(! task->get_has_finished_transmitting()){
	can_start = false;
      }

      for(int j=0; j<parents.size(); j++){

	if(!parents[j]->get_can_cloud_children_start()){
	  can_start = false;
	  break;
	}

      }
      
    }

    if(can_start){
      std::cout<<"Adding "<<task->get_id()<< " to the ready queue\n";
      task->set_is_unlocked(true);
      ready_queue.push_back(task);
      to_remove.push_back(task);
    }
    
  }

  /**
   * Remove elements which have been added to ready queue
   * from the tasks in pool
   * 
   */
  for(int i=0; i<to_remove.size(); i++){
    Task* tmp = to_remove[i];
    tasks_in_pool.erase(std::remove(tasks_in_pool.begin(),
				    tasks_in_pool.end(), tmp),
			tasks_in_pool.end());
  }
  
}




void start(Task *task,
  	   ExecutionUnit *cpu,
	   std::array<std::array<int,3>, 10> core_table,
	   int tick_id,
	   std::vector<int> allocation)
{
  
  cpu->set_available(false);
  task->set_cpu(cpu);
  task->set_is_running(true);

  int cpu_id = cpu->get_id();
  int task_index = task->get_id() - 1;
  int cpu_index = cpu_id - 1;

  float ticks_to_finish;

  if(cpu_id <= 3){
    // local CPU - pick time from core table
    ticks_to_finish = (float)core_table[task_index][cpu_index];
    std::cout<<"\nTicks to finish for task "<<task->get_id()<<" :"<<ticks_to_finish<<"\n";
  } else{
    // hard coding for now
    ticks_to_finish = 2;
    std::cout<<"\nTicks to finish for cloud task "<<task->get_id()<<" :"
    <<ticks_to_finish<<"\n";
  }

  if(task->get_type() == 'c'){
    task->set_ticks_to_finish_for_local_children(ticks_to_finish);
    task->set_ticks_to_finish_for_cloud_children(ticks_to_finish - 1);
  } else {
    task->set_ticks_to_finish_for_local_children(ticks_to_finish);
    task->set_ticks_to_finish_for_cloud_children(ticks_to_finish);
    
  }
  
  task->set_ready_time(tick_id);
  std::cout<<"\nRunning task "<<task->get_id()<<" on CPU with ID: "<<cpu->get_id()<<"\n";
  
}


void assign(std::vector<Task*> &ready_queue,
	    std::vector<Task*> &running_queue,	    
	    std::vector<ExecutionUnit*> &cpus,
	    std::array<std::array<int,3>, 10> core_table,
	    int tick_id,
	    std::vector<int> allocation)
{

  /**
   * For each task in the ready queue, look
   * for availability of cores / cloud instance.
   *
   * If some processing unit is available assign it to it.
   * set the ticks to finish for the task
   *
   * If not break out, we cannot assign any more tasks
   * anyway.
   */
  std::vector<Task*> to_remove;
  
  for(int i=0; i<ready_queue.size(); i++){

    Task* _task = ready_queue[i];
    ExecutionUnit* cpu = get_free_cpu(cpus,
				      _task,
				      ready_queue,
				      allocation);

    if(cpu != NULL){

      //      std::cout<<"\nAssigning "<<_task->get_id()<<" CPU"<<cpu->get_id();
      start(_task, cpu, core_table, tick_id, allocation);
      /**
       * Remove the task from ready queue and assign it to running queue
       */
      running_queue.push_back(_task);
      to_remove.push_back(_task);
    }else{
      std::cout<<"No free execution units available. Scheduler will wait until the next tick!\n";
      break;
    }
    
  }

  for(int k=0; k<to_remove.size(); k++){
    Task* _tmp = to_remove[k];
    ready_queue.erase(std::remove(ready_queue.begin(),
				  ready_queue.end(), _tmp),
		      ready_queue.end());
  }
  
}


void run(std::vector<Task*> &running_queue)
{

  /**
   * Increment the tick for each task.
   * If the task progress  = ticks to finish
   * set its execution unit availablity to true.
   *
   */
  for(int i=0; i<running_queue.size(); i++){
    running_queue[i]->increment_progress();
    std::cout<<"\nTask "<<running_queue[i]->get_id()<<" is "
    <<running_queue[i]->get_progress_percentage()<<"% complete.\n";        
  } 
  
}

void remove_transmitted_tasks(std::vector<Task*> &tasks_in_pool,
			      WirelessChannel* channel){

  for(int i=0; i<tasks_in_pool.size(); i++){
      
    if(tasks_in_pool[i]->get_has_blocked_channel() &&
       (tasks_in_pool[i]->get_wireless_ticks_current() ==
	tasks_in_pool[i]->get_wireless_ticks_total())){
	
      channel->set_is_occupied(false);
      tasks_in_pool[i]->set_has_blocked_channel(false);
      tasks_in_pool[i]->set_has_finished_transmitting(true);
      std::cout<<"\nTask "<<tasks_in_pool[i]->get_id()<<" has finished transmitting";
      break;
    }
  }
  
}

void send_to_cloud(std::vector<Task*> &tasks_in_pool,
		   WirelessChannel* channel){

  if(!channel->get_is_occupied()){

    for(int i=0; i<tasks_in_pool.size(); i++){
      if(tasks_in_pool[i]->get_type() == 'c'){

    	channel->set_is_occupied(true);
    	tasks_in_pool[i]->set_has_blocked_channel(true);
    	break;
	
      }
    }
  }
}


void increment_transmission(std::vector<Task*> &tasks_in_pool){

  for(int i=0; i<tasks_in_pool.size(); i++){

    if(tasks_in_pool[i]->get_has_blocked_channel()){

      tasks_in_pool[i]->increment_wireless_ticks();
      break;
	
    }
  }
}


int run_scheduler(std::vector<Task*> &tasks_in_pool,
		   std::vector<Task*> &ready_queue,
		   std::vector<ExecutionUnit*> &cpus,
		   std::array<std::array<int,3>, 10> core_table,
		   std::vector<int> allocation = {})
{

  std::vector<Task*> running_queue;
  /**
   *  Every time it goes into this loop,
   *  it is one tick (shown in Fig. 3)
   */
  int i=0;
  int tick_id = 0;
  WirelessChannel* channel = new WirelessChannel();
  
  do{
    std::cout<<"\n--------------  Tick: "<<tick_id<< "   ---------------";
    tick_id +=1;
    /**
     * If any job has finished, free the CPU
     * onto_remove[i] which it has been running
     */
    remove_finished_tasks(running_queue, tick_id);
    show_free_units(cpus);
    
    remove_transmitted_tasks(tasks_in_pool,channel);
    send_to_cloud(tasks_in_pool, channel);
    increment_transmission(tasks_in_pool);
    
    try_unlocking(tasks_in_pool, ready_queue);
    
    if (ready_queue.size() > 0){

      assign(ready_queue,
    	     running_queue,
    	     cpus,
    	     core_table,
    	     tick_id,
    	     allocation);
    }

    run(running_queue);
    //    sleep(1);
  }while(tasks_in_pool.size() > 0 || running_queue.size()>0);

  return tick_id;

}
