/******************************************************************************
Beispiel:      setBrightness

Beschreibung:  Dieses Beispiel zeigt die Möglichkeit die Helligkeit der Segmente
               einzustellen. 
******************************************************************************/
#include "Wire.h"
#include "I2C_4DLED.h"

/******************************************************************************
globale Konstanten
******************************************************************************/

// die I2C-Adresse des SAA1064-Chips ergibt sich durch die Belegung der Lötjumper
// J6 bis J9, wobei immer nur ein Jumper gebrückt sein darf.
// J6 gebrückt -> Adresse 0x76
// J7 gebrückt -> Adresse 0x74
// J8 gebrückt -> Adresse 0x72
// J9 gebrückt -> Adresse 0x70 (werkseitig gebrückt)
const uint8_t i2cAddressSAA1064 = 0x70;

/******************************************************************************
globale Variablen
******************************************************************************/
// Helligkeitsstufe
uint8_t brightness = 0;

// Richtung des Dimmens
int8_t dimmDirection = 1;

// Ausgangsstrom des SAA1064
I2C_4DLED::OutputCurrent outputCurrent = I2C_4DLED::OUTPUT_CURRENT_0_MA;

/******************************************************************************
Funktionen
******************************************************************************/
void setup() 
{
  // I2C-Modul initialisieren
  Wire.begin();
  
  // 4-Digit-LED-Anzeige mit I2C-Adresse des SAA1064 initialisieren
  FourDigitLedDisplay.begin(i2cAddressSAA1064);
  
  // 8888 ausgeben, so dass alle Segmente eingeschaltet werden 
  FourDigitLedDisplay.writeDecimal(8888);
}

void loop()
{
  // je nach aktueller Helligkeit den Ausgangstrom einstellen
  switch(brightness)
  {
  case 0:
    outputCurrent = I2C_4DLED::OUTPUT_CURRENT_0_MA;
    break;
    
  case 1:
    outputCurrent = I2C_4DLED::OUTPUT_CURRENT_3_MA;
    break;

  case 2:
    outputCurrent = I2C_4DLED::OUTPUT_CURRENT_6_MA;
    break;

  case 3:
    outputCurrent = I2C_4DLED::OUTPUT_CURRENT_9_MA;
    break;

  case 4:
    outputCurrent = I2C_4DLED::OUTPUT_CURRENT_12_MA;
    break;
  
  default:
    brightness = 0;
    break; 
  }
  
  // Ausgangsstrom dem SAA1064 übergeben
  FourDigitLedDisplay.setDisplayOutputCurrent(outputCurrent);
  
  // Helligkeit hoch- und runterdimmen
  brightness += dimmDirection;
  
  if(brightness == 4)
  {
    dimmDirection = -1;
  }
  else if(brightness == 0)
  {
    dimmDirection = 1;
  }

  delay(250);
}
