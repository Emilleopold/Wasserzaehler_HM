/******************************************************************************
Beispiel:      configuration

Beschreibung:  dieses Beispiel zeigt die Nutzung der get- und set-Methoden, um
               die Konfiguration des MCP8901 zu ändern bzw. auszulesen.
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

const uint8_t i2cAddressMCP9801     = 0x90;

/******************************************************************************
Funktionen
******************************************************************************/
void printConfiguration(void)
{
  int16_t temperature;
  
  Serial.print("OneShot: ");
  Serial.print(TemperatureSensor.getOneShot(), DEC);
  Serial.print("\r\n");

  Serial.print("ADCResolution: ");
  Serial.print(TemperatureSensor.getADCResolution(), DEC);
  Serial.print("\r\n");

  Serial.print("AlertPolarity: ");
  Serial.print(TemperatureSensor.getAlertPolarity(), DEC);
  Serial.print("\r\n");

  Serial.print("Mode: ");
  Serial.print(TemperatureSensor.getMode(), DEC);
  Serial.print("\r\n");

  Serial.print("Shutdown: ");
  Serial.print(TemperatureSensor.getShutdown(), DEC);
  Serial.print("\r\n");
  
  temperature = TemperatureSensor.getTemperatureHysteresis();

  Serial.print("Hysterese: ");
  Serial.print(temperature / 10, DEC);
  Serial.print(",");
  
  if(temperature < 0)
  {
    temperature = -temperature;
  }
  
  Serial.print(temperature % 10, DEC);
  Serial.print(" Grad Celsius\r\n");
  
  temperature = TemperatureSensor.getTemperatureLimit();
  
  Serial.print("Limit: ");
  Serial.print(temperature / 10, DEC);
  Serial.print(",");
  
  if(temperature < 0)
  {
    temperature = -temperature;
  }
  
  Serial.print(temperature % 10, DEC);
  Serial.print(" Grad Celsius\r\n");
  
  Serial.print("----------------\r\n");  
}

void setup() 
{
  // Serielle Schnittstelle konfigurieren
  Serial.begin(115200);
  
  // I2C-Modul initialisieren
  Wire.begin();
  
  // Temperatursensor mit I2C-Adresse des MCP9801 initialisieren
  TemperatureSensor.begin(i2cAddressMCP9801);
  
  // Zufallszahlengenerator initialisieren
  randomSeed(analogRead(0));
  
  // Start-Konfiguration ausgeben
  printConfiguration();
  
  // Konfiguration bearbeiten
  Serial.print("Konfiguration wird bearbeitet.\r\n");
  Serial.print("----------------\r\n");
  
  TemperatureSensor.setShutdown(MCP9801::SHUTDOWN_ENABLE);  
  TemperatureSensor.setMode(MCP9801::INTERRUPT_MODE);
  TemperatureSensor.setAlertPolarity(MCP9801::POLARITY_ACTIVE_HIGH);
  TemperatureSensor.setFaultQueue(MCP9801::FAULTQUEUE_8);
  TemperatureSensor.setADCResolution(MCP9801::RESOLUTION_12BIT);
  TemperatureSensor.setOneShot(MCP9801::ONESHOT_ENABLE);

  // die Temperaturen werden auf einen Zufallswert zwischen -100,0°C und
  // +100°C gesetzt, um bei einem Reset sich ändernde Werte zu generieren.
  TemperatureSensor.setTemperatureLimit(random(-1000, 1000));
  TemperatureSensor.setTemperatureHysteresis(random(-1000, 1000));

  // neue Konfiguration ausgeben
  printConfiguration();
}

void loop()
{
}
