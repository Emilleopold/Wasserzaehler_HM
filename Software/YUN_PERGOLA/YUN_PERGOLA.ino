#include <Bridge.h>
#include <Process.h>
// #include <YunServer.h>
// #include <YunClient.h>
#include <HttpClient.h>
#include <EEPROM.h>
#include "Timer.h"
#include <Console.h>
#include <avr/wdt.h> // Für AVR-Watchdog

boolean Status = false;
boolean AenderungSenden = false;
char Orderstring[255];
char Str10[10];
char SysVarTemperatur[] = "Temp_UV_Pergola";
char SysVarWasserzaehler[] = "Wasser_Zisterne";
char SysVarWasserfluss[] = "Wasserfluss_Zisterne";
char SysVarWasserdruck[] = "Druck_Zisterne";
char StrTemperatur[10] = "";
char StrWasserfluss[10] = "";
char StrWasserdruck[10] = "";
float Temperatur = 0.0;
float TemperaturAlt = 0.0;
float Wasserfluss = 0.0;
float WasserflussAlt = 0.0;
float Wasserdruck = 0.0;
float WasserdruckAlt = 0.0;
unsigned long Wasserzaehler = 0;
unsigned long WasserzaehlerAlt = 0;
unsigned long WasserzaehlerRaw = 0;
unsigned long WasserzaehlerOld = 0;
unsigned long OldMilli = 0;
//long counter = 0;
unsigned long FlussWasserzaehlerOld = 0;
unsigned long FlussMilli = 0;
unsigned long FlussOldMilli = 0;
int HMSequence = 0;

#define ain0 0
#define int4 7

Timer t;

Process date;
int hours, minutes, seconds;

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
#include <Wire.h>
#include <LiquidCrystal.h>

boolean BackLightState = LOW;             // BackLightState used to set the BackLight
boolean BackLightStateChanged = false;             // BackLightState used to set the BackLight


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
#include <Bounce2.h>

/******************************************************************************
globale Konstanten
******************************************************************************/
// mögliche Einheiten der Temperaturanzeige
/*enum Unit
{
  UNIT_CELSIUS = 0x00,
  UNIT_FAHRENHEIT,
  UNIT_KELVIN
};
*/
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
#define pinButton1 8
#define pinButton2 9
#define pinButton3 10
#define pinButton4 11

// Instantiate a Bounce object
Bounce debounceButton1 = Bounce();
Bounce debounceButton2 = Bounce();
Bounce debounceButton3 = Bounce();
Bounce debounceButton4 = Bounce();

/******************************************************************************
globale Variablen
******************************************************************************/
// Temperaturwert
int32_t RawTemperature = 0;

// Einheit in der die Temperatur angezeigt wird
//uint8_t unit = UNIT_CELSIUS;

// Connect via i2c, default address #0 (A0-A2 not jumpered)
//LiquidCrystal lcd(0);
LiquidCrystal lcd(0);  // set the LCD address to 0x3f for a 16 chars and 2 line display

#define ledrd 13 // Rote LED on board
boolean ledState1s = LOW;             // ledState used to set the LED in 1s-Timer
boolean TimeState = LOW;

void setup() {
  Serial.begin(9600);
  // Wait until a Serial Monitor is connected.
  //while (!Serial);
  Serial.println("Bridge Init starten");
  // Bridge startup
  pinMode(ledrd, OUTPUT);
  digitalWrite(ledrd, LOW);
  Bridge.begin();
  digitalWrite(ledrd, HIGH);
  Serial.println("Bridge Init fertig");

  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%T");
    date.run();
  }

  int tick1SekEvent = t.every(1000, doAll1Sek, (void*)2);
  //Serial.print("1 second tick started id=");
  //Serial.println(tick1SekEvent);

  int tick10SekEvent = t.every(10000, doAll10Sek, (void*)2);
  //Serial.print("10 second tick started id=");
  //Serial.println(tick10SekEvent);

  int tick1MinEvent = t.every(60000, doAll1Min, (void*)2);
  //Serial.print("1 minute tick started id=");
  //Serial.println(tick1MinEvent);

  int tick1HourEvent = t.every(3600000, doAll1Hour, (void*)2); // 1h = 3.600.000 mSek
  //Serial.print("1 hour tick started id=");
  //Serial.println(tick1HourEvent);

  int tick6HourEvent = t.every(21600000, doAll6Hour, (void*)2); // 6h = 21.600.000 mSek
  //Serial.print("6 hour tick started id=");
  //Serial.println(tick6HourEvent);

  Wasserzaehler = ReadCountEEProm(4);
  Wasserzaehler = 0;
  // WriteCountEEProm(Wasserzaehler , 4);
  // EEPromWrite();
  Console.begin();
  Console.println("SETUP finished");


  // set up the LCD's number of rows and columns:
  lcd.begin(20, 4);
  // Print a message to the LCD.
  // lcd.print("hello, world!");

  // I2C-Modul initialisieren
  // Wire.begin();

  // 4-Digit-LED-Anzeige mit I2C-Adresse des SAA1064 initialisieren
  FourDigitLedDisplay.begin(i2cAddressSAA1064);

  // Temperatursensor mit I2C-Adresse des MCP9801 initialisieren
  TemperatureSensor.begin(i2cAddressMCP9801);

  // Temperatursensor auf eine Auflösung von 12 Bit umschalten
  TemperatureSensor.setADCResolution(TemperatureSensor.RESOLUTION_12BIT);

  // Pins für die Taster als Eingänge setzen
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(pinButton4, INPUT);
  pinMode(int4, INPUT);


  // interne Pullup-Widerstände für die Tastereingänge aktivieren
  digitalWrite(pinButton1, HIGH);
  digitalWrite(pinButton2, HIGH);
  digitalWrite(pinButton3, HIGH);
  digitalWrite(pinButton4, HIGH);
  digitalWrite(int4, HIGH);

  // After setting up the button, setup debouncer
  debounceButton1.attach(pinButton1);
  debounceButton1.interval(10);
  debounceButton2.attach(pinButton2);
  debounceButton2.interval(10);
  debounceButton3.attach(pinButton3);
  debounceButton3.interval(10);
  debounceButton4.attach(pinButton4);
  debounceButton4.interval(10);

  attachInterrupt(4, interrupt, RISING);

  wdt_enable(WDTO_8S); // Enable den Watchdog mit 8 Sekunden
}

