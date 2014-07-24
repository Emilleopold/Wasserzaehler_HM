//For ARDUINO 1.0+ 
const String ver = "homeduino_5.ino / Stand: 2014.07.09"; // Versionsbezeichner
//Verfasser: Eugen Stall
//Arduino Board Mega 2560
//Quellen: 
// >> libraries "onewire.h"  und "DallasTemperature.h" von hier http://www.hacktronics.com/Tutorials/arduino-1-wire-tutorial.html
// in das Arduino-Verzeichnis "libraries" speichern
// diese Software erlaubt die Steuerung der Pinfunktionen mit einfachen Browserbefehlen

// 20140724ms
// Es gibt bei dem Client.print noch ein Problem mit der stringwandlung von float
// Temporär durch (int)  type casting gelöst

#include <Ethernet.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RCSwitch.h>
#include <IRremote.h>
 
////////////////////////////////////////////////////////////////////////
//Netzwerk-Konfiguration muss individuell hier eingegeben werden
byte ip[] = { 192, 168, 178, 58 };      //das ist die IP des Arduino 
byte gateway[] = { 192, 168, 178, 1 };  //das ist die IP des Routers
byte subnet[] = { 255, 255, 255, 0 };   //wie immer
byte mac[] = { 0xDE, 0xAF, 0xEE, 0xEF, 0xEE, 0xDE }; //nur 1x im Netz
EthernetServer server = EthernetServer(80);   //port 80
 
//Variablendefinitionen:
boolean reading = false;
boolean valid_command = false;
String command = String(200); // string for fetching data from address
String befehl = String(20);
String parameter = String(20);
String header = String(20);
 
String ip_adresse = String(15);
int param;
long param1;
int param2;
String message = String(60);
boolean test = false;
unsigned long currentMillis;
 
float tempNTC;
float B_wert = 3950; //aus dem Datenblatt des NTC
float Tn = 298.15;  //25°Celsius in °Kelvin
float Rv = 10000;  //Vorwiderstand
float Rn = 10000;  //NTC-Widerstand bei 25°C
float Rt ; 
 
int data[10];
int datum;
 
int PWM;
 
int onewire_pin;
float temp_tur;
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
 
