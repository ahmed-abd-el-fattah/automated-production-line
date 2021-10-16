#include <Servo.h>
//IR sensor to stop conveyer
int irPin =A4 ;
int sensorOut;
int counter1=0; //counter for sensor readings to
reduce error in readings
//communication
char stop_key[2]="f";
//stepper motor conveyer
#define dirPin 4
#define stepPin 3
#define stepsPerRevolution 1600
#define delay1 400

//relay_suction
int RELAY1 = 8;
int delayValue = 1000;
//relay_pusher
int RELAYF = 12; //F for forward positiom
int delayF = 1000;
//relay_unpusher
int RELAYB = 11 ; //B for backward
int delayB = 1000;
//IR sensor to start the conveyer
int irPin2 = A1;
int sensorOut2;
//suction
Servo myservo;
#define servoPin 10
int angle = 100;
void setup()
{
//relay_suction
pinMode(RELAY1, OUTPUT);
digitalWrite(RELAY1, LOW); //dont suck air initially
//relay_pushing
pinMode(RELAYF, OUTPUT);
pinMode(RELAYB, OUTPUT);
digitalWrite(RELAYB,LOW); //LOW means active (send pusher
to backward position)
digitalWrite(RELAYF, HIGH); //HIGH means inactive
//sensors
pinMode(irPin, INPUT);
pinMode(irPin2, INPUT);
//conveyer
pinMode(stepPin, OUTPUT);
pinMode(dirPin, OUTPUT);
//suction
myservo.attach(servoPin);
pinMode(servoPin, OUTPUT);
move_suction(95);
Serial.begin(9600); //begin serial comm with baud
rate 9600
}

void move_suction(int pos)
{
myservo.write(pos); //move suction to a given angle
}
void suck() // suck the cover for
disassembly
{
digitalWrite(RELAY1, HIGH); //activate the relay for the
solenoid valve for vacuum generator
}
void unsuck()
{
digitalWrite(RELAY1, LOW); //deactivate the relay for the
solenoid valve for vacuum generator
}
void push()
{
digitalWrite(RELAYB,HIGH); // deactivate backward position
relay (normal position of the piston is front)
delay(100);
}
void unpush()
{
digitalWrite(RELAYB, LOW); // deactivate backward position
relay
delay(100);
}
void perform_disassembly()
{
push();
move_suction(10);
suck();
move_suction(70);
unpush();
move_conveyer(1.5);
move_suction(50);
unsuck();
move_suction(90);
move_conveyer_wait_for_TX();
}
void move_conveyer_wait_for_sensor()
{
int count=0; //count for sensor readings to
eliminate errors in readings
int flag=0; //flag to perform disassembly
(1=perform)

sensorOut=analogRead(irPin); //get readings of IR sensor
digitalWrite(dirPin, LOW); //set rotation to clockwise
direction
for (int i = 0; i < 200 * stepsPerRevolution; i++) //move conveyer
continioulsy
{
// These four lines result in 1 step:
sensorOut=analogRead(irPin); // read sensor readings to stop
// Spin the stepper motor 1 revolution:
digitalWrite(stepPin, HIGH);
delayMicroseconds(delay1);
digitalWrite(stepPin, LOW);
delayMicroseconds(delay1);
//calibration to stop motor
if( sensorOut<70 || sensorOut>100) //the product is infront of the
suction cup ready for disassembly
{
count++;
if(count==850) // counter to remove error in
reading
{
count=0; //reset
flag=1; //flag to perform disassembly
break; //stop the stepper motor by
breaking of the for loop
}
}
}
if(flag==1)
{
perform_disassembly(); // do disassembly functions
}
}
void move_conveyer_wait_for_TX()
{
digitalWrite(dirPin, LOW); //set rotation to clockwise
direction
for (int i = 0; i < 200 * stepsPerRevolution; i++) //move conveyer
continioulsy
{
// These four lines result in 1 step:
sensorOut=analogRead(irPin); // read sensor readings to stop
// Spin the stepper motor 1 revolution:

digitalWrite(stepPin, HIGH);
delayMicroseconds(delay1);
digitalWrite(stepPin, LOW);
delayMicroseconds(delay1);
if(i%400==0) //check each 1/4 revolution
{
Serial.readBytes(stop_key,2); //Read the serial data from TX
of feeding station and store in stop_key
if(stop_key[0]=='o') //o= stop the conveyer ,f=dont
stop conveyer
{
digitalWrite(stepPin, LOW);
break;
}
}
}
}
void loop()
{
sensorOut2=analogRead(irPin2); //get readings of IR sensor to
start conveyer
if(sensorOut2<300&&sensorOut2>60) //if there is a product
(calibration values)
{
counter1++;
delay(20);
if(counter1==20) // a product has arrived
{
counter1=0;
move_conveyer_wait_for_sensor(); //move conveyer to start
disassembly process
}
}
}
