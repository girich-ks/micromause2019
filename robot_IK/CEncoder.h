#pragma once

#include "Arduino.h"

enum EEncoderDirection {
  EED_NONE, 
  EED_FORWARD, 
  EED_BACKWARD
};

class CEncoder{
public:

  long count1;
  long count2;
  float speed_l;
  float speed_r;

  String left_or_right;
  
  CEncoder(){}
  CEncoder(String str, int left_pin, int right_pin, float round_length, int point_count);
  
  void loop();
  void reset_values();
  float get_distance_from_pin1();
  float get_distance_from_pin2();
  
 private:
  int _pin1;
  int _pin2;
  float step_length;
  int last_state1;
  int last_state2;
  long last_time;
  
    // Измеряемое энкодером направление вращения вала мотора
  EEncoderDirection rotate_direction;

  String get_direction();
};
