#include <vector>

class RunInfo{

  std::vector<int> assignment;
  float power_consumption;
  float time_taken;

public:

  RunInfo(std::vector<int> assignment){
    this->assignment = assignment;
  }

  void set_power_consumption(float power_consumption){
    this->power_consumption = power_consumption;
  }

  float get_power_consumption(){
    return this->power_consumption;
  }

  void set_time_taken(float time_taken){
    this->time_taken = time_taken;
  }

  int get_time_taken(){
    return this->time_taken;
  }
  
};
