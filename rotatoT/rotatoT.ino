#include <Wire.h> //For I2C communication commands, so I can pull data from MPU
#include <Servo.h> //For Servo-specific commands

float AccX, AccY, AccZ, AngleUp, AngleAcc, AngleAcc2, AngleServo, AngleServo2; //Defines variables to be used later
float offset = 0;
int servoPin = 10; //Defines the pins I'll be using for my servos
int servoPin2 = 9;
Servo myServo; //Creating Servo objects
Servo myServo2;


void mpu(void) { //Separating out mpu from loop, and then running the function in loop, just to be more clear
  Wire.beginTransmission(0b1101000); //Slave Address, per DataSheet
  Wire.write(0x6B); //Register - Power Management, per Register Map
  Wire.write(0b00000000); //Need to wake up MPU first
  Wire.endTransmission(); //Begin and end transmission so communication in I2C isn't too complicated.
  //A4 is SDA (Serial Data) for sending and receiving data.
  //A5 is SCL (Serial Clock), which alternates between high and low signals and acts as a metronome or clock. 

  Wire.beginTransmission(0b1101000);
  Wire.write(0x1C); //Register - Accelerometer Scale. Need to set scaling factor so I don't get nonsense outputs. 
  Wire.write(0b000000); // 2g, Divide by 16384
  Wire.endTransmission();

  Wire.beginTransmission(0b1101000);
  Wire.write(0x3B); //Accelerometer Values
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Pull 6-bytes starting at 0x3B. 2-bytes each for X, Y, and Z. 
  int16_t AccXLSB = Wire.read() << 8 | Wire.read(); // This is how 2 1-byte values are combined
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();

  AccX = (float)AccXLSB/16384; //Scaling the output. Float, because I was just getting 1 or 0. 
  AccY = (float)AccYLSB/16384;
  AccZ = (float)(AccZLSB/16384);


  AngleUp = (float)atan(AccY/sqrt((sq(AccX)+sq(AccZ))))*(180/3.1459); //Angle calculation for Frosh project (using Servo 2)
  AngleAcc = (float)atan(AccX/sqrt((sq(AccY)+sq(AccZ))))*(180/3.1459); //Angle calculation for Servo 1 of Senior project
  AngleAcc2 = (float)(atan(AccY/sqrt((sq(AccX)+sq(AccZ))))*(180/3.1459)); //Angle calculation for Servo 2 of Senior project
}

void setup() {
  Serial.begin(9600); //Serial clock monitor

  Serial.println("Enter offset:");
  while(Serial.available()==0) //Pauses code, and thus servo rotations below, until I enter offset. Moreso Frosh project.
  {

  }
  offset=Serial.parseInt();

  Serial.print("Offset is: ");
  Serial.println(offset);

  myServo.attach(servoPin); //Identifying where Servos are connected
  myServo2.attach(servoPin2);
  myServo.write(90); //Initial angles for the servos, just to have a starting point
  myServo2.write(0);
  delay(2000); //Delay for the servos to get into their default position

  }

void loop() {
  mpu();

  //AngleServo = 90; //These two lines are for Frosh project (remember to change attachment + servo orientation)
  //AngleServo2 = (float)90-AngleUp+offset;

  AngleServo = (float)90-AngleAcc; //The calculated angles range from -90 to 90, but servo angles range from 0 to 180.
  AngleServo2 = (float)90-AngleAcc2+offset;

  Serial.print("Servo Angle = "); //Printing everything
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

  myServo.write(AngleServo); //Adjusting servo to above angles
  myServo2.write(AngleServo2);

  delay(500); //Choose this delay to reduce wonky movement 
}