#include "Wire.h"
#include "MCP9801.h"

/**********************************************************
Konstruktor
**********************************************************/
MCP9801::MCP9801()
{}

/**********************************************************
öffentiliche Methoden
**********************************************************/

/*---------------------------------------------------------
Name:           begin

Beschreibung:   initialisiert den Temperatursensor MCP9801
                mit der werkseitigen Konfiguration
 
Eingänge:       uint8_t i2cAddress
                  I2C-Adresse des MCP9801

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::begin(uint8_t i2cAddress)
{
  address         = i2cAddress >> 1;
  configRegister  = 0x00;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           begin

Beschreibung:   initialisiert den Temperatursensor MCP9801
                mit den übergebenen Parametern          
 
Eingänge:       uint8_t i2cAddress
                  I2C-Adresse des MCP9801

                OneShot oneShot
                  zeigt an, ob der MCP9801 kontinuierlich oder
                  nur ein einziges Mal messen soll.

                  mögliche Werte:
                  MCP9801::ONESHOT_DISABLED    -> kontinuierliche Messung
                  MCP9801::ONESHOT_ENABLED     -> einmalige Messung

                ADCResolution adcResolution
                  Auflösung der Temperaturmessung

                  mögliche Werte:
                  MCP9801::RESOLUTION_9BIT      -> 9 Bit (0,5 °C, 30 ms Wandlungszeit)
                  MCP9801::RESOLUTION_10BIT     -> 10 Bit (0,25 °C, 60 ms Wandlungszeit)
                  MCP9801::RESOLUTION_11BIT     -> 11 Bit (0,125 °C, 120 ms Wandlungszeit)
                  MCP9801::RESOLUTION_12BIT     -> 12 Bit (0,0625 °C, 240 ms Wandlungszeit)

                FaultQueue faultQueue
                  Anzahl der Wandlungen, die die Temperatur über T_SET bzw.
                  unter T_HYST bleiben muss, bis der ALERT-Pin seinen Zustand
                  ändert.

                  mögliche Werte:
                  MCP9801::FAULTQUEUE_1         -> 1 Wandlung
                  MCP9801::FAULTQUEUE_2         -> 2 Wandlungen
                  MCP9801::FAULTQUEUE_4         -> 4 Wandlungen
                  MCP9801::FAULTQUEUE_8         -> 8 Wandlungen

                AlertPolarity alertPolarity
                  aktiver Pegel des ALERT-Ausgang.

                  mögliche Werte:
                  MCP9801::POLARITY_ACTIVE_LOW    -> Low-Pegel
                  MCP9801::POLARITY_ACTIVE_HIGH   -> High-Pegel

                Mode mode
                  Modus wie der ALERT-Ausgang geschaltet wird.

                  mögliche Werte:
                  MCP9801::COMPARATOR_MODE
                    Im Komparator-Modus wird der ALERT-Ausgang bei
                    überschreiten der Temperatur T_SET auf den in
                    alertPolarity gesetzen Pegel gezogen. Erst nach
                    unterschreiten der Temperatur T_HYST wird der Pegel
                    zurückgeschaltet.

                  MCP9801::INTERRUPT_MODE
                    Im Interrupt-Modus wird der ALERT-Ausgang bei
                    überschreiten der Temperatur T_SET oder bei unterschreiten
                    der Temperatur T_HYST auf den in alertPolarity 
                    gesetzen Pegel gezogen. Der Pegel wird zurückgeschaltet, sobald
                    vom angeschlossenen Mikrocontroller ein Lesebefehl durchgeführt
                    wurde.                    

                Shutdown shutdown
                  alle stromverbrauchenden Komponenten
                  ein- bzw. ausschalten. Im ausgeschalteten
                  Zustand können die Register weiterhin gelesen
                  und geschrieben werden, jedoch ist die
                  Temperaturmessung abgeschaltet.

                  mögliche Werte:
                  MCP9801::SHUTDOWN_DISABLE    -> Komponenten eingeschaltet
                  MCP9801::SHUTDOWN_ENABLE     -> Komponenten ausgeschaltet

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::begin(uint8_t i2cAddress, MCP9801::OneShot oneShot, MCP9801::ADCResolution adcResolution, 
                    MCP9801::FaultQueue faultQueue, MCP9801::AlertPolarity alertPolarity,
                    MCP9801::Mode mode, MCP9801::Shutdown shutdown)
{
  address         = i2cAddress >> 1;
  configRegister  = oneShot | adcResolution | faultQueue | alertPolarity | mode | shutdown;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           setOneShot

Beschreibung:   setzt die Messart des MCP9801
 
Eingänge:       OneShot oneShot
                  zeigt an, ob der MCP9801 kontinuierlich oder
                  nur ein einziges Mal messen soll.

                  mögliche Werte:
                  MCP9801::ONESHOT_DISABLED    -> kontinuierliche Messung
                  MCP9801::ONESHOT_ENABLED     -> einmalige Messung      

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setOneShot(MCP9801::OneShot oneShot)
{
  configRegister &= ~(1 << 7);
  configRegister |= oneShot;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           getOneShot

Beschreibung:   liest die Messart des MCP9801
 
Eingänge:       void      

Ausgang:        OneShot oneShot
                  aktuelle Messart des MCP9801

                  mögliche Werte:
                  MCP9801::ONESHOT_DISABLED    -> kontinuierliche Messung
                  MCP9801::ONESHOT_ENABLED     -> einmalige Messung
---------------------------------------------------------*/
MCP9801::OneShot MCP9801::getOneShot(void)
{
  readConfigRegister();

  return ( (MCP9801::OneShot)(configRegister & (1 << 7)) );
}

