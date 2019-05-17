// Motor.cpp

#include "Arduino.h"
#include "motor.h"
//#include <cmath>

void Motor::stopMotor() { // РѕСЃС‚Р°РЅРѕРІРёС‚СЊ РјРѕС‚РѕСЂ
  if (Motor != MD_STOP) {
    Motor = MD_STOP;
    digitalWrite(Motor5_pin, 0);
    analogWrite(Motor6_pin, 0);
    digitalWrite(Motor7_pin, 0);
    digitalWrite(17, 0);
  }
}
void Motor::goForward() { // Р·Р°РїСѓСЃС‚РёС‚СЊ РјРѕС‚РѕСЂ РІРїРµСЂРµРґ
  if (Motor != MD_FORWARD) {
    Motor = MD_FORWARD;
    digitalWrite(Motor5_pin, 0);
//    analogWrite(Motor6_pin, speed);
    digitalWrite(Motor7_pin, 1);
    digitalWrite(17, 1);
  }
}

void Motor::goBack() { // С„СѓРЅРєС†РёСЏ РґРІРёР¶РµРЅРёСЏ РЅР°Р·Р°Рґ
  if (Motor != MD_BACK) {
    Motor = MD_BACK;    
    digitalWrite(Motor5_pin, 1);
//    analogWrite(Motor6_pin, speed);
    digitalWrite(Motor7_pin, 0);
    
    digitalWrite(17, 1);
  }
}

void Motor::setSpeed(byte aspeed){
  analogWrite(Motor6_pin, aspeed);
  speed = aspeed;  
}

void Motor::getSpeed(){
  return speed;
}


OptoPara::OptoPara(int left_pin, int right_pin, float round_length, int point_count){
  this->left_pin = left_pin;
  this->right_pin = right_pin;
  this->step_length = round_length / point_count;
  last_time = 0;
  speed_l = 0;
  speed_r = 0;
  // pinMode(left_pin, INPUT);
  // pinMode(right_pin, INPUT);
  state_left = digitalRead(left_pin);
  state_right = digitalRead(right_pin);
}

void OptoPara::loop(){
  int cur_left = digitalRead(left_pin);
  int cur_right = digitalRead(right_pin);
  if (cur_left != state_left){
    count_left++;
    state_left = cur_left;  
  }
  if (cur_right != state_right){
    count_right++;
    state_right = cur_right;  
  }
  if (micros() - last_time > 100){
    speed_l = get_distance_left()/0.0001;
    speed_r = get_distance_right()/0.0001;
    last_time = micros();
  }
}
  
float OptoPara::get_distance_left(){
  return count_left*(0.079)*1.2;    
}
float OptoPara::get_distance_right(){
  return count_right*0.079*1.2;  
}

void OptoPara::reset(){
  count_left = 0;
  count_right = 0;
  speed_l = 0;
  speed_r = 0;
}

