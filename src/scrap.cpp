

/*
Check while constructing graph
 */  

    for(int i=0; i<job_count; i++){
      Task t = tasks[i];
      std::vector<Task*> parents = t.get_parents();
      for(int j=0; j< parents.size(); j++){
      	Task* tp = parents[j];
      	std::cout<<tp->get_id()<<"\t";
      }
      std::cout<<"\n";
    }



/*
Test for children
 */

    for(int i=0; i<job_count; i++){
      Task t = tasks[i];
      std::vector<Task*> children = t.get_children();
      for(int j=0; j< children.size(); j++){
    	std::cout<<children[j]->get_id()<<"\t";
      }
      std::cout<<"\n";
    }
	
	



/*

  Code to see the initial priority values of tasks

 */

  std::cout<<"\n";
  for(int i=0; i<10; i++){
    std::cout<<"\n"<<tasks[i]->get_id()<<"\t"<<tasks[i]->get_priority();
  }
  std::cout<<"\n";




 




  std::cout<<"++++++++++++++++++";
  for(int i=0; i<tasks_in_pool.size(); i++){
    std::cout<<tasks_in_pool[i]->get_id();
  }
  std::cout<<"++++++++++++++++++";


      // tasks_in_pool.erase(std::remove(tasks_in_pool.begin(),
      // 				      tasks_in_pool.end(), task),
      // 			  tasks_in_pool.end());

      ready_queue.erase(std::remove(ready_queue.begin(),
				    ready_queue.end(), _task),
			ready_queue.end());






//  task migration
// see all tasks


  std::cout<<"Execution Units selected: \n";
  for(int i=0; i<tasks.size(); i++){
    //    std::cout<<tasks[i]->get_execution_unit_id()<<"\t";
    std::cout<<tasks[i]->get_id()<<"\t";    
  }
  std::cout<<"\n";


