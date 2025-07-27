//Motor pins
#define rmf 5 //IN1
#define rmb 18  //IN2
#define lmf 19  //IN3
#define lmb 2  //IN2
//EnA and EnB
#define rms 4  //EnA
#define lms 15  //EnB

int sensor[6];  //to store the sensor value
int threshold = 512; //threshold = (minimum analog value + Maximum Analog Value) / 2
float c;
int left_motor_speed = 200, right_motor_speed = 200;
int left_motor, right_motor;
int kp = 40, kd = 1000;
int PID_value;
float current_error, previous_error;
int turn_speed =120;
char t;

//Sensor pins
#define sensor1 12
#define sensor2 14
#define sensor3 27
#define sensor4 26
#define sensor5 25
#define sensor6 33

void setup() {
  //motor driver pins as output
  pinMode(lmf, OUTPUT);
  pinMode(lmb, OUTPUT);
  pinMode(rmf, OUTPUT);
  pinMode(rmb, OUTPUT);
  pinMode(23,OUTPUT);

  //sensor pins as input
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);
  pinMode(sensor6, INPUT);

  //EnA and EnB pins as output
  pinMode(rms, OUTPUT);
  pinMode(lms, OUTPUT);
  digitalWrite(23,HIGH);

  Serial.begin(9600);
}

void loop() {
  Line_Follow();  // line follow using pid
}

void sensor_reading() {
  float a;
  float b;
  
  // Read analog values from the sensors
  sensor[0] = analogRead(sensor1);
  sensor[1] = analogRead(sensor2);
  sensor[2] = analogRead(sensor3);
  sensor[3] = analogRead(sensor4);
  sensor[4] = analogRead(sensor5);
  sensor[5] = analogRead(sensor6);

  // Check if the sensor value is above the threshold
  for (int i = 0; i < 6; i++) {
    if (sensor[i] > threshold) {
      sensor[i] = 1;
    } else {
      sensor[i] = 0;
    }
  }
  
  a = (sensor[0] * 1 + sensor[1] * 2 + sensor[2] * 3 + sensor[3] * 4 + sensor[4] * 5 + sensor[5] * 6);
  b = (sensor[0] + sensor[1] + sensor[2] + sensor[3] + sensor[4] + sensor[5]);
  if (b > 0) c = a / b;
}

void Line_Follow() {
  while (1) {
    sensor_reading();

    // Straight Line Follow
    if (sensor[2] == 1 || sensor[3] == 1) {
      current_error = 3.5 - c;
      PID_value = current_error * kp + kd * (current_error - previous_error);
      previous_error = current_error;

      right_motor = right_motor_speed - PID_value;
      left_motor = left_motor_speed + PID_value;
      motor(left_motor, right_motor);
    }

    // All sensors on white surface
    if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0) {
      if (t == 'r') right();
      else if (t == 'l') left();
      else U_turn();
    }

    // Right Turn
    if (sensor[5] == 0 && sensor[0] == 1) t = 'r';
    // Left Turn
    if (sensor[5] == 1 && sensor[0] == 0) t = 'l';

    // All sensors on black surface
    if (sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 1) {
      delay(30);
      sensor_reading();
      if ((sensor[0] + sensor[1] + sensor[2] + sensor[3] + sensor[4] + sensor[5]) == 6) {
        motor(0, 0);  // stop
        while ((sensor[0] + sensor[1] + sensor[2] + sensor[3] + sensor[4] + sensor[5]) == 6) sensor_reading();
      } else if ((sensor[0] + sensor[1] + sensor[2] + sensor[3] + sensor[4] + sensor[5]) == 0) t = 'r';
    }
  }
}

void right() {
  while (1) {
    motor(turn_speed, -turn_speed);
    while (sensor[2] == 0 && sensor[3] == 0) sensor_reading();
    motor(-turn_speed, turn_speed);
    delay(20);
    break;
  }
}

void left() {
  while (1) {
    motor(-turn_speed, turn_speed);
    while (sensor[2] == 0 && sensor[3] == 0) sensor_reading();
    motor(turn_speed, -turn_speed);
    delay(20);
    break;
  }
}

void U_turn() {
  while (1) {
    delay(120);
    motor(-turn_speed, turn_speed);
    while (sensor[2] == 0 && sensor[3] == 0) sensor_reading();
    motor(turn_speed, -turn_speed);
    delay(20);
    break;
  }
}

//motor run function
void motor(int a, int b) {
  if (a > 0) {
    digitalWrite(lmf, 1);
    digitalWrite(lmb, 0);
  } else {
    a = -(a);
    digitalWrite(lmf, 0);
    digitalWrite(lmb, 1);
  }
  if (b > 0) {
    digitalWrite(rmf, 1);
    digitalWrite(rmb, 0);
  } else {
    b = -(b);
    digitalWrite(rmf, 0);
    digitalWrite(rmb, 1);
  }

  if (a > 250) a = 250;
  if (b > 250) b = 250;

  analogWrite(lms, a);
  analogWrite(rms, b);
}
