
class ExecutionUnit{

  int id;
  char type;
  bool available = true;

public:

  ExecutionUnit(int id, char type){
    this->id = id;
    this->type = type;
  }

  int get_id(){
    return this->id;
  }

  char get_type(){
    return this->type;
  }

  void set_available(bool available){
    this->available = available;
  }

  bool get_available(){
    return this->available;
  }
  
};