/*---------------------------------------------------------
Name:           setADCResolution

Beschreibung:   setzt die Auflösung der Temperaturmessung          
 
Eingänge:       ADCResolution adcResolution
                  Auflösung der Temperaturmessung

                  mögliche Werte:
                  MCP9801::RESOLUTION_9BIT      -> 9 Bit (0,5 °C, 30 ms Wandlungszeit)
                  MCP9801::RESOLUTION_10BIT     -> 10 Bit (0,25 °C, 60 ms Wandlungszeit)
                  MCP9801::RESOLUTION_11BIT     -> 11 Bit (0,125 °C, 120 ms Wandlungszeit)
                  MCP9801::RESOLUTION_12BIT     -> 12 Bit (0,0625 °C, 240 ms Wandlungszeit)     

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setADCResolution(MCP9801::ADCResolution adcResolution)
{
  configRegister &= ~( (1 << 6) | (1 << 5) );
  configRegister |= adcResolution;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           getADCResolution

Beschreibung:   liest die Auflösung der Temperaturmessung          
 
Eingänge:       void

Ausgang:        ADCResolution adcResolution
                  Auflösung der Temperaturmessung

                  mögliche Werte:
                  MCP9801::RESOLUTION_9BIT      -> 9 Bit (0,5 °C, 30 ms Wandlungszeit)
                  MCP9801::RESOLUTION_10BIT     -> 10 Bit (0,25 °C, 60 ms Wandlungszeit)
                  MCP9801::RESOLUTION_11BIT     -> 11 Bit (0,125 °C, 120 ms Wandlungszeit)
                  MCP9801::RESOLUTION_12BIT     -> 12 Bit (0,0625 °C, 240 ms Wandlungszeit)
---------------------------------------------------------*/
MCP9801::ADCResolution MCP9801::getADCResolution(void)
{
  readConfigRegister();

  return ( (MCP9801::ADCResolution)(configRegister & ((1 << 6) | (1 << 5))) );
}

/*---------------------------------------------------------
Name:           setFaultQueue

Beschreibung:   setzt die Anzahl der Wandlungen die zum Umschalten
                des ALERT-Ausgangs benötigt werden
 
Eingänge:       FaultQueue faultQueue
                  Anzahl der Wandlungen, die die Temperatur über T_SET bzw.
                  unter T_HYST bleiben muss, bis der ALERT-Pin seinen Zustand
                  ändert.

                  mögliche Werte:
                  MCP9801::FAULTQUEUE_1         -> 1 Wandlung
                  MCP9801::FAULTQUEUE_2         -> 2 Wandlungen
                  MCP9801::FAULTQUEUE_4         -> 4 Wandlungen
                  MCP9801::FAULTQUEUE_8         -> 8 Wandlungen      

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setFaultQueue(MCP9801::FaultQueue faultQueue)
{
  configRegister &= ~( (1 << 4) | (1 << 3) );
  configRegister |= faultQueue;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           getFaultQueue

Beschreibung:   liest die Anzahl der Wandlungen die zum Umschalten
                des ALERT-Ausgangs benötigt werden
 
Eingänge:       void

Ausgang:        FaultQueue faultQueue
                  Anzahl der Wandlungen, die die Temperatur über T_SET bzw.
                  unter T_HYST bleiben muss, bis der ALERT-Pin seinen Zustand
                  ändert.

                  mögliche Werte:
                  MCP9801::FAULTQUEUE_1         -> 1 Wandlung
                  MCP9801::FAULTQUEUE_2         -> 2 Wandlungen
                  MCP9801::FAULTQUEUE_4         -> 4 Wandlungen
                  MCP9801::FAULTQUEUE_8         -> 8 Wandlungen      
---------------------------------------------------------*/
MCP9801::FaultQueue MCP9801::getFaultQueue(void)
{
  readConfigRegister();

  return ( (MCP9801::FaultQueue)(configRegister & ((1 << 4) | (1 << 3))) );
}

