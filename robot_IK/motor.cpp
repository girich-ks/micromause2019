// Motor.cpp

#include "motor.h"

#include "Arduino.h"

//#include <cmath>

void Motor::stopMotor() { // остановить мотор
  if (MotorDir != MD_STOP) {
    MotorDir = MD_STOP;
//    digitalWrite(Motor5_pin, 0);
    analogWrite(Motor6_pin, 0);
//    digitalWrite(Motor7_pin, 0);
//    digitalWrite(17, 0);
  }
}
void Motor::goForward() { // Р·Р°РїСѓСЃС‚РёС‚СЊ РјРѕС‚РѕСЂ РІРїРµСЂРµРґ
  if (MotorDir != MD_FORWARD) {
    MotorDir = MD_FORWARD;
    digitalWrite(Motor5_pin, 0);
    analogWrite(Motor6_pin, speed);
    digitalWrite(Motor7_pin, 1);
//    digitalWrite(17, 1); зачем?
  }
}

void Motor::goBack() { // запустить мотор вперед
  if (MotorDir != MD_BACK) {
    MotorDir = MD_BACK;    
    digitalWrite(Motor5_pin, 1);
    analogWrite(Motor6_pin, speed);
    digitalWrite(Motor7_pin, 0);
    
//    digitalWrite(17, 1); зачем?
  }
}

void Motor::setSpeed(byte aspeed){
  analogWrite(Motor6_pin, aspeed);
  speed = aspeed;  
}

byte Motor::getSpeed(){
  return speed;
}


void MoveModule::loop(){
  switch (status){
    case MM_STOP:{
      if (micros() - last_time < 40000) return;
      last_time = micros();
      double speedl = encoder->get_distance_from_pin1() - last_dist_l;
      double speedr = encoder->get_distance_from_pin2() - last_dist_r;
      last_dist_l = encoder->get_distance_from_pin1();
      last_dist_r = encoder->get_distance_from_pin2();
      // Serial.println(speedl);
      motorLeft->stopMotor();
      motorRight->stopMotor();
      status = MM_NONE;
      ready_flag = true;
      break;
    }
    case MM_MOVE:{
      if (micros() - last_time < 50) return; // частота формирования СУ на моторы
      last_time = micros();
      float dist_l = encoder->get_distance_from_pin1() - last_dist_l;
      float dist_r = encoder->get_distance_from_pin2() - last_dist_r;
      
//      Serial.print("otpopara: get_distance_left: ");
//      Serial.println(encoder->get_distance_left());
//      Serial.print(" & ");
//      Serial.println(dist_r);
//      
      if (dist_l >= distance && dist_r >= distance*scale){
        hard_stop();
        Serial.println("MoveModule::HARD_STOP");
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
//      Serial.print("MM_Rotate");
      if (micros() - last_time < 100) return;
      last_time = micros();
      double dist_l = encoder->get_distance_from_pin1();
      double dist_r = encoder->get_distance_from_pin2();
      double speed_l = encoder->speed_l;
      double speed_r = encoder->speed_r;

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
//          encoder->reset();
//          last_time = micros();
//          return;
//        }
        return;
      }
      if (dist_l > dist_r){
        motorLeft->setSpeed(max_speed/10);
        motorRight->setSpeed(max_speed);
      }
      if (dist_l < dist_r){
        motorRight->setSpeed(max_speed/10);
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
  last_dist_l = encoder->get_distance_from_pin1();
  last_dist_r = encoder->get_distance_from_pin2();
  scale = ascale;
  last_time = micros();
  ready_flag = false;
  this->distance = distance;
  Serial.println("MoveModule::move");
}

void MoveModule::move_back(double distance, double ascale){
  motorLeft->setSpeed(max_speed);
  motorRight->setSpeed(max_speed);
  motorLeft->goBack();
  stoper_l = false;
  motorRight->goBack();
  stoper_r = false;
  last_dist_l = encoder->get_distance_from_pin1();
  last_dist_r = encoder->get_distance_from_pin2();
  scale = ascale;
  last_time = micros();
  ready_flag = false;
  this->distance = distance;
}

void MoveModule::hard_stop(){
  last_dist_l = encoder->get_distance_from_pin1();
  last_dist_r = encoder->get_distance_from_pin2();
  // last
  ready_flag = false;
  status = MM_STOP;
//  if (stoper_l){
//    motorLeft->goBack();
//  }
//  else{
//    motorLeft->goForward();
//  }
//  if (stoper_r){
//    motorRight->goBack();
//  }
//  else{
//    motorRight->goForward();
//  }
//  motorLeft->setSpeed(max_speed);
//  motorRight->setSpeed(max_speed);
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
  encoder->reset_values();
  scale = 1.0;
  last_time = micros();
  ready_flag = false;
  status = MM_ROTATE;
  // status = MM_MOVE;
  this->distance = abs(angle)*PI/180*4.2;
}

void MoveModule::rotate_left(){
  set_max_speed(max_speed);
  rotate(95.0);
}
void MoveModule::rotate_right(){
  set_max_speed(max_speed);
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
