#include <SPI.h>
#include <mcp2515.h>

#define potPin A0
#define lm35Pin A2
#define doorsw 6

int potValue = 0;
float tempValue;  //variable to store celcius
float vout;     // temporay variable to hold sensor reading
int sw;

struct can_frame canMsg1; // pot message
struct can_frame canMsg2; // lm35 message
struct can_frame canMsg3; // door sw message

MCP2515 mcp2515(10);  //chip select cs pin 10

void setup() {
  
  Serial.begin(9600);
  pinMode(doorsw,INPUT_PULLUP);  //configure pin 6 as an input and enable the internal pull-up resistor
  SPI.begin();         //Begins SPI communication
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //Sets CAN at speed 500KBPS
  mcp2515.setNormalMode();
  
  canMsg1.can_id  = 0xAA;  //CAN id as 0xAA for potentiometer
  canMsg1.can_dlc = 2;      //CAN data length as 2 bytes

  canMsg2.can_id  = 0xBB;  //CAN id as 0xBB for LM35
  canMsg2.can_dlc = 1;      //CAN data length as 1 byte

  canMsg3.can_id  = 0xCC;  //CAN id as 0xCC for door switch
  canMsg3.can_dlc = 2;      //CAN data length as 2 bytes
 
}

void loop() {

  potValue = analogRead(potPin);
  tempValue = analogRead(lm35Pin);
  potValue = map(potValue, 0, 1023, 0,255);
  tempValue = (tempValue/1023)*5000;
  vout = tempValue/10;
  int temp = int(vout);

  Serial.println("Temp value deg C :");
  Serial.println(vout);
  Serial.println(potValue);
  
  canMsg1.data[0] = potValue;    //Update pot value in [0]
  canMsg1.data[1] = 0x00; 
  mcp2515.sendMessage(&canMsg1);  //Sends the CAN message
  delay(200);

  canMsg2.data[0] = temp;    //Update lm335 value in [0]
  //canMsg2.data[1] = 0x00; 
  mcp2515.sendMessage(&canMsg2);  //Sends the CAN message
  delay(200);

  canMsg3.data[0] = 0x00;    
  canMsg3.data[1] = sw;   //Update doorsw value in [0]
  mcp2515.sendMessage(&canMsg3);  //Sends the CAN message
  delay(100);
}