/*---------------------------------------------------------
Name:           setAlertPolarity

Beschreibung:   setzt den aktiven Pegel des ALERT-Ausgangs          
 
Eingänge:       AlertPolarity alertPolarity
                  aktiver Pegel des ALERT-Ausgang.

                  mögliche Werte:
                  MCP9801::POLARITY_ACTIVE_LOW    -> Low-Pegel
                  MCP9801::POLARITY_ACTIVE_HIGH   -> High-Pegel      

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setAlertPolarity(MCP9801::AlertPolarity alertPolarity)
{
  configRegister &= ~(1 << 2);
  configRegister |= alertPolarity;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           getAlertPolarity

Beschreibung:   liest den aktiven Pegel des ALERT-Ausgangs          
 
Eingänge:       void

Ausgang:        AlertPolarity alertPolarity
                  aktiver Pegel des ALERT-Ausgang.

                  mögliche Werte:
                  MCP9801::POLARITY_ACTIVE_LOW    -> Low-Pegel
                  MCP9801::POLARITY_ACTIVE_HIGH   -> High-Pegel      
---------------------------------------------------------*/
MCP9801::AlertPolarity MCP9801::getAlertPolarity(void)
{
  readConfigRegister();

  return ( (MCP9801::AlertPolarity)(configRegister & (1 << 2)) );
}

/*---------------------------------------------------------
Name:           setMode

Beschreibung:   setzt den Modus, wie der ALERT-Ausgang geschaltet
                wird
 
Eingänge:       Mode mode
                  Modus wie der ALERT-Ausgang geschaltet wird.

                  mögliche Werte:
                  MCP9801::COMPARATOR_MODE
                    Im Komparator-Modus wird der ALERT-Ausgang bei
                    überschreiten der Temperatur T_SET auf den in
                    alertPolarity gesetzen Pegel gezogen. Erst nach
                    unterschreiten der Temperatur T_HYST wird der Pegel
                    zurückgeschaltet.

                  MCP9801::INTERRUPT_MODE
                    Im Interrupt-Modus wird der ALERT-Ausgang bei
                    überschreiten der Temperatur T_SET oder bei unterschreiten
                    der Temperatur T_HYST auf den in alertPolarity 
                    gesetzen Pegel gezogen. Der Pegel wird zurückgeschaltet, sobald
                    vom angeschlossenen Mikrocontroller ein Lesebefehl durchgeführt
                    wurde.      

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setMode(MCP9801::Mode mode)
{
  configRegister &= ~(1 << 1);
  configRegister |= mode;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           getMode

Beschreibung:   liest den Modus, wie der ALERT-Ausgang geschaltet
                wird
 
Eingänge:       void

Ausgang:        Mode mode
                  Modus wie der ALERT-Ausgang geschaltet wird.

                  mögliche Werte:
                  MCP9801::COMPARATOR_MODE
                    Im Komparator-Modus wird der ALERT-Ausgang bei
                    überschreiten der Temperatur T_SET auf den in
                    alertPolarity gesetzen Pegel gezogen. Erst nach
                    unterschreiten der Temperatur T_HYST wird der Pegel
                    zurückgeschaltet.

                  MCP9801::INTERRUPT_MODE
                    Im Interrupt-Modus wird der ALERT-Ausgang bei
                    überschreiten der Temperatur T_SET oder bei unterschreiten
                    der Temperatur T_HYST auf den in alertPolarity 
                    gesetzen Pegel gezogen. Der Pegel wird zurückgeschaltet, sobald
                    vom angeschlossenen Mikrocontroller ein Lesebefehl durchgeführt
                    wurde.
---------------------------------------------------------*/
MCP9801::Mode MCP9801::getMode(void)
{
  readConfigRegister();

  return ( (MCP9801::Mode)(configRegister & (1 << 1)) );
}

