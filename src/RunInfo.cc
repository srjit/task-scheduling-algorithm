#include <vector>

class RunInfo{

  std::vector<int> assignment;
  float power_consumption;
  float time_taken;
  
  float energy_reduction;
  float time_difference;
  float energyinc_timeinc_ratio;

public:

  RunInfo(std::vector<int> assignment){
    this->assignment = assignment;
  }

  std::vector<int> get_assignment(){
    return this->assignment;
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

  void calculate_energy_reduction(float baseline_power){
    this->energy_reduction = baseline_power - this->get_power_consumption();
  }

  float get_energy_reduction(){
    return this->energy_reduction;
  }

  void calculate_time_difference(float baseline_time){
    this->time_difference = baseline_time - this->get_time_taken();
  }

  float get_time_difference(){
    return this->time_difference;
  }

  void calculate_energyinc_timeinc_ratio(){
    this->energyinc_timeinc_ratio =
      this->get_energy_reduction() / this->get_time_difference();
  }

  float get_energyinc_timeinc_ratio(){
    return this->energyinc_timeinc_ratio;
  }
  
};
