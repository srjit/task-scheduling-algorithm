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

  for(int i=0; i<core_count; i++){
    task_mintime_cloud[i] = c_task_attributes.t_send
      + c_task_attributes.t_c_exec + c_task_attributes.t_recv;
  }

  /**
   *  Assign where this task has to be executed - local/cloud
   */
  char task_type[job_count];
  for(int i=0; i<job_count; i++){
    task_type[i] = task_mintime_local[i] < task_mintime_cloud[i] ? 'l' : 'c';
  }

  std::cout<<"\n";
  std::cout<<"Primary assignment to either local/cloud done...";

  for(int i=0; i< job_count; i++){
    std::cout<<task_type[i]<<"\n";
  }

  
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
  // task_prioritizing();
  
}
