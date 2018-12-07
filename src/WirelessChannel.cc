
class WirelessChannel{


  bool is_occupied = false;


public:

  WirelessChannel(){
  }

  void set_is_occupied(bool occupied){
    this->is_occupied = occupied;
  }

  bool get_is_occupied(){
    return this->is_occupied;
  }
  
};
