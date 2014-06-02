#include "Wire.h"
#include "SAA1064.h"

/**********************************************************
Konstruktor
**********************************************************/
SAA1064::SAA1064()
{}

/**********************************************************
öffentiliche Methoden
**********************************************************/

/*---------------------------------------------------------
Name:           begin

Beschreibung:   initialisiert den LED-Treiber SAA1064
                mit der werkseitigen Konfiguration          
 
Eingänge:       uint8_t i2cAddress
                  I2C-Adresse des SAA1064

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::begin(uint8_t i2cAddress)
{
  address           = i2cAddress >> 1;
  controlRegister   = DYNAMIC_MODE | OUTPUT_CURRENT_12_MA | (1 << DIGITS_2_AND_4) | (1 << DIGITS_1_AND_3);

  writeControlRegister();  
}

/*---------------------------------------------------------
Name:           begin

Beschreibung:   initialisiert den LED-Treiber SAA1064
                mit den übergebenen Werten für den Modus
                und der Helligkeit
 
Eingänge:       uint8_t i2cAddress
                  I2C-Adresse des SAA1064

                Mode mode
                  Modus des SAA1064

                  mögliche Werte:
                  SAA1064::STATIC_MODE    -> statischer Modus (2 Siebensegmentanzeigen)
                  SAA1064::DYNAMIC_MODE   -> dynamischer Modus (4 Siebensegmentanzeigen)

                OutputCurrent outputCurrent
                  Ausgangsstrom des LED-Treibers,
                  gleichbedeutend mit der Einstellung der
                  Helligkeit

                  mögliche Werte:
                  SAA1064::OUTPUT_CURRENT_0_MA    -> 0 mA   
                  SAA1064::OUTPUT_CURRENT_3_MA    -> 3 mA
                  SAA1064::OUTPUT_CURRENT_6_MA    -> 6 mA
                  SAA1064::OUTPUT_CURRENT_9_MA    -> 9 mA
                  SAA1064::OUTPUT_CURRENT_12_MA   -> 12 mA
                  SAA1064::OUTPUT_CURRENT_15_MA   -> 15 mA
                  SAA1064::OUTPUT_CURRENT_18_MA   -> 18 mA
                  SAA1064::OUTPUT_CURRENT_21_MA   -> 21 mA

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::begin(uint8_t i2cAddress, Mode mode, OutputCurrent outputCurrent)
{
  address           = i2cAddress >> 1;
  controlRegister   = mode | outputCurrent | (1 << DIGITS_2_AND_4) | (1 << DIGITS_1_AND_3);

  writeControlRegister();  
}

/*---------------------------------------------------------
Name:           clear

Beschreibung:   löscht die Segmentdaten innerhalb des SAA1064          
 
Eingänge:       void      

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::clear(void)
{
  Wire.beginTransmission(address);
  Wire.write(SUBADDRESS_DIGIT_1);
  Wire.write((uint8_t)0x00);
  Wire.write((uint8_t)0x00);
  Wire.write((uint8_t)0x00);
  Wire.write((uint8_t)0x00);
  Wire.endTransmission();   
}

/*---------------------------------------------------------
Name:           activeDigits

Beschreibung:   aktiviert bzw. deaktiviert die Anzeigen          
 
Eingänge:       ActiveDigits blankedDigits
                  Anzeige, die aktiviert bzw. deaktiviert
                  werden soll

                  mögliche Werte:
                  SAA1064::DIGITS_1_AND_3   -> Anzeige 1 und 3
                  SAA1064::DIGITS_2_AND_4   -> Anzeige 1 und 3

                uint8_t active
                  neuer Zustand der Aktivierung

                  0 -> Anzeige deaktiviert
                  1 -> Anzeige aktiviert

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::activeDigits(ActiveDigits blankedDigits, uint8_t active)
{
  if(blankedDigits & DIGITS_1_AND_3)
  {
    controlRegister &= ~(1 << DIGITS_1_AND_3);
    controlRegister |= (active << DIGITS_1_AND_3);
  }

  if(blankedDigits & DIGITS_2_AND_4)
  {
    controlRegister &= ~(1 << DIGITS_2_AND_4);
    controlRegister |= (active << DIGITS_2_AND_4);
  }

  writeControlRegister();
}

/*---------------------------------------------------------
Name:           segmentTest

Beschreibung:   aktiviert bzw. deaktiviert den Segmenttest
                (alle Segmente eingeschaltet)
 
Eingänge:       uint8_t active
                  neuer Zustand des Segmenttests

                  0 -> Segmenttest deaktiviert
                  1 -> Segmenttest aktiviert

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::segmentTest(uint8_t active)
{
  controlRegister &= ~(1 << 3);
  controlRegister |= (active << 3);
  
  writeControlRegister();
}

/*---------------------------------------------------------
Name:           setMode

Beschreibung:   setzt den Modus des LED-Treibers SAA1064       
 
Eingänge:       Mode mode
                  Modus des SAA1064

                  mögliche Werte:
                  SAA1064::STATIC_MODE    -> statischer Modus (2 Siebensegmentanzeigen)
                  SAA1064::DYNAMIC_MODE   -> dynamischer Modus (4 Siebensegmentanzeigen)      

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::setMode(Mode mode)
{
  controlRegister &= ~(1 << 0);
  controlRegister |= mode;
  
  writeControlRegister();
}

/*---------------------------------------------------------
Name:           setOutputCurrent

Beschreibung:   setzt den Ausgangsstrom des LED-Treibers          
 
Eingänge:       OutputCurrent outputCurrent
                  Ausgangsstrom des LED-Treibers,
                  gleichbedeutend mit der Einstellung der
                  Helligkeit

                  mögliche Werte:
                  SAA1064::OUTPUT_CURRENT_0_MA    -> 0 mA   
                  SAA1064::OUTPUT_CURRENT_3_MA    -> 3 mA
                  SAA1064::OUTPUT_CURRENT_6_MA    -> 6 mA
                  SAA1064::OUTPUT_CURRENT_9_MA    -> 9 mA
                  SAA1064::OUTPUT_CURRENT_12_MA   -> 12 mA
                  SAA1064::OUTPUT_CURRENT_15_MA   -> 15 mA
                  SAA1064::OUTPUT_CURRENT_18_MA   -> 18 mA
                  SAA1064::OUTPUT_CURRENT_21_MA   -> 21 mA      

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::setOutputCurrent(OutputCurrent outputCurrent)
{
  controlRegister &= ~((1 << 6) | (1 << 5) | (1 << 4));
  controlRegister |= outputCurrent;
  
  writeControlRegister();
}

/*---------------------------------------------------------
Name:           writeDigits

Beschreibung:   schreibt die Segmentdaten der einzelnen Anzeigen          
 
Eingänge:       Subaddress subaddress
                  Addresse im SAA1064 ab dem die Daten
                  geschrieben werden sollen

                  mögliche Werte:
                  SAA1064::SUBADDRESS_DIGIT_1   -> Adresse Anzeige 1
                  SAA1064::SUBADDRESS_DIGIT_2   -> Adresse Anzeige 2
                  SAA1064::SUBADDRESS_DIGIT_3   -> Adresse Anzeige 3
                  SAA1064::SUBADDRESS_DIGIT_4   -> Adresse Anzeige 4

                uint8_t *data
                  Pointer auf die zu schreibenden Segmentdaten
                
                uint8_t datalength
                  Anzahl der Anzeigen, die neu beschrieben werden
                  sollen

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::writeDigits(Subaddress subaddress, uint8_t *data, uint8_t datalength)
{
  Wire.beginTransmission(address);
  Wire.write(subaddress);
  Wire.write(data, datalength);
  Wire.endTransmission();  
}

/**********************************************************
private Methoden
**********************************************************/
/*---------------------------------------------------------
Name:           writeControlRegister

Beschreibung:   schreibt die Daten des Kontrolregisters           
 
Eingänge:       void      

Ausgang:        void
---------------------------------------------------------*/
void SAA1064::writeControlRegister(void)
{
  Wire.beginTransmission(address);
  Wire.write(SUBADDRESS_CONTROLREGISTER);
  Wire.write(controlRegister);
  Wire.endTransmission();
}

/**********************************************************
Vorinstantiiertes Objekt
**********************************************************/
SAA1064 LedDriver = SAA1064();