void setup()
{ //Serial.begin(9600);
 
 
  //Pins 10,11,12 & 13 fuer ethernet shield verwendet
 
  Ethernet.begin(mac, ip, gateway, subnet); 
  server.begin();
  ip_adresse = String(ip[0]) + "." + String(ip[1]) + "." +String(ip[2]) + "." + String(ip[3]);
  header = "arduino IP: " + ip_adresse + "\n\r";
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void loop()
{ command = ""; //String(100);
  EthernetClient client = server.available();
  if (client) 
    { // an http request ends with a blank line
       boolean currentLineIsBlank = true;
       while (client.connected())
         { if (client.available()) 
            { char c = client.read();
                if (reading && c == ' ') reading =false;
                if (c == '?') reading = true;  // beginn der Befehlssequenz 
                if (reading) 
                 { //read char by char HTTP request
                   if (command.length() < 100) 
                     { //store characters to string
                       command = command + c;
                     }
                  }   
                 if (c == '\n' && currentLineIsBlank)  break;
                 if (c == '\n') 
                  { currentLineIsBlank = true;} else if (c != '\r') 
                     { currentLineIsBlank = false;}
              }
          }
////////////////////////////////////////////////////////////////////////
pinMode(13, OUTPUT);    //lED leuchtet während der Abarbeitung der Befehle
digitalWrite(13, HIGH);
 
//jetzt wird das command ausgeführt 
// befehl herausmaskieren
int colonPosition = command.indexOf(':');
befehl = command.substring(1,colonPosition);
command = command.substring((colonPosition+1));   //Rest-command bilden
 
client.print(header);
valid_command = false;
//###########################################################################
if (befehl == "setpin") 
{ while (command.length() > 1 ) 
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
     pinMode(param, OUTPUT);
     digitalWrite(param, HIGH);
     client.print("Pin D" + parameter + " = 1 port is digital output E\n\r"); 
  }
}
//###########################################################################
if (befehl == "resetpin") 
{ while (command.length() > 1 ) 
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
     pinMode(param, OUTPUT);
     digitalWrite(param, LOW);
     client.print("Pin D" + parameter + " = 0 port is digital output E\n\r"); 
  }
}
//###########################################################################
if (befehl == "digitalin") 
{ while (command.length() > 1 ) 
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
     pinMode(param, INPUT);
     digitalWrite(param, HIGH);
     client.print("Pin D" + parameter + " = " + digitalRead(param) + "  port is digital input E\n\r");
  }
}
//###########################################################################
if (befehl == "analogin") 
{ while (command.length() > 1 ) 
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
     client.print("Pin A" + parameter + " = " + analogRead(param) + " port is analog input E\n\r");
   }
}
//###########################################################################
if (befehl == "pwm_out") 
{    valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false;}; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
     PWM = command.toInt();
     pinMode(param, OUTPUT);
     analogWrite(param, PWM); 
     client.print("PWM D" + parameter + " = " + PWM + "% duty cycle at output E\n\r");
 
}
//###########################################################################
if (befehl == "ir_send")   
{ valid_command = true; 
  colonPosition = command.indexOf(':');
  parameter = command.substring(0,colonPosition);
  command = command.substring((colonPosition+1));
  param = parameter.toInt(); 
  colonPosition = command.indexOf(':');
  parameter = command.substring(0,colonPosition);
  param1 = parameter.toInt(); 
  parameter = command.substring((colonPosition+1));
  param2 = parameter.toInt(); 
// http://arcfn.com
      IRsend irsend;
      irsend.sendSony(param1, param2); // Sony TV power code
      delay(40);
 
  client.print("ir_send mit Sendecode :" + command + " \n\r"); 
} 
//###########################################################################
if (befehl == "ir_receive") 
{ while (command.length() > 1 ) 
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
//Quelle:  http://arcfn.com
IRrecv irrecv(param);
decode_results results;
irrecv.enableIRIn(); // Start the receiver
unsigned long time_rf0 = millis();
unsigned long time_rf1 = millis();
while ((time_rf1 - time_rf0) < 5000)
  { time_rf1 = millis();
  if (irrecv.decode(&results))
    { client.print(results.value, DEC);
      client.print(" \n\r");
      irrecv.resume();} // Receive the next value
    }
  client.print("ende ir_receive :" + command + " \n\r"); 
} 
}
//###########################################################################
if (befehl == "rf_send")   
{ valid_command = true; 
  colonPosition = command.indexOf(':');
  parameter = command.substring(0,colonPosition);
  command = command.substring((colonPosition+1));
  param = parameter.toInt(); 
  colonPosition = command.indexOf(':');
  parameter = command.substring(0,colonPosition);
  param1 = parameter.toInt(); 
  parameter = command.substring((colonPosition+1));
  param2 = parameter.toInt(); 
  // Quelle: http://code.google.com/p/rc-switch/
  RCSwitch mySwitch = RCSwitch();
  mySwitch.enableTransmit(param);
  // Optional set pulse length.
  // mySwitch.setPulseLength(320);
  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);
  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
 
  mySwitch.send(param1, param2);
  client.print("rf_send mit Sendecode :" + command + " \n\r"); 
} 
//###########################################################################
if (befehl == "rf_receive")  //  http://code.google.com/p/rc-switch/ 
{ valid_command = true; 
  colonPosition = command.indexOf(':');
  parameter = command.substring(0,colonPosition);
  command = command.substring((colonPosition+1));
  param = parameter.toInt(); 
  param = param - 2;  // nur Pin D2 und D3 sind zulässig
  RCSwitch mySwitch = RCSwitch();
  mySwitch.enableReceive(param);  //receiver on interrupt 0 or 1 => that is pin D2 or D3
 
unsigned long time_rf0 = millis();
unsigned long time_rf1 = millis();
while ((time_rf1 - time_rf0) < 3000)
{ time_rf1 = millis();
  if (mySwitch.available()) 
   {int value = mySwitch.getReceivedValue();
    if (value == 0) 
      {client.print("Unknown encoding");} 
      else 
         {client.print("Pin D2 received Code : ");
          client.print (mySwitch.getReceivedValue() );
          client.print (" / ");
          client.print( mySwitch.getReceivedBitlength() );
          client.print("bit Protocol: ");
          client.println( mySwitch.getReceivedProtocol() + " \n\r" );
         }
    mySwitch.resetAvailable();
  }
}
  client.print("ende rf_receive :" + command + " \n\r"); 
} 
//###########################################################################      
if (befehl == "onewire") 
{ valid_command = true;  
  colonPosition = command.indexOf(':');
  if (colonPosition > 2) {valid_command = false; }; //kanalnummern max 2 stellen erlaubt
  parameter = command.substring(0,colonPosition);
  command = command.substring((colonPosition+1));
  onewire_pin = parameter.toInt();
//Setup onewire//////////////////////////////////////////////////////////////////////
//long intervall_onewire =10000; //onewire-Operation alle 10sec
//long lastTime_onewire =0;
float tempC[10];   // 10 Onewire-Sensoren werden maximal verwendet
 
OneWire oneWire(onewire_pin);  //Sensoren auf Pin D2 >>   hier ändern, wenn anderer Port verwendet wird
DallasTemperature sensors(&oneWire);
DeviceAddress TempSensor[] = //Adress-Array definieren
{ { 0x10, 0xC7, 0xBA, 0xDD, 0x01, 0x08, 0x00, 0x52 },
  { 0x10, 0x8E, 0xB8, 0xDD, 0x01, 0x08, 0x00, 0x32 },
  { 0x10, 0xC7, 0xBA, 0xDD, 0x01, 0x08, 0x00, 0x52 },
  { 0x10, 0x8E, 0xB8, 0xDD, 0x01, 0x08, 0x00, 0x32 },
  { 0x10, 0xC7, 0xBA, 0xDD, 0x01, 0x08, 0x00, 0x52 },
  { 0x10, 0x8E, 0xB8, 0xDD, 0x01, 0x08, 0x00, 0x32 },
  { 0x10, 0xC7, 0xBA, 0xDD, 0x01, 0x08, 0x00, 0x52 },
  { 0x10, 0x8E, 0xB8, 0xDD, 0x01, 0x08, 0x00, 0x32 },
  { 0x10, 0xC7, 0xBA, 0xDD, 0x01, 0x08, 0x00, 0x52 },
  { 0x10, 0x8E, 0xB8, 0xDD, 0x01, 0x08, 0x00, 0x32 },
};
  // Start up the library
  sensors.begin();
  // aufloesung 10 bit
  sensors.setResolution(TempSensor[0], 10);
  sensors.setResolution(TempSensor[1], 10);
  sensors.setResolution(TempSensor[2], 10);
  sensors.setResolution(TempSensor[3], 10);
  sensors.setResolution(TempSensor[4], 10);
  sensors.setResolution(TempSensor[5], 10); 
  sensors.setResolution(TempSensor[6], 10);
  sensors.setResolution(TempSensor[7], 10);
  sensors.setResolution(TempSensor[8], 10);
  sensors.setResolution(TempSensor[9], 10);
//Setup onewire//////////////////////////////////////////////////////////////////////
 
 while (command.length() > 1 ) 
 { colonPosition = command.indexOf(':');
   if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
   parameter = command.substring(0,colonPosition);
   command = command.substring((colonPosition+1));
   param = parameter.toInt(); 
   sensors.requestTemperatures();
   tempC[param] = sensors.getTempC(TempSensor[param]);
   //ms client.print("onewire T" + parameter + " = " + tempC[param] + " Celsius / pin D" + onewire_pin + " as input  \n\r");
   client.print("onewire T" + parameter + " = " +  (int) tempC[param] + " Celsius / pin D" + onewire_pin + " as input  \n\r");
  }   
} 
//###########################################################################
if (befehl == "1wire")           ///   Unterprogramm fuer 1wire Abfrage von einzelnen Sensoren
{ while (command.length() > 1 )  // verwendete Quelle: http://tushev.org/articles/arduino/item/52-how-it-works-ds18b20-and-arduino
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
 
     OneWire ds(param);  
     #define DS18S20_ID 0x10
     #define DS18B20_ID 0x28  
     byte i;
     byte present = 0;
     byte data[12];
     byte addr[8];
     if (!ds.search(addr)) { ds.reset_search(); temp_tur = -1000; } //find a device
     if (OneWire::crc8( addr, 7) != addr[7]) {temp_tur = -1000; }
     if (addr[0] != DS18S20_ID && addr[0] != DS18B20_ID) {temp_tur = -1000;}
     if (temp_tur > -1000) 
       { ds.reset();     
         ds.select(addr); 
         ds.write(0x44, 1);    // Start conversion
         delay(850);           // Wait some time...
         present = ds.reset();  
         ds.select(addr);
         ds.write(0xBE);        // Issue Read scratchpad command
         for ( i = 0; i < 9; i++) { data[i] = ds.read(); }    // Receive 9 bytes
         temp_tur = ( (data[1] << 8) + data[0] )*0.0625;      // Calculate temperature value
       }
     //ms client.print("1wire  T" + parameter + " = " + temp_tur + " Celsius / pin D" + param + " as input  \n\r");
     client.print("1wire  T" + parameter + " = " + (int) temp_tur + " Celsius / pin D" + param + " as input  \n\r");
   }
}
//###########################################################################
if (befehl == "1wire_address")           ///   Unterprogramm fuer Auslesen der Sensoradresse
{ while (command.length() > 1 )  // verwendete Quelle:http://fluuux.de/2012/09/arduino-adressen-aller-ds1820-ermitteln/
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
 
     OneWire ds(param);
     byte address[8];
     int i=0;
     byte ok = 0, tmp = 0;
     client.print("--Suche gestartet--\n\r");
     while (ds.search(address))
     {tmp = 0;    
      if (address[0] == 0x10) { client.print("Device is a DS18S20 : "); tmp = 1;}  //0x10 = DS18S20
            else { if (address[0] == 0x28) { client.print("Device is a DS18B20 : "); tmp = 1; } //0x28 = DS18B20
     }
     if (tmp == 1) //display the address, if tmp is ok    
      {if (OneWire::crc8(address, 7) != address[7]) { client.print("but it doesn't have a valid CRC!\n\r"); }
         else {  for (i=0;i<8;i++)    //all is ok, display it
                 { if (address[i] < 9) { client.print("0");}
                   client.print("0x");
                   client.print(address[i],HEX);
                   if (i<7) { client.print(", ");}
                 }
                 client.print("\n\r");
                 ok = 1;
              }
      }//end if tmp
     }//end while
     if (ok == 0){client.print("Keine Sensoren gefunden\n\r"); }
     client.print("--Suche beendet--\n\r");
  }
}    
//###########################################################################
if (befehl == "w_data")  
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
     datum = command.toInt();
     data[param] = datum;
     client.print("w_data" + parameter + " = " + datum + " set w_data        E\n\r");
}
//###########################################################################
if (befehl == "r_data")    
{ while (command.length() > 1 ) 
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false;break;}; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
     client.print("r_data" + parameter + " = " + data[param]+ "  Wert der Variablen   E\n\r");
   }
}
//###########################################################################
if (befehl == "dist")    // messung mit ultraschallsensor an "beliebigen pin
{ while (command.length() > 1 ) 
  {  valid_command = true; 
     colonPosition = command.indexOf(':');
     if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
     parameter = command.substring(0,colonPosition);
     command = command.substring((colonPosition+1));
     param = parameter.toInt(); 
 
     long duration, cm;  
     pinMode(param, OUTPUT);
     digitalWrite(param, LOW);
     delayMicroseconds(2);
     digitalWrite(param, HIGH);
     delayMicroseconds(5);
     digitalWrite(param, LOW);
     pinMode(param, INPUT);
     duration = pulseIn(param, HIGH);
     cm = duration /29/2; 
     delay(100);
 
     client.print("ultrasonic an Pin D" + parameter + " = " + cm + " cm                    E\n\r");
  }
}
//###########################################################################
if (befehl == "w_msg")     
{ message = command;
  valid_command = true; 
  client.print("w_msg :" + message + "\n\r");
}  
//###########################################################################
if (befehl == "r_msg")     
{ valid_command = true; 
  client.print("r_msg :" + message + "\n\r");
}  
//###########################################################################
if (befehl == "ntc_in") 
{ while (command.length() > 1 ) 
  { valid_command = true; 
    colonPosition = command.indexOf(':');
    if (colonPosition > 2) {valid_command = false; break; }; //kanalnummern max 2 stellen erlaubt
    parameter = command.substring(0,colonPosition);
    command = command.substring((colonPosition+1));
    param = parameter.toInt();
 
    Rt = Rv/((1024/analogRead(param))- 1);
    tempNTC = (B_wert * Tn / ( B_wert + (Tn * log(Rt/Rn)))) -Tn ;
    //ms client.print("NTC an A" + parameter + " = " + tempNTC + " gradCelsius          E\n\r");
    client.print("NTC an A" + parameter + " = " + (int) tempNTC + " gradCelsius          E\n\r");
  }   
}  
//###########################################################################
if (befehl == "help")     
{  valid_command = true; 
   client.print("IP-Adresse/?Befehl:Param1:Param2: ...\n\r");
   client.print( 
     ip_adresse + "/?setpin:2:3:           >> Digitalpins D2,D3 auf HIGH setzen und Pins als Output setzen\n\r" 
   + ip_adresse + "/?resetpin:2:4:3:       >> Digitalpins D2,D4,D3 auf LOW setzen und Pinsals Output setzen\n\r" 
   + ip_adresse + "/?digitalin:2:4:        >> Digitalpins D2,D4 lesen und Pins als Input setzen \n\r"
   + ip_adresse + "/?analogin:0:5:         >> Analogpins A0,A5 lesen und Pins auf Analoginput setzen\n\r");
   client.print( 
     ip_adresse + "/?pwm_out:4:96:         >> PWM-Signal mit Tastverhältnis 96% an Digitalpin D4 ausgeben\n\r"
   + ip_adresse + "/?rf_send:2:4523029:24: >> An Digitalpin D2 das RF-Telegramm 4523029 mit 24bit-Kodierung  ausgeben\n\r"
   + ip_adresse + "/?rf_receive:2:         >> An Digitalpin D2 auf rf-Signal 3s warten und RF-Telegramm ausgeben\n\r"
   + ip_adresse + "/?ir_send:3:1086136543: >> An Digitalpin D3 das IR-Telegramm 1086136543 ausgeben\n\r");
 
   client.print( 
     ip_adresse + "/?ir_receive:3:         >> An Digitalpin D3 auf IR-Signal 3s warten und IR-Telegramm ausgeben\n\r"
   + ip_adresse + "/?onewire:2:0:5:        >> An Digitalpin D2 die 1-Wire Sensoren Nr.0 und Nr.5 einlesen\n\r"
   + ip_adresse + "/?1wire:3:              >> An Digitalpin D3 den 1-Wire Sensor einlesen\n\r"
   + ip_adresse + "/?1wire_address:3:      >> An Digitalpin D3 die Adresse des 1-Wire Sensors auslesen\n\r");
   client.print( 
     ip_adresse + "/?w_data:0:1425:        >> Arduino-Integervariablen \"w_data[0]\" mit Wert 1525 setzen \n\r"
   + ip_adresse + "/?r_data:4:5:           >> Arduino-Integervariablen \"w_data[4]\" und \"w_data[5]\" auslesen \n\r"
   + ip_adresse + "/?dist:6:               >> Entfernung in cm mit Ultraschallsensor an Digitalpin 6 einlesen\n\r"
   + ip_adresse + "/?w_msg:abcdef          >> \"abcdef\" in Arduino-String \"message\" speichern\n\r");
   client.print( 
     ip_adresse + "/?r_msg:                >> Arduino-String \"message\" auslesen\n\r"
   + ip_adresse + "/?ntc_in:0:5:           >> NTC-Temperaturen von Analog pin A0 und A5 einlesen\n\r" 
   + ip_adresse + "/?help:                 >> Anzeige der Befehlsliste\n\r" 
   + ip_adresse + "/?ver:                  >> Anzeige der Firmware-Version\n\r" 
 
   );
 }
//########################################################################### 
if (befehl == "ver")     
{ valid_command = true; 
  client.print(ver + "\n\r");
}  
//###########################################################################
if (valid_command == false) {client.print("no valid command !\n\r");}
 
//###########################################################################
 
client.println(); 
 
////////////////////////////////////////////////////////////////////////
        delay(100); // give the web browser time to receive the data
        client.stop(); // close the connection:
 
      }
////////////////////////////////////////////////////////////////////////      
pinMode(13, OUTPUT);    //lED leuchtet während der Abarbeitung der Befehle
digitalWrite(13, LOW);
 
//hierhin kommen eigenständige Steuerung-_Programme, die dauernd zyklisch durchlaufen werden, 
//wenn aktuell keine http-Anfrage abgearbeitet wird:
 
 
 
////////////////////////////////////////////////////////////////////////
}
 
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
