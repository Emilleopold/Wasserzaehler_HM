#include "I2C_4DLED.h"
#include "utility/sevensegment.h"

/**********************************************************
Konstruktor
**********************************************************/
I2C_4DLED::I2C_4DLED()
{}

/**********************************************************
öffentliche Methoden
**********************************************************/

/*---------------------------------------------------------
Name:           begin

Beschreibung:   initialisiert das I2C-4Digit-LED-Modul            
 
Eingänge:       uint8_t i2cAddressSAA1064
                  I2C-Adresse des LED-Treibers (SAA1064)

                  die I2C-Adresse des SAA1064-Chips ergibt 
                  sich durch die Belegung der Lötjumper
                  J6 bis J9, wobei immer nur ein Jumper 
                  gebrückt sein darf.
                  
                  J6 gebrückt -> Adresse 0x76
                  J7 gebrückt -> Adresse 0x74
                  J8 gebrückt -> Adresse 0x72
                  J9 gebrückt -> Adresse 0x70 (werkseitig gebrückt)

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::begin(uint8_t i2cAddressSAA1064)
{
  LedDriver.begin(i2cAddressSAA1064);
}

/*---------------------------------------------------------
Name:           clearDisplay

Beschreibung:   löscht die Daten auf dem Display          
 
Eingänge:       void      

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::clearDisplay(void)
{
  LedDriver.clear();  
}

/*---------------------------------------------------------
Name:           testDisplaySegments

Beschreibung:   schaltet alle Segmente des Displays ein bzw.
                aus.
 
Eingänge:       uint8_t active
                  zeigt an, ob alle Segmente ein- oder
                  ausgeschaltet werden sollen.

                  active == 1: alle Segmente einschalten
                  active == 0: alle Segmente ausschalten

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::testDisplaySegments(uint8_t active)
{
  LedDriver.segmentTest(active);
}

/*---------------------------------------------------------
Name:           setDisplayOutputCurrent

Beschreibung:   setzt den Ausgangsstrom des LED-Treibers,
                gleichbedeutend mit der Einstellung der
                Helligkeit.
 
Eingänge:       OutputCurrent outputCurrent
                  Ausgangsstrom des LED-Treibers

                  mögliche Werte:
                  I2C_4DLED::OUTPUT_CURRENT_0_MA    -> 0 mA   
                  I2C_4DLED::OUTPUT_CURRENT_3_MA    -> 3 mA
                  I2C_4DLED::OUTPUT_CURRENT_6_MA    -> 6 mA
                  I2C_4DLED::OUTPUT_CURRENT_9_MA    -> 9 mA
                  I2C_4DLED::OUTPUT_CURRENT_12_MA   -> 12 mA
                  I2C_4DLED::OUTPUT_CURRENT_15_MA   -> 15 mA
                  I2C_4DLED::OUTPUT_CURRENT_18_MA   -> 18 mA
                  I2C_4DLED::OUTPUT_CURRENT_21_MA   -> 21 mA

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::setDisplayOutputCurrent(OutputCurrent outputCurrent)
{
  LedDriver.setOutputCurrent((SAA1064::OutputCurrent)outputCurrent);
}


/*---------------------------------------------------------
Name:           writeDecimal

Beschreibung:   zeigt eine Dezimalzahl ohne Punkt auf dem
                Display an
 
Eingänge:       int16_t decimal
                  auf dem Display darzustellende Dezimalzahl

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::writeDecimal(int16_t decimal)
{
  uint8_t digits[4] = {0x00, 0x00, 0x00, 0x00};

  calcDigits(digits, decimal, 0, 10);

  LedDriver.writeDigits(SAA1064::SUBADDRESS_DIGIT_1, digits, 4);
}

/*---------------------------------------------------------
Name:           writeDecimal

Beschreibung:   zeigt eine Dezimalzahl mit Nachkommastellen 
                auf dem Display an          
 
Eingänge:       int16_t decimal
                  auf dem Display darzustellende Dezimalzahl

                uint8_t decimalDigits
                  Anzahl der Nachkommastellen in decimal
                  (Der Punkt wird entsprechend der 
                  Nachkommastellengesetzt)

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::writeDecimal(int16_t decimal, uint8_t decimalDigits)
{
  uint8_t digits[4] = {0x00, 0x00, 0x00, 0x00};

  calcDigits(digits, decimal, decimalDigits + 1, 10);

  digits[3 - decimalDigits] |= 0x80;

  LedDriver.writeDigits(SAA1064::SUBADDRESS_DIGIT_1, digits, 4);
}

/*---------------------------------------------------------
Name:           writeHexadecimal

Beschreibung:   zeigt eine Hexadezimalzahl mit Nachkommastellen 
                auf dem Display an           
 
Eingänge:       uint16_t hexadecimal
                  auf dem Display darzustellende Hexadezimalzahl

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::writeHexadecimal(uint16_t hexadecimal)
{
  uint8_t digits[4] = {0x00, 0x00, 0x00, 0x00};

  calcDigits(digits, hexadecimal, 0, 16);

  LedDriver.writeDigits(SAA1064::SUBADDRESS_DIGIT_1, digits, 4);
}

/**********************************************************
private Methoden
**********************************************************/
/*---------------------------------------------------------
Name:           calcDigits

Beschreibung:   rechnet die übergebene Zahl in die auf dem
                Display einzuschaltenden Elemente um.
 
Eingänge:       uint8_t *digits
                  Pointer auf ein Array mit 4 Elementen, in das
                  die berechneten Elemente gespeichert werden.

                int decimal
                  darzustellende Zahl

                uint8_t minimumDigitCount
                  Mindestanzahl der darzustellenden Zeichen

                uint8_t base
                  Basis der darzustellenden Zahl
                  (z.B. 10: Dezimalzahl, 16: Hexadezimalzahl)

Ausgang:        void
---------------------------------------------------------*/
void I2C_4DLED::calcDigits(uint8_t *digits, int decimal, uint8_t minimumDigitCount, uint8_t base)
{
  uint8_t sign = 0;
  uint8_t position = 3;
  
  // kontrollieren, ob die Zahl negativ ist
  if(decimal < 0)
  {
    sign = 1;
    decimal = -decimal;
  }

  // die einzelnen Stellen der Anzeige berechnen
  while( (decimal > 0) && (position != 0xff) )
  {
    digits[position--] = sevensegment[decimal % base];
    decimal /= base;
    
    if(minimumDigitCount > 0)
    {
      minimumDigitCount--;
    }
  }

  // falls noch mehr Zeichen dargestellt werden sollen, die restlichen
  // Zeichen mit Nullen auffüllen
  while( (minimumDigitCount > 0) && (position != 0xff) )
  {
    digits[position--] = sevensegment[0];
    minimumDigitCount--;
  }

  // Vorzeichen setzen
  if( (sign == 1) && (position != 0xff) )
  {
    digits[position] = 0x40;
  }
}

/**********************************************************
Vorinstantiiertes Objekt
**********************************************************/
I2C_4DLED FourDigitLedDisplay = I2C_4DLED();