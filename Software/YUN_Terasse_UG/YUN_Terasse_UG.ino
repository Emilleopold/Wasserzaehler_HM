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
char Orderstring[255];
char Str10[10];
char SysVarWasserzaehlerHecke[] = "Wasser_Hecke";
char SysVarWasserflussHecke[] = "Wasserfluss_Hecke";
char SysVarWasserzaehlerRasen[] = "Wasser_Rasen";
char SysVarWasserflussRasen[] = "Wasserfluss_Rasen";
char SysVarWasserdruck[] = "Druck_Leitung";
char StrWasserflussHecke[10] = "";
char StrWasserflussRasen[10] = "";
char StrWasserdruck[10] = "";
float WasserflussHecke = 0.0;
float WasserflussRasen = 0.0;
float Wasserdruck = 0.0;
unsigned long WasserzaehlerHecke = 0;
unsigned long WasserzaehlerRawHecke = 0;
unsigned long WasserzaehlerOldHecke = 0;
unsigned long WasserzaehlerRasen = 0;
unsigned long WasserzaehlerRawRasen = 0;
unsigned long WasserzaehlerOldRasen = 0;
unsigned long OldMilli = 0;
unsigned long FlussWasserzaehlerOldHecke = 0;
unsigned long FlussWasserzaehlerOldRasen = 0;
unsigned long FlussMilli = 0;
unsigned long FlussOldMilli = 0;
int HMSequence = 0;

#define ain0 A0
#define int0 3
#define int1 2

Timer t;

Process date;
int hours, minutes, seconds;

/*
  LiquidCrystal Library - display() and noDisplay()
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD and uses the 
 display() and noDisplay() functions to turn on and off
 the display.
 
 The circuit:
 * LCD RS pin to digital pin 12 -> 8
 * LCD RW pin to digital pin 9
 * LCD Enable pin to digital pin 11 -> 10
 * LCD D4 pin to digital pin 5 -> 4
 * LCD D5 pin to digital pin 4 -> 5
 * LCD D6 pin to digital pin 3 -> 6
 * LCD D7 pin to digital pin 2 -> 7
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

*/
// include the library code:
#include <Wire.h>
#include <LiquidCrystal.h>

boolean BackLightState = LOW;             // BackLightState used to set the BackLight
boolean BackLightStateChanged = false;             // BackLightState used to set the BackLight


// Pinbelegung der Tasten zur Auswahl der Einheit
#define pinButton1 12
#define pinButton2 11
#define pinButton3 A4

#include "Bounce2.h"
// Instantiate a Bounce object
Bounce debounceButton1 = Bounce();
Bounce debounceButton2 = Bounce();
Bounce debounceButton3 = Bounce();

/******************************************************************************
globale Variablen
******************************************************************************/

// initialize the library with the numbers of the interface pins
// LiquidCrystal(rs, rw, enable, d4, d5, d6, d7) 
LiquidCrystal lcd(8, 9, 10, 4, 5, 6, 7);

#define ledrd 13 // Rote LED on board
#define ledButton1 A1 // Grüne LED in Taste 1 (A1)
#define ledButton2 A2 // Grüne LED in Taste 2 (A2)
#define ledButton3 A3 // Grüne LED in Taste 3 (A3)
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

  WasserzaehlerRawHecke = ReadCountEEProm(0);
  //WasserzaehlerRawHecke = 0;
  WasserzaehlerRawRasen = ReadCountEEProm(4);
  //WasserzaehlerRawRasen = 0;
  //EEPromWrite();
  Console.begin();
  Console.println("SETUP finished");


  // set up the LCD's number of rows and columns:
  lcd.begin(20, 4);

  // Pins für die LED als AusEgänge setzen
  pinMode(ledButton1, OUTPUT);
  digitalWrite(ledButton1, LOW);
  pinMode(ledButton2, OUTPUT);
  digitalWrite(ledButton2, LOW);
  pinMode(ledButton3, OUTPUT);
  digitalWrite(ledButton3, LOW);

  // Pins für die Taster als Eingänge setzen
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(int0, INPUT);
  pinMode(int1, INPUT);


  // interne Pullup-Widerstände für die Tastereingänge aktivieren
  digitalWrite(pinButton1, HIGH);
  digitalWrite(pinButton2, HIGH);
  digitalWrite(pinButton3, HIGH);
  digitalWrite(int0, HIGH);
  digitalWrite(int1, HIGH);

  // After setting up the button, setup debouncer
  debounceButton1.attach(pinButton1);
  debounceButton1.interval(10);
  debounceButton2.attach(pinButton2);
  debounceButton2.interval(10);
  debounceButton3.attach(pinButton3);
  debounceButton3.interval(10);

  attachInterrupt(0, interrupt0, RISING); // Wasserzähler Hecke
  attachInterrupt(1, interrupt1, RISING); // Wasserzähler Rasen

  //wdt_enable(WDTO_8S); // Enable den Watchdog mit 8 Sekunden
}