/*---------------------------------------------------------
Name:           setShutdown

Beschreibung:   setzt die Aktivierung bzw. Deaktivierung des
                Stromsparmodus          
 
Eingänge:       Shutdown shutdown
                  alle stromverbrauchenden Komponenten
                  ein- bzw. ausschalten. Im ausgeschalteten
                  Zustand können die Register weiterhin gelesen
                  und geschrieben werden, jedoch ist die
                  Temperaturmessung abgeschaltet.

                  mögliche Werte:
                  MCP9801::SHUTDOWN_DISABLE    -> Komponenten eingeschaltet
                  MCP9801::SHUTDOWN_ENABLE     -> Komponenten ausgeschaltet      

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setShutdown(MCP9801::Shutdown shutdown)
{
  configRegister &= ~(1 << 0);
  configRegister |= shutdown;

  writeConfigRegister();
}

/*---------------------------------------------------------
Name:           getShutdown

Beschreibung:   liest die Aktivierung bzw. Deaktivierung des
                Stromsparmodus

Eingänge:       void
 
Ausgang:        Shutdown shutdown
                  alle stromverbrauchenden Komponenten
                  ein- bzw. ausschalten. Im ausgeschalteten
                  Zustand können die Register weiterhin gelesen
                  und geschrieben werden, jedoch ist die
                  Temperaturmessung abgeschaltet.

                  mögliche Werte:
                  MCP9801::SHUTDOWN_DISABLE    -> Komponenten eingeschaltet
                  MCP9801::SHUTDOWN_ENABLE     -> Komponenten ausgeschaltet      
---------------------------------------------------------*/
MCP9801::Shutdown MCP9801::getShutdown(void)
{
  readConfigRegister();

  return ( (MCP9801::Shutdown)(configRegister & (1 << 0)) );
}

/*---------------------------------------------------------
Name:           readTemperature

Beschreibung:   liest die Temperatur des MC9801 aus.

                Format:
                Die Temperatur ist in Festkommaarithmetik
                mit 4 Nachkommastellen angegeben.

                Beispiele:
                100     ->    0,0100 °C
                1000    ->    0,1000 °C
                10000   ->    1,0000 °C
 
Eingänge:       void      

Ausgang:        int32_t
                  ausgelesene Temperatur (Format beachten)
---------------------------------------------------------*/
int32_t MCP9801::readTemperature(void)
{
  int32_t temperature;
  int16_t readValue;
  uint8_t readData[2];
  
  Wire.beginTransmission(address);
  Wire.write(TEMPERATURE_REGISTER);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)address, (uint8_t)2);

  readData[0] = Wire.read();
  readData[1] = Wire.read();

  readValue = ((int16_t)readData[0] << 4) | ((int16_t)readData[1] >> 4);

  if(readData[0] & 0x80)
  {
    readValue |= 0xf000;
  }

  temperature = 625L * readValue;

  return temperature;
}

/*---------------------------------------------------------
Name:           setTemperatureHystersis

Beschreibung:   schreibt den Temperatur-Hysterese-Wert
 
Eingänge:       int16_t hysteresis
                  Temperatur-Hysterese-Wert in Festkommaarithmetik
                  mit 1 Nachkommastelle in 5er Schritten

                  Beispiele:
                    75,0°C    -> 750
                    -3,5°C    -> -35
                    55,2°C    -> 552 (wird innerhalb der Routine
                                      zu 550 gewandelt)

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setTemperatureHysteresis(int16_t hysteresis)
{
  uint8_t registerData[2] = {0x00 , 0x00};

  if(hysteresis < 0)
  {
    registerData[0] = 0x80;
    hysteresis = -hysteresis;
  }

  registerData[0] |= (hysteresis / 10);

  // Berechnung der Nachkommastelle:
  // hysteresis % 10  ->  Nachkommastelle herausfiltern
  // x / 5            ->  0, 1, 2, 3, 4 -> 0
  //                      5, 6, 7, 8, 9 -> 1
  // x << 7           ->  im Register liegt die
  //                      Nachommastelle im Bit 7 
  //                      des unteren Bytes
  registerData[1] = (((hysteresis % 10) / 5) << 7);

  Wire.beginTransmission(address);
  Wire.write(TEMPERATURE_HYSTERESIS_REGISTER);
  Wire.write(registerData, 2);
  Wire.endTransmission();
}

/*---------------------------------------------------------
Name:           getTemperatureHystersis

Beschreibung:   liest den Temperatur-Hysterese-Wert aus
 
Eingänge:       void      

Ausgang:        int16_t
                  Temperatur-Hysterese-Wert in Festkommaarithmetik
                  mit 1 Nachkommastelle

                  Beispiele:
                    120   -> 12,0 °C
                    -35   -> -3,5 °C
---------------------------------------------------------*/
int16_t MCP9801::getTemperatureHysteresis(void)
{
  int16_t hysteresis;
  uint8_t registerData[2] = {0x00 , 0x00};

  Wire.beginTransmission(address);
  Wire.write(TEMPERATURE_HYSTERESIS_REGISTER);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)address, (uint8_t)2);

  registerData[0] = Wire.read();
  registerData[1] = Wire.read();

  hysteresis = (registerData[0] & ~(1 << 7)) * 10;
  
  if(registerData[1] > 0)
  {
    hysteresis += 5;
  }

  if(registerData[0] & 0x80)
  {
    hysteresis = -hysteresis;
  }

  return hysteresis;
}

