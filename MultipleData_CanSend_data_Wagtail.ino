#include <SPI.h>
#include <mcp2515.h>
#include <Adafruit_MAX31865.h>
#include <Wire.h>

// Pin definitions
#define MAX31865_CS1 7         //Was 10
#define MAX31865_CS2 9
#define MAX31865_CS3 8

#define potPin A0
int potValue = 0;

/*RPM */
#define rpm1in 3
volatile unsigned long T_RPM;    //RPM periode in mikrosekondes.
float RPM_Out = 0;
float EngineRPM;
bool ConvTemp;

// Create MAX31865 objects
Adafruit_MAX31865 max31865_1 = Adafruit_MAX31865(MAX31865_CS1);
Adafruit_MAX31865 max31865_2 = Adafruit_MAX31865(MAX31865_CS2);
Adafruit_MAX31865 max31865_3 = Adafruit_MAX31865(MAX31865_CS3);


struct can_frame canMsg1; // PT100 Sensor1 message
struct can_frame canMsg2; // PT100 Sensor2 message
struct can_frame canMsg3; //PT100 Sensor3 message

MCP2515 mcp2515(10);  //chip select cs pin 10
void updateRPM();

void setup() {
  
  Serial.begin(9600);
  SPI.begin();         //Begins SPI communication
  
  max31865_1.begin(MAX31865_3WIRE);
  max31865_2.begin(MAX31865_3WIRE);
  max31865_3.begin(MAX31865_3WIRE);
  
  /*RPM */
  pinMode (rpm1in, INPUT);
  attachInterrupt(digitalPinToInterrupt(rpm1in), updateRPM, FALLING);

  cli();                      //stop interrupts for till we make the settings
  /*1. First we reset the control register to amke sure we start with everything disabled.*/
  TCCR1A = 0;                 // Reset entire TCCR1A to 0 
  TCCR1B = 0;                 // Reset entire TCCR1B to 0
 
  /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */  
  TCCR1B |= B00000101;        //Set CS12 to 1 so we get prescalar 1024  
  
  /*3. We enable compare match mode on register A*/
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A 
  
  /*4. Set the value of register A to 32500/65536*/
  OCR1A = 4250;               //Finally we set compare register A to this value = 31250;
  sei();                      //Enable back the interrupts

  DDRK = B00000000; 

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //Sets CAN at speed 500KBPS
  mcp2515.setNormalMode();
  
  canMsg1.can_id  = 0xAA;  //CAN id as 0xAA for PT100 Sensor 1
  canMsg1.can_dlc = 2;      //CAN data length as 2 bytes

  canMsg2.can_id  = 0xBB;  //CAN id as 0xBB for PT100 Sensor 2
  canMsg2.can_dlc = 2;      //CAN data length as 2 byte

  canMsg3.can_id  = 0xCC;  //CAN id as 0xCC for PT100 Sensor 3
  canMsg3.can_dlc = 2;      //CAN data length as 2 bytes
 
}
  void loop() {

   /////////////////////////////////////Engine RPM START///////////////////////////
  static float RPM_Per;
  RPM_Per = 1.0 * (T_RPM * 1e-6) + 0.0 * RPM_Per; // 50 000 //T_RPM is in mikrosekondes, RPM_per is in sekondes;

  T_RPM = 4294967295;
  /* Choose the largest possible value for T_RPM after it has been used,
  so that in the next round it will update to 0 if there was no further update in engine RPM from the ECU.
  Then pilot will see 0 RPM and know at least one ECU has gone. */

  RPM_Out = 60 / RPM_Per;

  RPM_Out = (round((RPM_Out) / 10)) * 10;
  
  //////////////////////RPM END//////////////////////////////
    
  potValue = analogRead(potPin);
  
  float temperature1 = max31865_1.temperature(100.0, 430.0);
  float temperature2 = max31865_2.temperature(100.0, 430.0);
  float temperature3 = max31865_3.temperature(100.0, 430.0); 

  Serial.println("Temp value deg C :");
  Serial.println(temperature1);
  Serial.println(potValue);
  Serial.println(temperature3);
  
  canMsg1.data[0] = (int)temperature1;    //PT100 Sensor 1
  canMsg1.data[1] = (int)(temperature1 * 100) % 100; 
  mcp2515.sendMessage(&canMsg1);  //Sends the CAN message
  delay(200);

 /*
  canMsg2.data[0] = (int)temperature2;    //PT100 Sensor 2
  canMsg2.data[1] = (int)(temperature2 * 100) % 100; 
  mcp2515.sendMessage(&canMsg2);  //Sends the CAN message
  delay(200);

  canMsg3.data[0] = (int)temperature3;    
  canMsg3.data[1] =(int)(temperature3 * 100) % 100;   //PT100 Sensor 3
  mcp2515.sendMessage(&canMsg3);  //Sends the CAN message
  delay(100);
  */

  canMsg2.data[0] = potValue;    //POT 
  canMsg2.data[1] = 0x00; 
  mcp2515.sendMessage(&canMsg2);  //Sends the CAN message
  delay(200);

  canMsg3.data[0] = (int)temperature3;    
  canMsg3.data[1] =(int)(temperature3 * 100) % 100;   //PT100 Sensor 3
  mcp2515.sendMessage(&canMsg3);  //Sends the CAN message
  delay(100);
  
  
}

/*RPM Code*/
void updateRPM()
{
  static unsigned long ouTyd = 0;
  unsigned long nuweTyd;
  nuweTyd = micros();
  T_RPM = nuweTyd - ouTyd;
  ouTyd = nuweTyd;
}

ISR(TIMER1_COMPA_vect)
{
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt
  ConvTemp=1;
}
