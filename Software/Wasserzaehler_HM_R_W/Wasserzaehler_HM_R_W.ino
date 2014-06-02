// Possible commands are listed here:
//
// "digital/13"     -> digitalRead(13)
// "digital/13/1"   -> digitalWrite(13, HIGH)
// "analog/2/123"   -> analogWrite(2, 123)
// "analog/2"       -> analogRead(2)
// "mode/13/input"  -> pinMode(13, INPUT)
// "mode/13/output" -> pinMode(13, OUTPUT)
/*
When the REST password is turned off, you can use a browser with the following URL structure :
http://YUNms.local/arduino/digital/13 : calls digitalRead(13);
http://YUNms.local/arduino/digital/13/1 : calls digitalWrite(13,1);
http://YUNms.local/arduino/digital/13/0 : calls digitalWrite(13,0);
http://YUNms.local/arduino/analog/9/123 : analogWrite(9,123);
http://YUNms.local/arduino/analog/2 : analogRead(2);
http://YUNms.local/arduino/mode/13/input : pinMode(13, INPUT);
http://YUNms.local/arduino/mode/13/output : pinMode(13, OUTPUT);
You can use the curl command from the command line instead of a browser if you prefer.
*/

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <HttpClient.h>
#include <EEPROM.h>
#include "Timer.h"
#include <Console.h>

// Listen on default port 5555, the webserver on the Yun
// will forward there all the HTTP requests for us.
YunServer server;

boolean Status = false;
char Orderstring[255];
char SysVarTemperatur[] = "Temperatur";
char SysVarWasserzaehler[] = "Wasserzaehler";
float Temperatur = 0.0;
unsigned long Wasserzaehler = 0;
unsigned long WasserzaehlerOld = 0;
unsigned long OldMilli = 0;

int ain0 = 0;

Timer t;

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

int BackLightState = LOW;             // BackLightState used to set the BackLight

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
  Serial.begin(9600);
  // Wait until a Serial Monitor is connected.
  //while (!Serial);
  Serial.println("Bridge Init starten");
  // Bridge startup
  pinMode(ledrd,OUTPUT);
  digitalWrite(ledrd, LOW);
  Bridge.begin();
  digitalWrite(ledrd, HIGH);
  Serial.println("Bridge Init fertig");

  // Listen for incoming connection only from localhost
  // (no one from the external network could connect)
  server.listenOnLocalhost();
  server.begin();
  
    // initialize the digital pin as an output.

  int tick1SekEvent = t.every(1000, doAll1Sek, (void*)2);
  Serial.print("1 second tick started id=");
  Serial.println(tick1SekEvent);

  int tick10SekEvent = t.every(10000, doAll10Sek, (void*)2);
  Serial.print("10 second tick started id=");
  Serial.println(tick10SekEvent);

  int tick1MinEvent = t.every(60000, doAll1Min, (void*)2);
  Serial.print("1 minute tick started id=");
  Serial.println(tick1MinEvent);

  int tick1HourEvent = t.every(3600000, doAll1Hour, (void*)2); // 1h = 3.600.000 mSek
  Serial.print("1 hour tick started id=");
  Serial.println(tick1HourEvent);

  int tick6HourEvent = t.every(21600000, doAll6Hour, (void*)2); // 6h = 21.600.000 mSek
  Serial.print("6 hour tick started id=");
  Serial.println(tick6HourEvent);

  Wasserzaehler = ReadCountEEProm(0);

  Console.begin(); 

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
  
  //pinMode(ledrd,OUTPUT);
}

void loop() {
  //Serial.println();
  //Serial.println("Hier startet loop()");
  //TimeStamp("Yclient");
  // Get clients coming from server
  YunClient Yclient = server.accept();
  //TimeStamp("accept");
  // There is a new client?
  if (Yclient) {
    // Process request
    process(Yclient);
    // Close connection and free resources.
    Yclient.stop();
  }
  //TimeStamp("Stop");
  delay(50); // Poll every 50ms
  t.update();
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);

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
  
}


void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');

  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
  }

  // is "analog" command?
  if (command == "analog") {
    analogCommand(client);
  }

  // is "mode" command?
  if (command == "mode") {
    modeCommand(client);
  }
}

void digitalCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } 
  else {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);

  // Update datastore key with the current pin value
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));
}

void analogCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/analog/5/120"
  if (client.read() == '/') {
    // Read value and execute command
    value = client.parseInt();
    analogWrite(pin, value);

    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    // Read analog pin
    value = analogRead(pin);

    // Send feedback to client
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);

    // Update datastore key with the current pin value
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void modeCommand(YunClient client) {
  int pin;

  // Read pin number
  pin = client.parseInt();

  // If the next character is not a '/' we have a malformed URL
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }

  String mode = client.readStringUntil('\r');

  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }

  if (mode == "output") {
    pinMode(pin, OUTPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }

  client.print(F("error: invalid mode "));
  client.print(mode);
}

void FloatToString( float val, unsigned int precision, char* Dest){
  // prints val with number of decimal places determine by precision
  // NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
  // example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)
  // Gibt den String in der globalen char-array variablen Str zurück
  long frac;
  char Teil1[30] = "";
  char Teil2[10] = "";
  if(val >= 0)
    frac = (val - long(val)) * precision;
  else
    frac = (long(val)- val ) * precision;
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


void TimeStamp(char* Str) {
  unsigned long Milli = millis();
  Serial.print(Str);
  Serial.print(" Differenzzeit [Sek] : ");
  Serial.println(Milli - OldMilli);
  OldMilli = Milli;
}

long ReadCountEEProm(int BaseAddress){
  long Count = 0;
  int Shift = BaseAddress + 3;
  for (int i = BaseAddress; i < BaseAddress+4; i++) {
    Count = Count << 8;
    Count = Count | long(EEPROM.read(Shift));
    Shift--;
  }
  return Count;
}

void WriteCountEEProm(long Count, int BaseAddress){
  int ByteX;
  for (int i = BaseAddress; i < BaseAddress+4; i++){
    ByteX = int(Count & 0x000000FF);
    EEPROM.write(i, ByteX);
    Count = Count >> 8;
  }
}

void EEPromWrite(void){
  Serial.println("EEPROM geschrieben");
  WriteCountEEProm(Wasserzaehler , 0);
}

void doAll1Sek(void* context) {
  //int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 1 second tick: millis()=");
  //Serial.println(millis());
  // set the LED with the ledState of the variable:
  if (ledStateLoop == LOW)
    ledStateLoop = HIGH;
  else
    ledStateLoop = LOW;
  digitalWrite(ledrd, ledStateLoop);
  Serial.print("Poti :" );
  Serial.println(analogRead(ain0));
  Wasserzaehler += long(analogRead(ain0)); 
}

void doAll10Sek(void* context) {
  int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 10 second tick: millis()=");
  //Serial.println(millis());
  HttpClient Hclient;
  TimeStamp("Hclient");
  //char Str1[20];
  //FloatToString(Temperatur, 100, Str1);
  //sprintf(Orderstring,"http://192.168.11.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%s\")",SysVarTemperatur, Str1);
  sprintf(Orderstring,"http://192.168.11.220:8181/do.exe?r1=dom.GetObject(\"%s\").State(\"%d\")",SysVarWasserzaehler, Wasserzaehler);
  Serial.println(Orderstring);
  Console.println(Orderstring);
  //Serial.println(Temperatur);
  //Temperatur = Temperatur + 0.12;
  TimeStamp("Temp");
  Hclient.get(Orderstring);
  TimeStamp("Orderstring");
}

void doAll1Min(void* context) {
  int time = (int)context;
  //Serial.print(time);
  //Serial.print(" 1 minute tick: millis()=");
  //Serial.println(millis());
    if (BackLightState == LOW)
    BackLightState = HIGH;
  else
    BackLightState = LOW;
  lcd.setBacklight(BackLightState);
}

void doAll1Hour(void* context) {
  int time = (int)context;
  Serial.print(time);
  Serial.print(" 1 hour tick: millis()=");
  Serial.println(millis());
  if (Wasserzaehler != WasserzaehlerOld) {
    EEPromWrite();
    WasserzaehlerOld = Wasserzaehler;
  }
}

void doAll6Hour(void* context) {
  int time = (int)context;
  Serial.print(time);
  Serial.print(" 6 hour tick: millis()=");
  Serial.println(millis());
}