/*---------------------------------------------------------
Name:           setTemperatureLimit

Beschreibung:   schreibt den Temperatur-Limit-Wert
 
Eingänge:       int16_t limit
                  Temperatur-Limit-Wert in Festkommaarithmetik
                  mit 1 Nachkommastelle in 5er Schritten

                  Beispiele:
                    75,0°C    -> 750
                    -3,5°C    -> -35
                    55,2°C    -> 552 (wird innerhalb der Routine
                                      zu 550 gewandelt)

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::setTemperatureLimit(int16_t limit)
{
  uint8_t registerData[2] = {0x00 , 0x00};

  if(limit < 0)
  {
    registerData[0] = 0x80;
    limit = -limit;
  }

  registerData[0] |= (limit / 10);

  // Berechnung der Nachkommastelle:
  // hysteresis % 10  ->  Nachkommastelle herausfiltern
  // x / 5            ->  0, 1, 2, 3, 4 -> 0
  //                      5, 6, 7, 8, 9 -> 1
  // x << 7           ->  im Register liegt die
  //                      Nachommastelle im Bit 7 
  //                      des unteren Bytes
  registerData[1] = (((limit % 10) / 5) << 7);

  Wire.beginTransmission(address);
  Wire.write(TEMPERATURE_LIMIT_SET_REGISTER);
  Wire.write(registerData, 2);
  Wire.endTransmission();
}

/*---------------------------------------------------------
Name:           getTemperatureLimit

Beschreibung:   liest den Temperatur-Limit-Wert aus
 
Eingänge:       void      

Ausgang:        int16_t
                  Temperatur-Limit-Wert in Festkommaarithmetik
                  mit 1 Nachkommastelle

                  Beispiele:
                    120   -> 12,0 °C
                    -35   -> -3,5 °C
---------------------------------------------------------*/
int16_t MCP9801::getTemperatureLimit(void)
{
  int16_t limit;
  uint8_t registerData[2] = {0x00 , 0x00};

  Wire.beginTransmission(address);
  Wire.write(TEMPERATURE_LIMIT_SET_REGISTER);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)address, (uint8_t)2);

  registerData[0] = Wire.read();
  registerData[1] = Wire.read();

  limit = (registerData[0] & ~(1 << 7)) * 10;
  
  if(registerData[1] > 0)
  {
    limit += 5;
  }

  if(registerData[0] & 0x80)
  {
    limit = -limit;
  }

  return limit;
}


/**********************************************************
private Methoden
**********************************************************/
/*---------------------------------------------------------
Name:           writeConfigRegister

Beschreibung:   schreibt die Daten des Konfigurationsregisters
 
Eingänge:       void      

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::writeConfigRegister(void)
{
  Wire.beginTransmission(address);
  Wire.write(CONFIG_REGISTER);
  Wire.write(configRegister);
  Wire.endTransmission();
}

/*---------------------------------------------------------
Name:           readConfigRegister

Beschreibung:   liest die Daten des Konfigurationsregisters
 
Eingänge:       void      

Ausgang:        void
---------------------------------------------------------*/
void MCP9801::readConfigRegister(void)
{
  Wire.beginTransmission(address);
  Wire.write(CONFIG_REGISTER);
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)address, (uint8_t)1);

  configRegister = Wire.read();  
}

/**********************************************************
Vorinstantiiertes Objekt
**********************************************************/
MCP9801 TemperatureSensor = MCP9801();