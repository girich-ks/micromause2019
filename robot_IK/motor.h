#pragma once

#include "Arduino.h"

#include "CEncoder.h"

enum MotorDirection {MD_NONE, MD_STOP, MD_FORWARD, MD_LEFT, MD_RIGHT, MD_BACK};
enum MoveModuleStatus {MM_NONE, MM_STOP, MM_MOVE, MM_ROTATE, MM_TURN};

class Motor;
class CEncoder;
typedef CEncoder* PCEncoder;
typedef Motor* PMotor;

class Motor {
  public:
    Motor(){} // конструктор
    
    Motor(byte p5, byte p6, byte p7, byte speed = 0): 
      Motor5_pin(p5), Motor6_pin(p6), Motor7_pin(p7), speed(speed){
      pinMode(Motor5_pin, OUTPUT);
      analogWrite(Motor6_pin, speed);
      pinMode(Motor7_pin, OUTPUT);
      this->stopMotor();
    } // конструктор
    
    void goForward();
    void goBack(); // запустить мотор назад вправо// запустить мотор вперед
    void stopMotor();// остановить мотор
    void setSpeed(byte speed);
    byte getSpeed();
  private:
    byte speed;
    byte Motor5_pin;
    byte Motor6_pin;
    byte Motor7_pin;
    MotorDirection MotorDir;
};

class MoveModule{
public:
  MoveModule(): encoder(), motorLeft(), motorRight(), scale(1.0){}
  MoveModule(PCEncoder _encoder, PMotor motorLeft, PMotor motorRight): 
    encoder(_encoder), motorLeft(motorLeft), motorRight(motorRight), scale(1.0),
    max_speed(255), ready_flag(true){}
  void loop();
  void setup();
  void move(double distance, double scale);
  void move_back(double distance, double scale);
  void hard_stop();
  void rotate(double angle);
  void rotate_left();
  void rotate_right();
  void set_max_speed(byte sp){
    max_speed = sp;
  }
  void set_scale(double mod){
    scale = mod;
  }
  void go_half();
  void turn_left();
  void turn_right();
  bool ready();
  MoveModuleStatus status;
private:
  double distance;
  double angle;
  double scale;
  unsigned long last_time;
  double last_dist_l;
  double last_dist_r;
  bool ready_flag;
  byte max_speed;

  bool stoper_l;
  bool stoper_r;

  PCEncoder encoder;
  PMotor motorLeft;
  PMotor motorRight;
};
