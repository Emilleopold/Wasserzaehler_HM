/******************************************************************************
Beispiel:      testDisplay

Beschreibung:  Dieses Beispiel zeigt die Möglichkeit alle Segmente des Displays
               einzuschalten, um einen Defekt eines Segments ausschließen zu können. 
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
Funktionen
******************************************************************************/
void setup() 
{
  // I2C-Modul initialisieren
  Wire.begin();
  
  // 4-Digit-LED-Anzeige mit I2C-Adresse des SAA1064 initialisieren
  FourDigitLedDisplay.begin(i2cAddressSAA1064);
  
  // alle im SAA1064 gespeicherten Digits löschen
  FourDigitLedDisplay.clearDisplay();
  
  // Segmenttest des Displays starten
  FourDigitLedDisplay.testDisplaySegments(1);
  
  // nach einer Sekunde den Segmenttest beenden
  delay(1000);
  FourDigitLedDisplay.testDisplaySegments(0);
}

void loop()
{}
