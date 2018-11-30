

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
    std::cout<<"\n"<<i<<"\t"<<tasks[i].get_priority();
  }
  std::cout<<"\n";





 


