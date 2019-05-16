//1
//2
//3
/*robot_IK.ino
*/
#include "SharpDistSensor.h"
#include "motor.h"
//#include "graph.h"
#include "sensors.h"
#include "CRegulator.h"

#define DIST_TO_WALL 3 // стандартное расстояние до стены, которое нужно поддерживать 

///////////////////////////////////////////////////////////////////////////////////////////////
// const byte motor_left_pinA = A0;
// const byte motor_left_pinB = A1;
// const byte motor_left_pinE = A4;
// const byte motor_right_pinA = A2;
// const byte motor_right_pinB = A3;
// const byte motor_right_pinE = A5;

const byte motor_left_pinA = 4;
const byte motor_left_pinB = 3;
const byte motor_left_pinE = 5;
const byte motor_right_pinA = 7;
const byte motor_right_pinB = 8;
const byte motor_right_pinE = 6;

Motor motorLeft(motor_left_pinA, motor_left_pinE, motor_left_pinB); // создать мотор
Motor motorRight(motor_right_pinA, motor_right_pinE, motor_right_pinB); // создать мотор

///////////////////////////////////////////////////////////////////////////////////////////////
//#2 датчики
const byte left_senor_pin = A0; // пин левого ик-датчика
const byte mid_senor_pin = A1; // пин центрального ик-датчика
const byte right_senor_pin = A2; // пин правого датчика
SharpDistSensor left_sensor(left_senor_pin);
SharpDistSensor mid_sensor(mid_senor_pin);
SharpDistSensor right_sensor(right_senor_pin);

///////////////////////////////////////////////////////////////////////////////////////////////
const byte encode_left_pin1 = 9;
const byte encode_left_pin2 = 10;
const byte encode_right_pin1 = 11;
const byte encode_right_pin2 = 12;
OptoPara optoPara;

// Encod_er encode_left1(encode_left_pin1, encode_left_pin2, 3);
// Encod_er encode_right1(encode_right_pin1, encode_right_pin2, 3);
// Encoder encode_left(encode_left_pin1, encode_left_pin2);
// Encoder encode_right(encode_right_pin2, encode_right_pin1);

///////////////////////////////////////////////////////////////////////////////////////////////
MoveModule moveModule;

///////////////////////////////////////////////////////////////////////////////////////////////

enum ECommand
{
  EC_None,
  EC_Rotate_Left,
  EC_Rotate_Right
};

ECommand command = EC_None;
long long command_time;
///////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  left_sensor.setModel(SharpDistSensor::GP2Y0A51SK0F_5V_DS);
  mid_sensor.setModel(SharpDistSensor::GP2Y0A51SK0F_5V_DS);
  right_sensor.setModel(SharpDistSensor::GP2Y0A51SK0F_5V_DS);
  // encode_left.read();
  // encode_right.read();
  optoPara = OptoPara(encode_left_pin1, encode_right_pin1, 1.0, 1);
  // motorLeft.goForward();
  // motorRight.goForward();
  // motorLeft.setSpeed(200);
  // motorRight.setSpeed(200);
  moveModule = MoveModule(&optoPara, &motorLeft, &motorRight);
  pinMode(17, OUTPUT);
  pinMode(A5, INPUT);
  moveModule.set_max_speed(190);
  if (!digitalRead(A5)) return;
//   moveModule.move(112, 1.0);
//    motorLeft.goForward();
//    motorRight.goForward();
  pinMode(13, OUTPUT);
}

int k = 1;
long last_time = 0;
double last_dist = 0;
double left_dist = 19;
double mid_dist = 19;
double right_dist = 19;
bool l_flag = false;
bool r_flag = false;

void stabilisation(){
  if (millis() - last_time > 1){
    left_dist = left_sensor.getDist()/10.0;
    mid_dist = mid_sensor.getDist()/10.0;
    right_dist = right_sensor.getDist()/10.0;
    last_time = millis();
      // Serial.print((left_dist));
      // Serial.print(" ");
      // Serial.println(right_dist);
  }
  // Serial.print(left_dist);
  return;
}

void print_dists()
{
  
  Serial.print(left_dist);
  Serial.print("\t");
  Serial.print(mid_dist);
  Serial.print("\t");
  Serial.println(right_dist);
}

// C_PID_Regulator leftRegul(100, 0, 0);
// C_PID_Regulator rightRegul(100, 0, 0);

C_PID_Regulator sideRegul(2, 0, 0);

bool is_stop = false;

int loop_count = 0;
int mes_count = 0;
long long count_time = millis();

