
class ExecutionUnit{

  int id;
  char type;
  float power = 0.0;
  bool available = true;

public:

  ExecutionUnit(int id, char type){
    this->id = id;
    this->type = type;
  }

  ExecutionUnit(int id, char type, float power){
    this->id = id;
    this->type = type;
    this->power = power;
  }
  
  int get_id(){
    return this->id;
  }

  char get_type(){
    return this->type;
  }

  void set_power(float power){
    this->power = power;
  }

  float get_power(){
    return this->power;
  }

  void set_available(bool available){
    this->available = available;
  }

  bool get_available(){
    return this->available;
  }
  
};
