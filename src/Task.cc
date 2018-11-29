#include <vector>

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
  bool is_exit = false;
  std::vector<Task*> parents;
  std::vector<Task*> children;
  
  
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
  
};
