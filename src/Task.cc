#include <vector>

#include "ExecutionUnit.cc"

class Task{

  int id;

  /**
   * If task is a cloud task cost, wi = Ti^re
   * else, if it is local Wi = average computation time
   *                           on the local cores
   * Ref:  Page 195, final para
   */
  float cost;
  char type;
  float priority;
  
  bool is_unlocked = false;
  bool is_running = false;
  bool is_finished = false;
  
  bool is_exit = false;
  std::vector<Task*> parents;
  std::vector<Task*> children;

  ExecutionUnit* cpu;

  int execution_unit_id;
  float finish_time;
  float ready_time = 0;

  int ready1;
  int ready2;

  float power_consumed = 0.0;
  
  /**
   * Increment this unit by 1 tick
   * evey time it is running
   */
  float progress = 0;

  /**
   * The ticks to finish a task depends on the CPU on
   * which is assigned to.
   */
  float ticks_to_finish;
  
public:

  Task(int id){
    this->id = id;
  }

  int get_id(){
    return this->id;
  }

  float get_cost(){
    return this->cost;
  }

  void set_cost(float cost){
    this->cost = cost;
  }
  
  void set_type(char type){
    this->type = type;
  }

  char get_type(){
    return this->type;
  }

  void set_priority(float priority){
    this->priority = priority;
  }

  float get_priority(){
    return this->priority;
  }

  void set_is_unlocked(bool unlocked){
    this->is_unlocked = true;
  }

  bool get_is_unlocked(){
    return this->is_unlocked;
  }

  void set_is_running(bool running){
    this->is_running = running;
  }

  bool get_is_running(){
    return this->is_running;
  }

  void set_is_finished(bool finished){
    this->is_finished = finished;
  }

  bool get_is_finished(){
    return this->is_finished;
  }
  
  void set_is_exit(bool is_exit){
    this->is_exit = is_exit;
  }

  bool get_is_exit(){
    return this->is_exit;
  }

  /*
   * Getters and setters for exposing parents
   */
  std::vector<Task*> get_parents(){
    return this->parents;
  }

  void set_parents(std::vector<Task*> parents){
    this->parents = parents;
  }

  void add_parent(Task* parent){
    this->parents.push_back(parent);
  }

  /*
   * Getters and setters for exposing children
   */
  std::vector<Task*> get_children(){
    return this->children;
  }

  void set_children(std::vector<Task*> children){
    this->children = children;
  }

  void add_child(Task* child){
    this->children.push_back(child);
  }

  void set_ticks_to_finish(float ticks){
    this->ticks_to_finish = ticks;
  }

  int get_ticks_to_finish(){
    return this->ticks_to_finish;
  }

  void increment_progress(){
    this->progress += 1.0;

    if(this->type == 'l'){
      std::cout<<"-------";
      int power_per_tick = this->cpu->get_power();
      this->power_consumed += power_per_tick;
    }
      
  }

  void set_progress(float progress){
    this->progress = progress;
  }

  float get_progress(){
    return this->progress;
  }

  float get_progress_percentage(){
    float progress_percentage = (this->progress)/(this->ticks_to_finish)*100;
    if (progress_percentage > 100){
      progress_percentage = 100;
    }
    return progress_percentage;
  }
  

  void set_cpu(ExecutionUnit* cpu){
    this->cpu = cpu;
    this->execution_unit_id = cpu->get_id();

    if(this->type == 'c'){
      this->power_consumed += cpu->get_power();
    }
  }

  ExecutionUnit* get_cpu(){
    return this->cpu;
  }

  int get_execution_unit_id(){
    return this->execution_unit_id;
  }

  void set_finish_time(float finish_time){
    this->finish_time = finish_time - 1;
  }

  float get_finish_time(){
    return this->finish_time;
  }
  
  void set_ready_time(float ready_time){
    this->ready_time = ready_time;
  }

  float get_ready_time(){
    return this->ready_time;
  }

  void set_power_consumed(float power_consumed){
    this->power_consumed = power_consumed;
  }

  float get_power_consumed(){
    return this->power_consumed;
  }

  /**
   *  Overloading == operator for later use
   */
  bool operator==(const Task& _task){
    return this->get_id() == _task.id;
  }
  

};


