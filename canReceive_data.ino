#include <SPI.h>            //Library for using SPI communication
#include <mcp2515.h>        //Library for using CAN Communication

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);     // set the LCD address to 0x3F

struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;

MCP2515 mcp2515(10);     // SPI CS Pin 10

void setup() {
 lcd.init();    //initialize i2c LCD
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
    if(canMsg1.can_id==0xAA)
    {
   int x = canMsg1.data[0];
   Serial.println(x);
   lcd.setCursor(1,0);
      lcd.print("pot value:");
      lcd.print(x);
     delay(200); 
    }
  }

 if (mcp2515.readMessage(&canMsg2) == MCP2515::ERROR_OK) 
  {
    if(canMsg2.can_id==0xBB)
    {
   int y= canMsg2.data[0];
   Serial.println(y);
   lcd.setCursor(0,1);
      lcd.print("lm35_T:");
      lcd.print(y);
     delay(200); 
    }
  }

  if (mcp2515.readMessage(&canMsg3) == MCP2515::ERROR_OK) 
  {
    if(canMsg3.can_id==0xCC)
    {
   int z = canMsg3.data[1];
   Serial.println(z);
   lcd.setCursor(12,1);
      lcd.print("D");
      lcd.print(z);
     delay(200); 
    }
  }

  delay(20);
  
}
