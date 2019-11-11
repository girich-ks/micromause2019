
/*robot_IK.ino
*/
#include <SharpDistSensor.h>
//#include <SimpleKalmanFilter.h>
#include "motor.h"
//#include "graph.h"
//#include "sensors.h"
#include "CRegulator.h"

//#define PRINT_IN_READ
//#define KALMAN_FILTER

#define DIST_TO_WALL 3 // стандартное расстояние до стены, которое нужно поддерживать 
#define MAX_IR_DIST 19.0 // максимальное расстояние, определяемое ИК датчиком
//#define MEAS_ACTUAL_DURATION 500 // время в миллисекундах, в течение которого измерение считается актальным
#define WALL_DIST 9 // Порог определения стены
#define MAX_WHEEL_SPEED 110

/* -------------------------------------  Распиновка моторов   ------------------------------*/
const byte motor_left_pinA = 12;
const byte motor_left_pinB = 14;
const byte motor_left_pinE = 9;

const byte motor_right_pinA = 2;
const byte motor_right_pinB = 8;
const byte motor_right_pinE = 3;

Motor motorLeft(motor_left_pinA, motor_left_pinE, motor_left_pinB); // создать мотор
Motor motorRight(motor_right_pinA, motor_right_pinE, motor_right_pinB); // создать мотор

///////////////////////////////////////////////////////////////////////////////////////////////
//#2 ИК-датчики
const byte left_senor_pin = A4; // 20й пин левого ик-датчика
const byte mid_senor_pin = A5; // 19й пин центрального ик-датчика
const byte right_senor_pin = A6; // 18й пин правого датчика

// датчики линии
const byte left_line_follower_pin = A3; // 17-й пин левого датчика движения по линии
const byte right_line_follower_pin = 13; // 13-й пин правого датчика движения по линии

// Window size of the median filter (odd number, 1 = no filtering)
const byte medianFilterWindowSize = 1;
SharpDistSensor left_sensor(left_senor_pin, medianFilterWindowSize);  
SharpDistSensor mid_sensor(mid_senor_pin, medianFilterWindowSize);
SharpDistSensor right_sensor(right_senor_pin, medianFilterWindowSize);

// Create the Kalman instances
// Третий аргумент должен соответствовать продолжительности работы одной итерации loop()
#ifdef KALMAN_FILTER
  SimpleKalmanFilter kalmanIRSensorDistLeft(5, 5, 0.1); 
  SimpleKalmanFilter kalmanIRSensorDistMiddle(5, 5, 0.1);
  SimpleKalmanFilter kalmanIRSensorDistRight(5, 5, 0.1);
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
const byte encoder_right_pin1 = 4;
const byte encoder_right_pin2 = 5;
const byte encoder_left_pin1 = 6;
const byte encoder_left_pin2 = 7;

CEncoder right_encoder;
CEncoder left_encoder;

///////////////////////////////////////////////////////////////////////////////////////////////
MoveModule moveModule;
MovementDirection last_move_cmd = MMD_NONE;
int confused_threshold = 0;
///////////////////////////////////////////////////////////////////////////////////////////////

enum ECommand
{
  EC_None,
  EC_Rotate_Left,
  EC_Rotate_Right
};

ECommand command = EC_None;
long long command_time;

