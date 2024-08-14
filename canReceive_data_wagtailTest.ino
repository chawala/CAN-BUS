#include <SPI.h>            //Library for using SPI communication
#include <mcp2515.h>        //Library for using CAN Communication

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);     // set the LCD address to 0x3F

struct can_frame canMsg1;  //PT100 Sensor1 CAN ID 0xAA
struct can_frame canMsg2;  //PT100 Sensor2 CAN ID 0xBB
struct can_frame canMsg3;  //PT100 Sensor3 CAN ID 0xCC

MCP2515 mcp2515(10);     // SPI CS Pin 10

int A0_value;
float Pot0;
float Vref = 5.00;
float voltage;

uint16_t y=0;

void setup() {
  //LED pin
  DDRB |= (1<<DDB1);   // PB1 = O/P Arduino Uno Pin 9
  
 lcd.begin();    //initialize i2c LCD  lcd.init();
 lcd.backlight();
 lcd.setCursor(1,0);
 lcd.print("CAN RECEIVE");
 delay(1000);
 lcd.clear();

 SPI.begin();          //Begins SPI communication
  
 Serial.begin(9600);   //Begins Serial communication to 9600
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);   //Sets CAN at speed 500KBPS
  mcp2515.setNormalMode();
  
}

void loop() {
  if (mcp2515.readMessage(&canMsg1) == MCP2515::ERROR_OK) 
  {
 
   int x = canMsg1.data[0];
   int Pt100 = x;
   if(Pt100 <= 25)
   {
    PORTB |= (1<<PB1);
    }else{
     PORTB &= ~(1<<PB1); 
    }
    
   Serial.println(Pt100);
   lcd.setCursor(1,0);
      lcd.print("PT1: ");
      lcd.print(Pt100);
     delay(200); 
  }

 if (mcp2515.readMessage(&canMsg2) == MCP2515::ERROR_OK) 
  {
   
   y = ((canMsg2.data[0]<<2)+ canMsg2.data[1]);
   Pot0 = y;
   voltage = (Pot0/1023)*5;
   
   Serial.println(y);
   lcd.setCursor(0,1);
      lcd.print("POT: ");
      lcd.print(voltage);
     delay(200); 
   
  }

  if (mcp2515.readMessage(&canMsg3) == MCP2515::ERROR_OK) 
  {
   int z = canMsg3.data[0];
   Serial.println(z);
   lcd.setCursor(10,1);
      lcd.print("PT3:");
      lcd.print(z);
     delay(200); 
  
  }

  delay(20);
  
}