void loop()
{
  motorLeft.goForward();
  motorRight.goForward();
      
  loop_count++;

//  Serial.println("Hello!");
 
//  motorLeft.setSpeed(70);
//  motorRight.setSpeed(70);
//  return;
    // stabilisation();
   //print_dists();

  // char* message = "";
  // switch (moveModule.status)
  // {
  //   case MM_NONE:
  //     message = "None";
  //     break;
  //   case MM_STOP:
  //     message = "Stop";
  //     break;
  //   case MM_MOVE:
  //     message = "Move";
  //     break;
  //   case MM_ROTATE:
  //     message = "Rotate";
  //     break;
  // }
  // Serial.println(message);


  // if (moveModule.status != MoveModuleStatus::MM_NONE && moveModule.status != MoveModuleStatus::MM_STOP)
  //   return;

  // if (command != EC_None)
  // {
  //   if (command == EC_Rotate_Right)
  //     moveModule.rotate_right();
  //   else if (command == EC_Rotate_Left)
  //     moveModule.rotate_left();
  //   command = EC_None;
  //   return;
  // }

   // if (mid_dist < 10)
   // {
   //  motorLeft.setSpeed(0);
   //  motorRight.setSpeed(0);
   //  return;
   // }
//
   optoPara.loop();
   moveModule.loop();

//   Serial.print(moveModule.status);
//
//    if (!is_stop)
//    {
//      moveModule.rotate_left();
//      Serial.println("Rotate left");
//      is_stop = true;
//    }
//
//    return;

   
//
//    if (moveModule.status == MM_ROTATE)
//    {
//      Serial.println("ROTATE");
//      return;
//    }

  if (right_dist > 13)
  {
//     moveModule.move(2, 1);

//     delay(2000); 
     
     moveModule.rotate_right();     
    
    // moveModule.move(2, 1);
//     command_time = millis();
     command = EC_Rotate_Right;
//    motorLeft.setSpeed(0);
//    motorRight.setSpeed(0);

//    Serial.println("right");
//    return;
  }
  else if (left_dist > 13)
  {

//    motorLeft.setSpeed(0);
//    motorRight.setSpeed(0);
    digitalWrite(13, HIGH);
//    is_stop = true;
//    moveModule.move(2, 1);

//     delay(1000); 
    moveModule.rotate_left();
//    Serial.println("left");    
//    digitalWrite(3, LOW);
//    digitalWrite(4, HIGH);
//    digitalWrite(5, HIGH);
//    digitalWrite(7, LOW);
//    digitalWrite(8, HIGH);
//    digitalWrite(6, HIGH);
//    delay(100);
//    digitalWrite(3, LOW);
//    digitalWrite(4, LOW);
//    digitalWrite(5, LOW);
//    digitalWrite(7, LOW);
//    digitalWrite(8, LOW);
//    digitalWrite(6, LOW);
//
//    digitalWrite(3, LOW);
//    digitalWrite(4, HIGH);
//    digitalWrite(5, HIGH);
//    digitalWrite(7, HIGH);
//    digitalWrite(8, LOW);
//    digitalWrite(6, HIGH);
//    delay(100);
//
//    digitalWrite(3, LOW);
//    digitalWrite(4, LOW);
//    digitalWrite(5, LOW);
//    digitalWrite(7, LOW);
//    digitalWrite(8, LOW);
//    digitalWrite(6, LOW);


    // moveModule.move(2, 1);
    // command_time = millis();
     command = EC_Rotate_Left;
//     return;
  }
  else    
    digitalWrite(13, LOW);   

//    if (moveModule.status == MM_STOP)
//    {
//       if (command != EC_None)
//       {
//         if (command == EC_Rotate_Right)
//           moveModule.rotate_right();
//         else if (command == EC_Rotate_Left)
//           moveModule.rotate_left();
//         command = EC_None;
//         moveModule.status = MM_NONE;
//       }
//    }


    if (/*moveModule.status == MM_NONE &&*/ millis() - last_time > 1)
    {
      mes_count++;
      
      left_dist = left_sensor.getDist()/10.0;
      mid_dist = mid_sensor.getDist()/10.0;
      right_dist = right_sensor.getDist()/10.0;

      
      // float leftError = left_dist - DIST_TO_WALL;
      // float rightError = right_dist - DIST_TO_WALL;
      float sideError = right_dist - left_dist;
      float deltaTime = millis() - last_time;
      // float lSpeed = leftRegul.calculate(leftError, deltaTime);
      // float rSpeed = rightRegul.calculate(rightError, deltaTime);
      // lSpeed = _saturation(lSpeed, 140.0);
      // rSpeed = _saturation(rSpeed, 140.0);
      float sideValue = sideRegul.calculate(sideError, deltaTime);

      float satValue = 25;
      sideValue = _saturation(sideValue, satValue);

     last_time = millis();

  //    double leftSpeed = 180 - El*kl;
  //    double rightSpeed = 180 + El*kl; 

      float lSpeed = 160 - sideValue;
      float rSpeed = 160 + sideValue;

      if (sideValue > satValue - 1)
      {
        rSpeed = 40;
        lSpeed = 200;
      }
      if (sideValue < -satValue + 1)
      {
        lSpeed = 40;
        rSpeed = 200;
      }
     
      motorLeft.setSpeed(lSpeed);
      motorRight.setSpeed(rSpeed);
//
      Serial.print(mid_dist);
      Serial.print("\t");
      Serial.print(sideError);
      Serial.print("\t");
      Serial.print(sideValue);
      Serial.print("\t");
      Serial.print(lSpeed);
      Serial.print("\t");
      Serial.println(rSpeed);
     
    //  motorLeft.setSpeed(180 - Er*kr);
    //  motorRight.setSpeed(180 + Er*kr);
    
  }
  // if (left_dist < 4.5){
  //   motorLeft.setSpeed();
  // }
  // if (right_dist < 3.5){
  //   motorRight.set_scale(1/1.01);
  // }

  // if (moveModule.ready()){
  //   if (k == 1){
  //     moveModule.move(69, 1.0);
  //     k++;
  //     return;
  //   }
  // }

  // if (millis() - count_time > 1000)
  // {
  //   Serial.print(mes_count);
  //   Serial.print("\t");
  //   Serial.println(loop_count);
  //   mes_count = 0;
  //   loop_count = 0;
  //   count_time = millis();
  // }
}

float _saturation(const float x, const float value)
{
  return (x > value) ? value : ((x < -value) ? -value: x);
}
