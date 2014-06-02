/******************************************************************************
Beispiel:      usingAlertOutput

Beschreibung:  dieses Beispiel zeigt wie die Alarmfunktion des Temperatur-Chips
               verwendet wird. Der Alarmwert ist so eingestellt, dass der Sensor
               bei Berührung mit dem Finger (Körpertemperatur) nach einiger Zeit 
               einen Alarm auslöst. Der ALERT-Ausgang wechselt auf Low-Pegel.
               Wird der Finger wieder vom Sensor entfernt, sinkt die Temperatur
               nach kurzer Zeit wieder auf Raumtemperatur ab. Der Hysteresewert
               ist so eingestellt, dass unterhalb von 26°C der Ausgang wieder auf
               High-Pegel wechselt.

               Zur Veranschaulichung wird der aktuelle Temperaturwert über die
               serielle Schnittstelle ausgegeben.               
******************************************************************************/
#include "Wire.h"
#include "MCP9801.h"

/******************************************************************************
globale Konstanten
******************************************************************************/
// beim I2C-4DLED-Board kann die I2C-Adresse über Lötjumper konfiguriert werden
// folgende Adressen ergeben sich aus den verschiedenen Jumperbrücken:
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

/******************************************************************************
globale Variablen
******************************************************************************/
// ausgelesene Temperatur
int32_t temperature;

/******************************************************************************
Funktionen
******************************************************************************/
void setup() 
{
  // Serielle Schnittstelle konfigurieren
  Serial.begin(115200);
  
  // I2C-Modul initialisieren
  Wire.begin();
  
  // Temperatursensor mit I2C-Adresse des MCP9801 initialisieren
  TemperatureSensor.begin(i2cAddressMCP9801);
  
  // Temperatursensor auf eine Auflösung von 12 Bit umschalten
  TemperatureSensor.setADCResolution(TemperatureSensor.RESOLUTION_12BIT);
  
  // Limit auf 30,0 °C setzen
  TemperatureSensor.setTemperatureLimit(300);

  // Hysterese auf 26,0 °C setzen
  TemperatureSensor.setTemperatureHysteresis(260);
}

void loop()
{
  // Temperatur auslesen
  // Achtung: Die Temperatur wir in Festkommaarithmetik mit
  // 4 Nachkommastellen zurückgegeben
  temperature = TemperatureSensor.readTemperature();
  
  Serial.print(temperature / 10000UL, DEC);
  Serial.print(",");
  
  if(temperature < 0)
  {
    temperature = -temperature;
  }
  
  temperature = temperature % 10000UL;
  
  if(temperature == 0)
  {
    Serial.print("000");  
  }
  else if(temperature < 1000)
  {
     Serial.print("0");
  }

  Serial.print(temperature, DEC);
  Serial.print(" Grad Celsius\r\n");
  
  delay(1000);
}