void loop() {
  wdt_reset(); // Retrigger Watchdog
  //delay(50); // Poll every 50ms
  t.update();
  // Update the debouncer
  debounceButton1.update();
  debounceButton2.update();
  debounceButton3.update();

  if (debounceButton3.read() == LOW && BackLightStateChanged == false) {
    BackLightState = !BackLightState;
    BackLightStateChanged = true;
    //Serial.println("Button pressed");
  }
  if (debounceButton3.read() == HIGH) BackLightStateChanged = false;
  lcd.setBacklight(BackLightState);
}

void WerteAnzeigen()
{
  // WasserzählerHecke anzeigen
  WasserzaehlerHecke = WasserzaehlerRawHecke / 450; // 450 Imp/l
  lcd.setCursor(0, 0);
  lcd.print("W.zaehlerH:        l");
  lcd.setCursor(11, 0);
  lcd.print(WasserzaehlerHecke);
  Console.println(WasserzaehlerRawHecke);
  Console.println(WasserzaehlerHecke);

  
  // WasserzählerRasen anzeigen
  WasserzaehlerRasen = WasserzaehlerRawRasen / 450; // 450 Imp/l
  lcd.setCursor(0, 1);
  lcd.print("W.zaehlerR:        l");
  lcd.setCursor(11, 1);
  lcd.print(WasserzaehlerRasen);
  Console.println(WasserzaehlerRawRasen);
  Console.println(WasserzaehlerRasen);

  // Wasserfluss anzeigen
  FlussMilli = millis();
  WasserflussHecke = float(WasserzaehlerHecke - FlussWasserzaehlerOldHecke) / float(FlussMilli - FlussOldMilli) * 60000.0; // l/min
  FlussWasserzaehlerOldHecke = WasserzaehlerHecke;
  WasserflussRasen = float(WasserzaehlerRasen - FlussWasserzaehlerOldRasen) / float(FlussMilli - FlussOldMilli) * 60000.0; // l/min
  FlussWasserzaehlerOldRasen = WasserzaehlerRasen;
  FlussOldMilli = FlussMilli;
  lcd.setCursor(0, 2);
  lcd.print("WasserflussH:    l/h");
  lcd.setCursor(11, 2);
  FloatToString(WasserflussHecke, 10, StrWasserflussHecke);
  lcd.print(StrWasserflussHecke);
  Console.println(StrWasserflussHecke);

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
  HttpClient Hclient;
  switch (HMSequence) {
    case 0:
      sprintf(Orderstring, "http://192.168.11.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%d\")", SysVarWasserzaehlerHecke, WasserzaehlerHecke);
      break;
    case 1:
      sprintf(Orderstring, "http://192.168.11.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%s\")", SysVarWasserflussHecke, StrWasserflussHecke);
      break;
    case 2:
      sprintf(Orderstring, "http://192.168.11.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%d\")", SysVarWasserzaehlerRasen, WasserzaehlerRasen);
      break;
    case 3:
      sprintf(Orderstring, "http://192.168.11.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%s\")", SysVarWasserflussRasen, StrWasserflussRasen);
      break;
    case 4:
      sprintf(Orderstring, "http://192.168.11.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%s\")", SysVarWasserdruck, StrWasserdruck);
      break;
  }
  ++HMSequence;
  if (HMSequence > 4) HMSequence = 0;
  //Serial.println(Orderstring);
  Console.println(Orderstring);
  Hclient.get(Orderstring);
}

void interrupt0()
{
  ++WasserzaehlerRawHecke;
}

void interrupt1()
{
  ++WasserzaehlerRawRasen;
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
  WriteCountEEProm(WasserzaehlerRawHecke , 0);
  WriteCountEEProm(WasserzaehlerRawRasen , 4);
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
  digitalWrite(ledButton3, TimeState);
  Serial.println(debounceButton1.read());
  Serial.println(debounceButton2.read());
  Serial.println(debounceButton3.read());
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
  if ((WasserzaehlerHecke != WasserzaehlerOldHecke) || (WasserzaehlerRasen != WasserzaehlerOldRasen)) {
    EEPromWrite();
    WasserzaehlerOldHecke = WasserzaehlerHecke;
    WasserzaehlerOldRasen = WasserzaehlerRasen;
  }
}

void doAll6Hour(void* context) {
  int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 6 hour tick: millis()=");
  //Serial.println(millis());
}