void setup() {
  pinMode(1, INPUT);
  pinMode(2, OUTPUT);//in1 for motor1
  pinMode(3, OUTPUT);// enable motor1
  pinMode(encoder_right_pin1, INPUT); // pin #4 - encoder(A) from right motor
  pinMode(encoder_right_pin2, INPUT); // pin #5 - encoder(B) from right motor
  pinMode(encoder_left_pin1, INPUT);  // pin #6 - encoder(A) from left motor // TEST (_!_)
  pinMode(encoder_left_pin2, INPUT);  // pin #7 - encoder(B) from left motor
  pinMode(8, OUTPUT);//in2 for motor1
  pinMode(9, OUTPUT);// enable motor2
  pinMode(10, INPUT);// button right
  pinMode(11, INPUT_PULLUP);// button left
  pinMode(12, OUTPUT);// in4 for motor2
  pinMode(13, INPUT);// 
  pinMode(14, OUTPUT);// pinA0 // in3 for motor2
  pinMode(15, INPUT);// pinA1 // switch(right) for чего-нибудь/ если его включить, то на А1 будет подаваться Vin (7.2В)
  pinMode(16, INPUT);// pinA2 // compas (now not connection)
  pinMode(17, INPUT);// pinA3 // compas (now not connection)
  pinMode(18, INPUT);// pinA4 // IR-sensor Right
  pinMode(19, INPUT);// pinA5 // IR-sensor Center
  pinMode(20, INPUT);// pinA6 // IR-sensor Left
//  pinMode(21, INPUT);
  pinMode(22, INPUT);
    
  Serial.begin(115200);
  left_sensor.setModel(SharpDistSensor::GP2Y0A51SK0F_5V_DS);
  mid_sensor.setModel(SharpDistSensor::GP2Y0A51SK0F_5V_DS);
  right_sensor.setModel(SharpDistSensor::GP2Y0A51SK0F_5V_DS);

  left_encoder = CEncoder("LeftWheel", encoder_left_pin1, encoder_left_pin2, 1.0, 1);
  right_encoder = CEncoder("RightWheel", encoder_right_pin1, encoder_right_pin2, 1.0, 1);   // ошибка
  

  moveModule = MoveModule(&left_encoder, &motorLeft, &motorRight);  
  moveModule.set_max_speed(MAX_WHEEL_SPEED);

//  moveModule.move(10, 1.0);
//  moveModule.move_forward();
//moveModule.turn_right();

}



//int k = 1;
long last_time = 0;
//double last_dist = 0;
double left_dist = MAX_IR_DIST;
double mid_dist = MAX_IR_DIST;
double right_dist = MAX_IR_DIST;
//bool l_flag = false;
//bool r_flag = false;


//int ir_print_count = 0;



C_PID_Regulator sideRegul(2, 0, 0);
//
//bool is_stop = false;
//
int loop_count = 0;
//int mes_count = 0;
//long long count_time = millis();

void loop()
{
 
  loop_count++;

//  motorLeft.goForward();
//  motorRight.goForward();

  right_encoder.loop();
  left_encoder.loop();

  // Формирование показаний датчиков
//  read_sensor_data();
//print_ir_raw_data();

//  int d13 = digitalRead(13);
//  Serial.println(d13);



  // вывод данных на ПК с определенной частотой
  if (millis() - last_time > 1)
  {
    last_time = millis();
//    print_ir_raw_data();
//    print_ir_dists();

//    Serial.print((int)left_encoder.count1);
//    Serial.print("\t");
//    Serial.print((int)left_encoder.count2);
//    Serial.println("\t left encoder");
//
//    Serial.print((int)right_encoder.count1);
//    Serial.print("\t");
//    Serial.print((int)right_encoder.count2);
//    Serial.println("\t right encoder");
    
//    Serial.print("loop count ");
//    Serial.println(loop_count);
    loop_count = 0;

    move_by_line();
  
    moveModule.loop();
//    delay(50);
  }

//  moveModule.hard_stop();
//  moveModule.loop();

  return;
  
  if (right_dist < WALL_DIST && left_dist < WALL_DIST)
  {
    digitalWrite(13, LOW);  
    if (mid_dist > WALL_DIST)
    {
      moveModule.move(100, 1.0);
//      Serial.println("only FORWARD!");
    }
    else
    {
      moveModule.rotate(180);
//      Serial.println("go BACK");
    }
  } 
  else if (right_dist > WALL_DIST && left_dist > WALL_DIST) {
    digitalWrite(13, LOW);  
    moveModule.move(100, 1.0);
//    Serial.println("I'm free to head :)"); 
  }
//  else if (right_dist > WALL_DIST)
//  {
//    digitalWrite(13, HIGH);
//    moveModule.rotate_right();     
//    command = EC_Rotate_Right;
////    Serial.println("Turn to RIGHT");
//  }
//  else if (left_dist > WALL_DIST)
//  {
//    digitalWrite(13, HIGH);
//    moveModule.rotate_left();
//    command = EC_Rotate_Left;
////    Serial.println("Turn to LEFT");
//  }
  
  moveModule.loop();
    
} // loop()

