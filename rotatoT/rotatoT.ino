#include <Wire.h>
#include <Servo.h>

float AccX, AccY, AccZ, AngleUp, AngleAcc, AngleAcc2, AngleServo, AngleServo2;
float offset = 0;
int servoPin = 10;
int servoPin2 = 9;
Servo myServo;
Servo myServo2;


void mpu(void) {
  Wire.beginTransmission(0b1101000); //Slave address
  Wire.write(0x6B); //Register - Power Management
  Wire.write(0b00000000); //Waking up
  Wire.endTransmission(); 

  Wire.beginTransmission(0b1101000);
  Wire.write(0x1C); //Register - Accelerometer Scale
  Wire.write(0b000000); // 2g, Divide by 16384
  Wire.endTransmission();

  Wire.beginTransmission(0b1101000);
  Wire.write(0x3B); //Accelerometer Values
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Pull 6-bytes starting at 0x3B
  int16_t AccXLSB = Wire.read() << 8 | Wire.read(); // This is how 2 1-byte values are combined
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();

  AccX = (float)AccXLSB/16384; //float because it was only 1 or 0
  AccY = (float)AccYLSB/16384;
  AccZ = (float)(AccZLSB/16384);


  AngleUp = (float)atan(AccY/sqrt((sq(AccX)+sq(AccZ))))*(180/3.1459); //Frosh project
  AngleAcc = (float)atan(AccX/sqrt((sq(AccY)+sq(AccZ))))*(180/3.1459);
  AngleAcc2 = (float)(atan(AccY/sqrt((sq(AccX)+sq(AccZ))))*(180/3.1459));
}

void setup() {
  Serial.begin(9600);

  Serial.println("Enter offset:");
  while(Serial.available()==0)
  {

  }
  offset=Serial.parseInt();

  Serial.print("Offset is: ");
  Serial.println(offset);

  myServo.attach(servoPin);
  myServo2.attach(servoPin2);
  myServo.write(90);
  myServo2.write(0);
  delay(2000);

  }

void loop() {
  mpu();

  //AngleServo = 90; These two lines are for the servo always pointing up (remember to change attachment + servo orientation)
  //AngleServo2 = (float)90-AngleUp+offset;

  AngleServo = (float)90-AngleAcc;
  AngleServo2 = (float)90-AngleAcc2+offset;

  Serial.print("Servo Angle = ");
  Serial.print(AngleServo);
  Serial.print(", Servo Angle 2 = ");
  Serial.print(AngleServo2);
  Serial.print(", AccX = ");
  Serial.print(AccX);
  Serial.print(", AccY = ");
  Serial.print(AccY);
  Serial.print(", AccZ = ");
  Serial.print(AccZ);
  Serial.print(", Acc Angle = ");
  Serial.print(AngleAcc);
  Serial.print(", Acc Angle 2 = ");
  Serial.print(AngleAcc2);
  Serial.print(", Up Angle = ");
  Serial.print(AngleUp);
  Serial.print(", Offset = ");
  Serial.print(offset);
  Serial.println(".");

  myServo.write(AngleServo);
  myServo2.write(AngleServo2);

  delay(500);
}