void loop() {
  wdt_reset(); // Retrigger Watchdog
  //delay(50); // Poll every 50ms
  t.update();
  // Update the debouncer
  debounceButton1.update();
  debounceButton2.update();
  debounceButton3.update();
  debounceButton4.update();

  if (debounceButton4.read() == LOW && BackLightStateChanged == false) {
    BackLightState = !BackLightState;
    BackLightStateChanged = true;
    //Serial.println("Button pressed");
  }
  if (debounceButton4.read() == HIGH) BackLightStateChanged = false;
  lcd.setBacklight(BackLightState);

  //Serial.println("Button ?");
  //Serial.println(debounceBackLight.read());
  //Serial.println(BackLightStateChanged);
}

void WerteAnzeigen()
{
  // Temperaturwert in Grad Celsius vom Sensor lesen
  // (der Temperaturwert ist in Feskommadarstellung mit 4 Nachkommastellen gespeichert)
  RawTemperature = TemperatureSensor.readTemperature();
  // Temperatur anzeigen
  Temperatur = float(RawTemperature) / 10000.0;
  lcd.setCursor(0, 0);
  lcd.print("Temperatur:       °C");
  lcd.setCursor(11, 0);
  FloatToString(Temperatur, 10, StrTemperatur);
  lcd.print(StrTemperatur);
  Console.println(StrTemperatur);
  
  // Wasserzähler anzeigen
  // Wasserzaehler = WasserzaehlerRaw / 450; // 450 Imp/l
  lcd.setCursor(0, 1);
  lcd.print("W.zaehler:         l");
  lcd.setCursor(11, 1);
  lcd.print(Wasserzaehler);
  Console.println(WasserzaehlerRaw);
  Console.println(Wasserzaehler);

  // Wasserfluss anzeigen
  FlussMilli = millis();
  Wasserfluss = float(Wasserzaehler - FlussWasserzaehlerOld) / float(FlussMilli - FlussOldMilli) * 60000.0; // l/min
  FlussWasserzaehlerOld = Wasserzaehler;
  FlussOldMilli = FlussMilli;
  lcd.setCursor(0, 2);
  lcd.print("Wasserfluss:     l/h");
  lcd.setCursor(11, 2);
  FloatToString(Wasserfluss, 10, StrWasserfluss);
  lcd.print(StrWasserfluss);
  Console.println(StrWasserfluss);

  // Druck anzeigen
  Wasserdruck = float(analogRead(ain0) - 205) / 818.0 * 10.0; // 4-20mA -> 1-5V -> 0-10 bar
  lcd.setCursor(0, 3);
  lcd.print("W.Druck:         bar");
  lcd.setCursor(11, 3);
  FloatToString(Wasserdruck, 10, StrWasserdruck);
  lcd.print(StrWasserdruck);
  Console.println(StrWasserdruck);
}

