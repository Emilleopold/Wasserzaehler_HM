/*
 Demonstration sketch for Adafruit i2c/SPI LCD backpack
 using MCP23008 I2C expander
 ( http://www.ladyada.net/products/i2cspilcdbackpack/index.html )

 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * 5V to Arduino 5V pin
 * GND to Arduino GND pin
 * CLK to Analog #5 at YUN #3
 * DAT to Analog #4 at YUN #2 
*/

// include the library code:
#include "Wire.h"
#include "LiquidCrystal.h"

/******************************************************************************
Beispiel:      showTemperatureWithConversation

Beschreibung:  Dieses Beispiel zeigt das Auslesen des Temperatursensors MCP9801
               und die Ausgabe der Temperatur auf dem Display. Die Anzeige kann
               dabei mit Hilfe der Taster zwischen °C, °F und K umgeschaltet
               werden.
******************************************************************************/
//#include "Wire.h"
#include "I2C_4DLED.h"
#include "MCP9801.h"

/******************************************************************************
globale Konstanten
******************************************************************************/
// mögliche Einheiten der Temperaturanzeige
enum Unit
{
  UNIT_CELSIUS = 0x00,
  UNIT_FAHRENHEIT,
  UNIT_KELVIN
};

// Die I2C-Adresse des SAA1064-Chips ergibt sich durch die Belegung der Lötjumper
// J6 bis J9, wobei immer nur ein Jumper gebrückt sein darf.
// J6 gebrückt -> Adresse 0x76
// J7 gebrückt -> Adresse 0x74
// J8 gebrückt -> Adresse 0x72
// J9 gebrückt -> Adresse 0x70 (werkseitig gebrückt)
const uint8_t i2cAddressSAA1064 = 0x70;

// Die I2C-Adresse des MCP9801-Chips ergibt sich durch die Belegung der Lötjumper
// J10 bis J12.
//
// 0: Lötjumper gebrückt (Pin liegt an Masse)
// 1: Lötjumper offen (Pin liegt an Versorgungsspannung)
//
//   J12 (A2)        J11 (A1)        J10 (A0)        Adresse
// ---------------------------------------------------------
//    0               0               0                0x90
//    0               0               1                0x92
//    0               1               0                0x94
//    0               1               1                0x96
//    1               0               0                0x98
//    1               0               1                0x9A
//    1               1               0                0x9C
//    1               1               1                0x9E
const uint8_t i2cAddressMCP9801 = 0x90;

// Pinbelegung der Tasten zur Auswahl der Einheit
const uint8_t pinCelsiusButton        = 4;
const uint8_t pinFahrenheitButton     = 5;
const uint8_t pinKelvinButton         = 6;

/******************************************************************************
globale Variablen
******************************************************************************/
// Temperaturwert
int32_t temperature = 0;

// Einheit in der die Temperatur angezeigt wird
uint8_t unit = UNIT_CELSIUS;

// Connect via i2c, default address #0 (A0-A2 not jumpered)
LiquidCrystal lcd(0);

int ledrd = 13; // Rote LED on board
int ledStateLoop = LOW;             // ledState used to set the LED in loop()


void setup() {
  // set up the LCD's number of rows and columns: 
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  
  // I2C-Modul initialisieren
  // Wire.begin();
  
  // 4-Digit-LED-Anzeige mit I2C-Adresse des SAA1064 initialisieren
  FourDigitLedDisplay.begin(i2cAddressSAA1064);
  
  // Temperatursensor mit I2C-Adresse des MCP9801 initialisieren
  TemperatureSensor.begin(i2cAddressMCP9801);
  
  // Temperatursensor auf eine Auflösung von 12 Bit umschalten
  TemperatureSensor.setADCResolution(TemperatureSensor.RESOLUTION_12BIT);
  
  // Pins für die Taster als Eingänge setzen
  pinMode(pinCelsiusButton, INPUT);
  pinMode(pinFahrenheitButton, INPUT);
  pinMode(pinKelvinButton, INPUT);

  // interne Pullup-Widerstände für die Tastereingänge aktivieren  
  digitalWrite(pinCelsiusButton, HIGH);
  digitalWrite(pinFahrenheitButton, HIGH);
  digitalWrite(pinKelvinButton, HIGH);
  
  pinMode(ledrd,OUTPUT);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);

  lcd.setBacklight(HIGH);
  delay(500);
  lcd.setBacklight(LOW);
  delay(500);

  // Temperaturwert in Grad Celsius vom Sensor lesen
  // (der Temperaturwert ist in Feskommadarstellung mit 4 Nachkommastellen gespeichert)
  temperature = TemperatureSensor.readTemperature();
  
  // falls eine andere Einheit ausgewählt ist, die Temperatur entsprechend
  // umrechnen
  if(unit == UNIT_FAHRENHEIT)
  {
    temperature = temperature * 18 / 10 + 320000;
  }
  else if(unit == UNIT_KELVIN)
  {
    temperature = temperature + 2731500;
  }
  
  // Temperatur in der ausgewählten Einheit auf dem Display mit
  // einer Nachkommastelle anzeigen
  FourDigitLedDisplay.writeDecimal( (temperature) / 1000, 1 );
  
  // Auswahl der Einheit bei Betätigung des jeweiligen Tasters umschalten
  if(digitalRead(pinCelsiusButton) == 0) unit = UNIT_CELSIUS;
  if(digitalRead(pinFahrenheitButton) == 0) unit = UNIT_FAHRENHEIT;
  if(digitalRead(pinKelvinButton) == 0) unit = UNIT_KELVIN;
 
  //delay(200);
  
  if (ledStateLoop == LOW)
    ledStateLoop = HIGH;
  else
    ledStateLoop = LOW;
  digitalWrite(ledrd, ledStateLoop);

}