void MoveModule::loop(){
  switch (status){
    case MM_STOP:{
      if (micros() - last_time < 40000) return;
      last_time = micros();
      double speedl = optoPara->get_distance_left() - last_dist_l;
      double speedr = optoPara->get_distance_right() - last_dist_r;
      last_dist_l = optoPara->get_distance_left();
      last_dist_r = optoPara->get_distance_right();
      // Serial.println(speedl);
      motorLeft->stopMotor();
      motorRight->stopMotor();
      status = MM_NONE;
      ready_flag = true;
      break;
    }
    case MM_MOVE:{
      if (micros() - last_time < 50) return;
      last_time = micros();
      double dist_l = optoPara->get_distance_left() - last_dist_l;
      double dist_r = optoPara->get_distance_right() - last_dist_r;
      if (dist_l >= distance && dist_r >= distance*scale){
        hard_stop();
        return;
      }
      // if (dist_l > scale*dist_r){
      //   motorLeft->setSpeed(max_speed/2);
      //   motorRight->setSpeed(max_speed);
      // }
      // if (dist_l < scale*dist_r){
      //   motorRight->setSpeed(max_speed/2);
      //   motorLeft->setSpeed(max_speed);
      // }
      break;
    }
    case MM_ROTATE:{
      Serial.print("MM_Rotate");
      if (micros() - last_time < 100) return;
      last_time = micros();
      double dist_l = optoPara->get_distance_left();
      double dist_r = optoPara->get_distance_right();
      double speed_l = optoPara->speed_l;
      double speed_r = optoPara->speed_r;

//      Serial.print(dist_l);
//      Serial.print("\t");
//      Serial.print(dist_r);
//      Serial.print("\t");
//      Serial.print(speed_l);
//      Serial.print("\t");
//      Serial.println(speed_r);
      
      if (dist_l >= distance && dist_r >= distance){
        motorLeft->setSpeed(0.0);
        motorRight->setSpeed(0.0);
        status = MM_NONE;
//        if (speed_l <= 0.2 && speed_r <= 0.2){
//          if (dist_l <= 0.2 && dist_r <= 0.2){
//            ready_flag = true;
//            status = MM_NONE;
//            return;
//          }
//          if (stoper_l){
//            motorLeft->goBack();
//          }
//          else{
//            motorLeft->goForward();
//          }
//          if (stoper_r){
//            motorRight->goBack();
//          }
//          else{
//            motorRight->goForward();
//          }
//          stoper_l = !stoper_l;
//          stoper_r = !stoper_r;
//          optoPara->reset();
//          last_time = micros();
//          return;
//        }
        return;
      }
      if (dist_l > dist_r){
        motorLeft->setSpeed(max_speed/2);
        motorRight->setSpeed(max_speed);
      }
      if (dist_l < dist_r){
        motorRight->setSpeed(max_speed/2);
        motorLeft->setSpeed(max_speed);
      }
      break;
    }
    default:
      return;
  }
}

void MoveModule::move(double distance, double ascale){
  motorLeft->setSpeed(max_speed);
  motorRight->setSpeed(max_speed);
  status = MM_MOVE;
  motorLeft->goForward();
  stoper_l = true;
  motorRight->goForward();
  stoper_r = true;
  last_dist_l = optoPara->get_distance_left();
  last_dist_r = optoPara->get_distance_right();
  scale = ascale;
  last_time = micros();
  ready_flag = false;
  this->distance = distance;
}

void MoveModule::move_back(double distance, double ascale){
  motorLeft->setSpeed(max_speed);
  motorRight->setSpeed(max_speed);
  motorLeft->goBack();
  stoper_l = false;
  motorRight->goBack();
  stoper_r = false;
  last_dist_l = optoPara->get_distance_left();
  last_dist_r = optoPara->get_distance_right();
  scale = ascale;
  last_time = micros();
  ready_flag = false;
  this->distance = distance;
}

void MoveModule::hard_stop(){
  last_dist_l = optoPara->get_distance_left();
  last_dist_r = optoPara->get_distance_right();
  // last
  ready_flag = false;
  status = MM_STOP;
  if (stoper_l){
    motorLeft->goBack();
  }
  else{
    motorLeft->goForward();
  }
  if (stoper_r){
    motorRight->goBack();
  }
  else{
    motorRight->goForward();
  }
  motorLeft->setSpeed(max_speed);
  motorRight->setSpeed(max_speed);
}


void MoveModule::rotate(double angle){
  motorLeft->setSpeed(max_speed);
  motorRight->setSpeed(max_speed);
  if (angle < 0){
    motorLeft->goBack();
    stoper_l = false;
    motorRight->goForward();
    stoper_r = true;
//    motorRight->setSpeed(0);
  }
  else{
    motorRight->goBack();
    stoper_r = false;
    motorLeft->goForward();
    stoper_l = true;
//    motorLeft->setSpeed(0);
  }
  optoPara->reset();
  scale = 1.0;
  last_time = micros();
  ready_flag = false;
  status = MM_ROTATE;
  // status = MM_MOVE;
  this->distance = abs(angle)*PI/180*4.2;
}

void MoveModule::rotate_left(){
  set_max_speed(190);
  rotate(95.0);
}
void MoveModule::rotate_right(){
  set_max_speed(190);
  rotate(-95.0);
}

void MoveModule::go_half(){
  move(9.0, 1.0);
}

void MoveModule::turn_left(){
  move(9.0, 0.36);
}

void MoveModule::turn_right(){
  move(3.0, 3.0);
}

bool MoveModule::ready(){
  return ready_flag;
}

