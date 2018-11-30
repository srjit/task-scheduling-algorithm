#include <vector>
#include <algorithm>

#include "Task.cc"

using namespace std;

struct CloudTask{

  int t_send;
  int t_c_exec;
  int t_recv;
  
};


bool compare_by_priority(Task t1, Task t2)
{

 /**
  *  Comparator for sorting the tasks by priority
  */
   return t1.get_priority() > t2.get_priority();
   
}


void sort_by_priority(std::vector<Task> &tasks)
{

 /**
  *  Sorting tasks by priority in descending order
  */
  std::sort (tasks.begin(), tasks.end(), compare_by_priority);
  
}
