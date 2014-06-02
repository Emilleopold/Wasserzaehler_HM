//DFRobot.com
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f,20,4);  // set the LCD address to 0x3f for a 20 chars and 4 line display

const uint8_t ledrd = 13; // Rote LED on board
boolean ledStateLoop = LOW;             // ledState used to set the LED in 1s-Timer
const uint8_t DO6 = 6; // DO6 on board
const uint8_t DO7 = 7; // DO7 on board
long counter = 0;

void setup()
{
  lcd.init();                      // initialize the lcd 
  pinMode(ledrd, OUTPUT);
  pinMode(DO6, OUTPUT);
  pinMode(DO7, OUTPUT);
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setBacklight(HIGH);
  lcd.print("Hello, world!");
}

void loop()
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis());
  lcd.setCursor(0, 3);
  // print the number of seconds since reset:
  lcd.print(counter);
  ledStateLoop = !ledStateLoop;
  digitalWrite(ledrd, ledStateLoop);
  digitalWrite(DO6, ledStateLoop);
  digitalWrite(DO7, ledStateLoop);
  if (ledStateLoop) ++counter;
  delay(200);
}