// Считывает даные с ИК-датчиков расстояния и выводит на моитор.
void read_sensor_data()
{
#ifdef KALMAN_FILTER
    left_dist = kalmanIRSensorDistLeft.updateEstimate(left_sensor.getDist()/10.0);
#else
    left_dist = left_sensor.getDist()/10.0;
#endif

#ifdef KALMAN_FILTER
    mid_dist = kalmanIRSensorDistMiddle.updateEstimate(mid_sensor.getDist()/10.0);
#else
    mid_dist = mid_sensor.getDist()/10.0;
#endif

#ifdef KALMAN_FILTER
    right_dist = kalmanIRSensorDistRight.updateEstimate(right_sensor.getDist()/10.0);
#else
    right_dist = right_sensor.getDist()/10.0;
#endif   
}

// движение по черной полосе
void move_by_line()
{
  int leftLF = digitalRead(left_line_follower_pin);
  int rightLF = digitalRead(right_line_follower_pin);

//  Serial.print(leftLF);
//  Serial.print("\t");
//  Serial.println(rightLF);

  if (leftLF == LOW && rightLF == LOW)
  {
    moveModule.move_forward();
    last_move_cmd = MMD_FORWARD;
    Serial.println("to FORWARD");
    confused_threshold-=10;
  }
  else if (leftLF == HIGH && rightLF == HIGH)
  {
//    if (confused_threshold > 1000)
//    {
//      moveModule.move_forward();
//      Serial.println("FORCE FORCE FORCE FORWARD");
//      confused_threshold;
//    }
    if (last_move_cmd == MMD_TURN_RIGHT)
    {
      moveModule.turn_right(); 
      last_move_cmd = MMD_TURN_RIGHT;
      Serial.println("repeat RIGHT");
      confused_threshold++;
    }
    else if (last_move_cmd == MMD_TURN_LEFT)
    {
      moveModule.turn_left(); 
      last_move_cmd = MMD_TURN_LEFT;
      Serial.println("repeat LEFT");
      confused_threshold++;
    }
    else
    { // произошло нечто неординарное
      Serial.println("ЧЕРНАЯ ПОЛОСА ПОПЕРЕК");
      }
  }
  else if (leftLF == LOW && rightLF == HIGH)
  {
    moveModule.turn_right();
    last_move_cmd = MMD_TURN_RIGHT;
    Serial.println("to RIGHT");
  }
  else if (leftLF == HIGH && rightLF == LOW)
  {
    moveModule.turn_left();
    last_move_cmd = MMD_TURN_LEFT;
    Serial.println("to LEFT");
  }
}

void print_ir_dists()
{
  Serial.print(left_dist);
  Serial.print("\t");
  Serial.print(mid_dist);
  Serial.print("\t");
  Serial.print(right_dist);
  Serial.println();
}

void print_ir_raw_data()
{
//  analogRead(left_senor_pin);
//  analogRead(mid_senor_pin);
//  analogRead(right_senor_pin);
  Serial.print(analogRead(left_senor_pin));
  Serial.print("\t");
  Serial.print(analogRead(mid_senor_pin));
  Serial.print("\t");
  Serial.print(analogRead(right_senor_pin));
  Serial.println();
}

float _saturation(const float x, const float value)
{
  return (x > value) ? value : ((x < -value) ? -value: x);
}