void WerteZurHM()
{
  switch (HMSequence) {
    case 0:
      sprintf(Orderstring, "http://192.168.20.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%s\")", SysVarTemperatur, StrTemperatur);
      if (Temperatur != TemperaturAlt) AenderungSenden = true;
      TemperaturAlt = Temperatur;
      break;
    case 1:
      sprintf(Orderstring, "http://192.168.20.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%d\")", SysVarWasserzaehler, Wasserzaehler);
      if (Wasserzaehler != WasserzaehlerAlt) AenderungSenden = true;
      WasserzaehlerAlt = Wasserzaehler;
      break;
    case 2:
      sprintf(Orderstring, "http://192.168.20.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%s\")", SysVarWasserfluss, StrWasserfluss);
      if (Wasserfluss != WasserflussAlt) AenderungSenden = true;
      WasserflussAlt = Wasserfluss;
     break;
    case 3:
      sprintf(Orderstring, "http://192.168.20.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%s\")", SysVarWasserdruck, StrWasserdruck);
      if (Wasserdruck != WasserdruckAlt) AenderungSenden = true;
      WasserdruckAlt = Wasserdruck;
      break;
  }
  ++HMSequence;
  if (HMSequence > 3) HMSequence = 0;
  //Serial.println(Orderstring);
  Console.print("STATUS AenderungsSenden :");
  Console.println(AenderungSenden);
  if (AenderungSenden == true) {
    HttpClient Hclient;
    Console.println(Orderstring);
    Hclient.get(Orderstring);
    AenderungSenden = false;
  }
}

void interrupt()
{
  ++WasserzaehlerRaw;
  if (WasserzaehlerRaw > 450) {
    ++Wasserzaehler;
    WasserzaehlerRaw = 0; // 450 Imp/l
  }
}

void FloatToString( float val, unsigned int precision, char* Dest) {
  // prints val with number of decimal places determine by precision
  // NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
  // example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)
  // Gibt den String in der globalen char-array variablen Str zurück
  long frac;
  char Teil1[30] = "";
  char Teil2[10] = "";
  if (val >= 0)
    frac = (val - long(val)) * precision;
  else
    frac = (long(val) - val ) * precision;
  ltoa(long(val), Teil1, 10);
  ltoa(frac, Teil2, 10);
  //Serial.println("Teil1");
  //PrintChar(Teil1);
  //Serial.println("Teil2");
  //PrintChar(Teil2);
  strcat(Teil1, ".");
  strcat(Teil1, Teil2);
  strcpy(Dest, Teil1);
}

long ReadCountEEProm(int BaseAddress) {
  long Count = 0;
  int Shift = BaseAddress + 3;
  for (int i = BaseAddress; i < BaseAddress + 4; i++) {
    Count = Count << 8;
    Count = Count | long(EEPROM.read(Shift));
    Shift--;
  }
  return Count;
}

void WriteCountEEProm(long Count, int BaseAddress) {
  int ByteX;
  for (int i = BaseAddress; i < BaseAddress + 4; i++) {
    ByteX = int(Count & 0x000000FF);
    EEPROM.write(i, ByteX);
    Count = Count >> 8;
  }
}

void EEPromWrite(void) {
  Serial.println("EEPROM geschrieben");
  WriteCountEEProm(Wasserzaehler , 4);
}

void doAll1Sek(void* context) {
  //int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 1 second tick: millis()=");
  //Serial.println(millis());
  WerteAnzeigen();
  ledState1s = !ledState1s;
  digitalWrite(ledrd, ledState1s);
  TimeState = !TimeState;
  if (TimeState) {
    FourDigitLedDisplay.writeDecimal(hours * 100 + minutes, 2 );
  }
  else {
    FourDigitLedDisplay.writeDecimal(hours * 100 + minutes, -1 );
  }
}

void doAll10Sek(void* context) {
  int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 10 second tick: millis()=");
  //Serial.println(millis());
  WerteZurHM();
}

void doAll1Min(void* context) {
  int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 1 minute tick: millis()=");
  //Serial.println(millis());
  // restart the date process:
  if (!date.running())  {
    date.begin("date");
    date.addParameter("+%T");
    date.run();
  }
  //if there's a result from the date process, parse it:
  while (date.available() > 0) {
    // get the result of the date process (should be hh:mm:ss):
    String timeString = date.readString();
    Serial.println(timeString);

    // find the colons:
    int firstColon = timeString.indexOf(":");
    int secondColon = timeString.lastIndexOf(":");

    // get the substrings for hour, minute second:
    String hourString = timeString.substring(0, firstColon);
    String minString = timeString.substring(firstColon + 1, secondColon);
    String secString = timeString.substring(secondColon + 1);

    // convert to ints,saving the previous second:
    hours = hourString.toInt();
    minutes = minString.toInt();
    seconds = secString.toInt();
  }

}

void doAll1Hour(void* context) {
  int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 1 hour tick: millis()=");
  //Serial.println(millis());
  if (Wasserzaehler != WasserzaehlerOld) {
    EEPromWrite();
    WasserzaehlerOld = Wasserzaehler;
  }
}

void doAll6Hour(void* context) {
  int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 6 hour tick: millis()=");
  //Serial.println(millis());
}

