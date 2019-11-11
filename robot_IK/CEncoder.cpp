#include "CEncoder.h"


CEncoder::CEncoder(String _name, int pin1, int pin2, float round_length, int point_count){
// left_or_right = _name;
// reset_values();
// get_direction();
// 
// return;
  this->_pin1 = pin1;
  this->_pin2 = pin2;
  this->step_length = round_length / point_count;
  left_or_right = _name;
  last_time = 0;
  reset_values();
   pinMode(pin1, INPUT);
   pinMode(pin2, INPUT);
  last_state1 = digitalRead(pin1);
  last_state2 = digitalRead(pin2);

//  Serial.print(left_or_right);
//  Serial.print("\t FIRST state is ");
//  Serial.print(last_state1);
//  Serial.print("\t&\t SECOND state is ");
//  Serial.println(last_state2);
//  get_direction();
}

// увеличивает счетчик переключений энкодера и рассчитывает скорость
void CEncoder::loop(){
  int current_state1 = digitalRead(_pin1);
  int current_state2 = digitalRead(_pin2);

  if (current_state1 != last_state1){
    count1++;
//    Serial.print(left_or_right);
//    Serial.print("FIRST is changed to ");
//    Serial.print(current_state1);
    
    rotate_direction = ((last_state1 != last_state2) ? EED_FORWARD : EED_BACKWARD);

//    Serial.print(" --> ");
//    get_direction();
    
    last_state1 = current_state1;  
  }
  
  if (current_state2 != last_state2){
    count2++;
//    Serial.print("\t\t\t\t\t");
//    Serial.print(left_or_right);
//    Serial.print("\t\t\t\t\t SECOND is changed to ");
//    Serial.print(current_state2);

    rotate_direction = (last_state1 == last_state2 ? EED_FORWARD : EED_BACKWARD);
//    rotate_direction = EED_BACKWARD;
//    Serial.print(" --> ");
//    get_direction();
        
    last_state2 = current_state2;  
  }
  
  if (micros() - last_time > 10000){
    speed_l = get_distance_from_pin1()/0.01;
    speed_r = get_distance_from_pin2()/0.01;
//    Serial.print(speed_l);
//    Serial.print("\tright dist is ");
//    Serial.println(speed_r);
    last_time = micros();
  }
}
  
float CEncoder::get_distance_from_pin1(){
  return ((float)count1) * 7.1e-4;
}

float CEncoder::get_distance_from_pin2(){
//  return count_right*0.079*1.2;  
  return ((float)count2) * 7.1e-4;  
}

void CEncoder::reset_values(){
  count1 = 0;
  count2 = 0;
  speed_l = 0;
  speed_r = 0;
  rotate_direction = EED_NONE;
}

String  CEncoder::get_direction()
{
  Serial.print(left_or_right);
  Serial.print("\t");
  if (rotate_direction == EED_FORWARD)
    Serial.println("FORWARD");
  else if (rotate_direction == EED_BACKWARD)
    Serial.println("BACKWARD");
  else if (rotate_direction == EED_NONE)
    Serial.println("NONE");
  else
    Serial.println("dir ERROR");
}
