// Tugas Besar Robotika Semester 7
// Kelompok 2 EL-44-G1
// Januari 2023

#include <AFMotor.h>
#include <Servo.h>

#define pinTrig 2
#define pinEcho 13

AF_DCMotor rightBack(1, MOTOR12_1KHZ); 
AF_DCMotor rightFront(2, MOTOR12_1KHZ);
AF_DCMotor leftFront(3, MOTOR34_1KHZ);
AF_DCMotor leftBack(4, MOTOR34_1KHZ);

Servo servoLook;

int maxDist = 150;
int stopDist = 50;
float timeOut = 2*(maxDist+10)/100/340*1000000;

int motorSpeed = 55;
int motorOffset = 10;
int turnSpeed = 50;

int buzzer = A0;
int smoke = A5;
// Nilai treshold untuk sensor gas
int sensorThres = 100;
int analogSensor = 0;

// Inisialisasi sistem
void setup()
{
  rightBack.setSpeed(motorSpeed);
  rightFront.setSpeed(motorSpeed);
  leftFront.setSpeed(motorSpeed);
  leftBack.setSpeed(motorSpeed);

  rightBack.run(RELEASE);
  rightFront.run(RELEASE);
  leftBack.run(RELEASE);
  leftFront.run(RELEASE);
  servoLook.attach(10);

  pinMode(buzzer, OUTPUT);
  pinMode(smoke, INPUT);

  pinMode(pinTrig,OUTPUT);
  pinMode(pinEcho,INPUT);
}

// Dijalankan berulang
void loop()
{
  Serial.println("Mulai");
  servoLook.write(90);
  delay(1000);
  int distance = getDistance();
  if (distance >= stopDist)
  {moveForward();}
  while(distance >= stopDist)
  {
    distance = getDistance();
    delay(250);
  }
  moveStop();
  int turnDir = checkDirection();
  Serial.print("Cek Arah = : ");
  Serial.println(turnDir);
  switch (turnDir)
  {
    case 0:
    turnLeft(400);
    break;

    case 1:
    turnLeft(700);
    break;

    case 2:
    turnRight(400);
    break;
  }
}

// Untuk Gas Maju 4 Motor DC
void moveForward()
{
    rightBack.run(FORWARD);
    rightFront.run(FORWARD);
    leftBack.run(FORWARD);
    leftFront.run(FORWARD);
}

// Untuk Menghentikan 4 Motor DC
void moveStop()
{
    rightBack.run(RELEASE);
    rightFront.run(RELEASE);
    leftBack.run(RELEASE);
    leftFront.run(RELEASE);    
}

// Untuk Robot Belok ke Kiri
// Motor DC Sisi Kanan Maju dengan Speed berikut
// Motor DC Sisi Kiri Mundur dengan Speed berikut
void turnLeft(int duration)
{
    rightBack.setSpeed(motorSpeed+turnSpeed);
    rightFront.setSpeed(motorSpeed+turnSpeed);
    leftBack.setSpeed(motorSpeed+turnSpeed+motorOffset);
    leftFront.setSpeed(motorSpeed+turnSpeed+motorOffset);
    // Motor DC Kanan Bergerak Maju
    rightBack.run(FORWARD);
    rightFront.run(FORWARD);
    // Motor DC Kiri Bergerak Mundur
    leftBack.run(BACKWARD);
    leftFront.run(BACKWARD); 
    delay(duration);
    // Set Motor DC ke motorSpeed
    rightBack.setSpeed(motorSpeed);
    rightFront.setSpeed(motorSpeed);
    leftBack.setSpeed(motorSpeed+motorOffset);
    leftFront.setSpeed(motorSpeed+motorOffset);
    rightBack.run(RELEASE);
    rightFront.run(RELEASE);
    leftBack.run(RELEASE);
    leftFront.run(RELEASE);
}

// Untuk Robot Belok ke Kanan
// Motor DC Sisi Kanan Mundur dengan Speed berikut
// Motor DC Sisi Kiri Maju dengan Speed berikut
void turnRight(int duration)
{
    rightBack.setSpeed(motorSpeed+turnSpeed);
    rightFront.setSpeed(motorSpeed+turnSpeed);
    leftBack.setSpeed(motorSpeed+turnSpeed+motorOffset);
    leftFront.setSpeed(motorSpeed+turnSpeed+motorOffset);
    // Motor DC Kanan Bergerak Mundur
    rightBack.run(BACKWARD);
    rightFront.run(BACKWARD);
    // Motor DC Kiri Bergerak Maju
    leftBack.run(FORWARD);
    leftFront.run(FORWARD);
    delay(duration);
    // Set Motor DC ke motorSpeed
    rightBack.setSpeed(motorSpeed);
    rightFront.setSpeed(motorSpeed);
    leftBack.setSpeed(motorSpeed+motorOffset);
    leftFront.setSpeed(motorSpeed+motorOffset);
    rightBack.run(RELEASE);
    rightFront.run(RELEASE);
    leftBack.run(RELEASE);
    leftFront.run(RELEASE);
}

// Untuk Mengukur Jarak di depan Robot
int getDistance()
{
    unsigned long pulseTime;
    int distance;
    digitalWrite(pinTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinTrig, LOW);
    // Mengukur Waktu dari Sinyal Pulse untuk Kembali
    pulseTime = pulseIn(pinEcho, HIGH, timeOut);
    distance = (float) pulseTime * 340 / 2 / 10000;
    Serial.print("Jarak di Depan = ");
    Serial.println(distance);
    return distance;
}

// Untuk Mengecek Arah Robot dan Menentukan akan Bergerak kemana
int checkDirection()
{
    gasMQ2();
    int distances [2] = {0,0};
    // Arah untuk berbelok, 0 kiri, 1 kembali, 2 kanan
    int turnDir = 1;
    servoLook.write(180);
    delay(500);
    distances [0] = getDistance();
    servoLook.write(0);
    delay(1000);
    distances [1] = getDistance();
    // Jika Kedua Arah Sudah Jelas, Belok Kiri
    if (distances[0]>=200 && distances[1]>=200)
        turnDir = 0;
    // Jika Kedua Arah Terblokir, Putar Balik
    else if (distances[0]<=stopDist && distances[1]<=stopDist)
        turnDir = 1;
    // Apabila sisi kiri masih ada ruang, belok kiri
    else if (distances[0] >= distances[1])
        turnDir = 0;
    // Apabila sisi kanan masih ada ruang , belok kanan
    else if (distances[0] < distances[1])
        turnDir = 2;
    return turnDir;
}

// Fungsi untuk mendeteksi adanya gas di sekitar robot
void gasMQ2()
{
  analogSensor = analogRead(smoke);
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Cek apakah sudah mencapai nilai treshold
  if (analogSensor > sensorThres)
  {
    moveStop();
    tone(buzzer, 500);
  }
  else
  {
    noTone(buzzer);
  }
  checkDirection();
}

