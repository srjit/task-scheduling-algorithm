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
  }

  void set_progress(float progress){
    this->progress = progress;
  }

  float get_progress(){
    return (this->progress)/(this->ticks_to_finish)*100;
  }


  void set_cpu(ExecutionUnit* cpu){
    this->cpu = cpu;
  }

  ExecutionUnit* get_cpu(){
    return this->cpu;
  }

  /**
   *  Overloading == operator for later use
   */
  bool operator==(const Task& _task){
    return this->get_id() == _task.id;
  }
  

};


