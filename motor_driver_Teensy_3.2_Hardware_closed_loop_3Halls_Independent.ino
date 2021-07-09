

#include <SoftwareSerial.h>
#define LeftMotorPower Serial1

#define RightMotorPower Serial2
#define Forward 1;
#define Reverse -1;

volatile long StartTime = 1;
volatile long EndTime = 1;
volatile bool Counting = 0;
volatile int Direction = 0;
long CheckTime, Timout;
int Speed = 0;
volatile int DesiredSpeedRight = 0;
volatile int DesiredSpeedLeft = 0;
float Error = 0;
float OldError = 0;
float Ki = 0;
float Kd, Kp;
signed int DeliveredPowerLeft = 0;
signed int DeliveredPowerRight = 0;
void setup() 
{
  LeftMotorPower.begin (26315, SERIAL_9N1);
  RightMotorPower.begin (26315, SERIAL_9N1);
  Serial.begin(9600);

  pinMode(3, INPUT); pinMode(4, INPUT); pinMode(2, INPUT); //pins for M1 speed input
  attachInterrupt(2, GetTime1, RISING);
  attachInterrupt(4, GetTime2F, RISING);
  attachInterrupt(3, GetTime2R, RISING);

  pinMode(14, INPUT); pinMode(17, INPUT); pinMode(16, INPUT); pinMode(15, INPUT); //Pins for RC input
  attachInterrupt(14, RadioControlA, RISING);
  attachInterrupt(15, RadioControlB, RISING);
  attachInterrupt(16, RadioControlC, RISING);
  attachInterrupt(17, RadioControlD , RISING);


  StartTime = micros(); DesiredSpeedLeft = 0; DesiredSpeedRight = 0;
}

char c = ' ';


void loop() {
  Serial.println(c);
  if (c == ' ') {
    DesiredSpeedLeft = 0;
    DesiredSpeedRight = 0;
  } else if (c == 'q')
    DesiredSpeedLeft -= 10;
  else if (c == 'o')
    DesiredSpeedRight -= 10;
  else if (c == 'w')
    DesiredSpeedLeft += 10;
  else if (c == 'p')
    DesiredSpeedRight += 10;
  else if (c == '2')
    DesiredSpeedLeft += 100;
  else if (c == '0')
    DesiredSpeedRight += 100;
  else if (c == '1')
    DesiredSpeedLeft -= 100;
  else if (c == '9')
    DesiredSpeedRight -= 100;


  //  Serial.print(" low byte ");
  //  Serial.print((DeliveredPowerLeft & 0xFF), HEX);
  //  Serial.print(" high byte ");
  //  Serial.println((DeliveredPowerLeft >> 8) & 0xFF, HEX);

  do {



    LeftMotorPower.write9bit(256);
    LeftMotorPower.write9bit(DeliveredPowerLeft & 0xFF);
    LeftMotorPower.write9bit((DeliveredPowerLeft >> 8) & 0xFF);
    LeftMotorPower.write9bit(DeliveredPowerLeft & 0xFF);
    LeftMotorPower.write9bit((DeliveredPowerLeft >> 8) & 0xFF);
    LeftMotorPower.write9bit(85);

    RightMotorPower.write9bit(256);
    RightMotorPower.write9bit(-DeliveredPowerRight & 0xFF);
    RightMotorPower.write9bit((-DeliveredPowerRight >> 8) & 0xFF);
    RightMotorPower.write9bit(-DeliveredPowerRight & 0xFF);
    RightMotorPower.write9bit((-DeliveredPowerRight >> 8) & 0xFF);
    RightMotorPower  .write9bit(85);

    Serial.print("Left DesiredSpeed ");  Serial.print(  DesiredSpeedLeft  );
    Serial.print("  DeliveredPowerLeft ");  Serial.print(DeliveredPowerLeft);
    Serial.print("  StartTime ");  Serial.print(StartTime);
    Serial.print("  EndTime ");  Serial.print(EndTime);
    Serial.print("  Direction ");  Serial.print(Direction);
    Serial.print("  Speed ");  Serial.print(Speed);
    Serial.print(  "    Error ");  Serial.println(Error);

    Serial.print("Right DesiredSpeed ");  Serial.print(  DesiredSpeedRight  );
    Serial.print("  DeliveredPowerRight ");  Serial.print(DeliveredPowerRight);
    Serial.print("  StartTime ");  Serial.print(StartTime);
    Serial.print("  EndTime ");  Serial.print(EndTime);
    Serial.print("  Direction ");  Serial.print(Direction);
    Serial.print("  Speed ");  Serial.print(Speed);
    Serial.print(  "    Error ");  Serial.println(Error);

    CheckTime = micros();
    if (CheckTime - StartTime  > 400000) {
      Speed = 0;
      StartTime = micros();
    }


    if (!Counting)
    {
      CalculateSpeed();
    }
    CalculatePID();
    CalculatePowerToBeDelivered();






    // delayMicroseconds(300);
  } while (!Serial.available());
  c = Serial.read();
}




void GetTime1()
{
  StartTime = micros();
  Counting = 1;
}




void GetTime2F()
{
  if (Counting == 1)
  {
    EndTime = micros();
    Counting = 0;
    Direction = Forward;
  }
}




void GetTime2R()
{
  if (Counting == 1)
  {
    EndTime = micros();
    Counting = 0;
    Direction = Reverse;
  }
}




void CalculateSpeed()
{
  if (EndTime == StartTime)
    Speed = 0;
  else
    Speed = 4000000 / (EndTime - StartTime) * Direction;

}



void CalculatePowerToBeDelivered()
{
  DeliveredPowerLeft = DesiredSpeedLeft + Kp + Ki -  Kd;

  if (DeliveredPowerLeft > 600) DeliveredPowerLeft = 600;
  if (DeliveredPowerLeft < -600) DeliveredPowerLeft = -600;


  DeliveredPowerRight = DesiredSpeedRight + Kp + Ki -  Kd;

  if (DeliveredPowerRight > 600) DeliveredPowerRight = 600;
  if (DeliveredPowerRight < -600) DeliveredPowerRight = -600;
}




void CalculatePID()
{
  Error = DesiredSpeedLeft - Speed;
  Kp = Error / 2;
  if (Ki > 1) Ki -= 1;
  if (Ki < -1) Ki += 1;
  //Ki=Ki*.97;
  Ki += Error / 70;
  Kd = 0;//(Error - OldError)/20;
  OldError = Error;
}

void RadioControlA()
{
  DesiredSpeedRight += 100;

}


void RadioControlB()
{
  DesiredSpeedRight -= 100;

}



void RadioControlC()
{
  DesiredSpeedLeft += 100;

}



void RadioControlD()
{

  DesiredSpeedLeft -= 100;
}
