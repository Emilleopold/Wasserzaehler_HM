/******************************************************************************
Beispiel:      stopWatch

Beschreibung:  dieses Beispiel zeigt die Darstellung der Zahlen auf dem Display
               Anhand einer Stoppuhr.
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
const uint8_t i2cAddressSAA1064     = 0x70;

// Frequenz des Oszillators in Hz -> 16 MHz
const uint32_t crystalFrequency     = 16000000UL;

// Frequenz des Timer 1 in Hz -> 10 Hz
const uint8_t timerFrequency        = 10UL;

// Pinbelegung der Tasten für Start, Stopp und Reset
const uint8_t pinStartButton        = 4;
const uint8_t pinStopButton         = 5;
const uint8_t pinResetButton        = 6;

/******************************************************************************
globale Variablen
******************************************************************************/
// Anzeige, ob Stoppuhr steht (0) oder läuft (1)
uint8_t running = 0;

// vergangen Zeit in 0,1 s Schritten
uint16_t time = 0;

// Flag zur Anzeige, dass der Timerinterrupt aktiviert wurde
volatile uint8_t timerflag = 0;

/******************************************************************************
Funktionen
******************************************************************************/
// Initialisierung des Timer 1
void timerInit()
{
  // Modus 4: CTC
  TCCR1A = (0 << WGM11) | (0 << WGM10);
  TCCR1B = (0 << WGM13) | (1 << WGM12);
 
  // Counter zurücksetzen
  TCNT1  = 0;
  
  // Prescaler: 256
  TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);

  // Output Compare Register für 10 Hz bei einem Prescaler von 256 einstellen
  OCR1A = (uint16_t)(crystalFrequency / (timerFrequency * 256UL)) - 1;
  
  //Compare Match Flag zurücksetzen
  TIFR1 = (1 << OCF1A);
  
  // Compare Match Interrupt aktivieren
  TIMSK1 |= (1 << OCIE1A); 
}

// Interrupt-Routine des Timer 1, welcher im 0,1s-Takt auslöst
ISR(TIMER1_COMPA_vect)
{
  // Timerflag setzen, so dass die loop-Routine erkennt, dass 0,1 s vergangen sind
  timerflag = 1;
}

void setup() 
{
  // Timer initialisieren
  noInterrupts();
  timerInit();
  interrupts();

  // I2C-Modul initialisieren
  Wire.begin();
  
  // 4-Digit-LED-Anzeige mit I2C-Adresse des SAA1064 initialisieren
  FourDigitLedDisplay.begin(i2cAddressSAA1064);
  
  // Pins für die Taster als Eingänge setzen
  pinMode(pinStartButton, INPUT);
  pinMode(pinStopButton, INPUT);
  pinMode(pinResetButton, INPUT);
  
  // interne Pullup-Widerstände für die Tastereingänge aktivieren
  digitalWrite(pinStartButton, HIGH);
  digitalWrite(pinStopButton, HIGH);
  digitalWrite(pinResetButton, HIGH);
}

void loop()
{
  // alle 0,1 s die Zeit inkrementieren, die neue Zeit ausgeben und
  // die Taster auswerten.
  if(timerflag == 1)
  {
    timerflag = 0;
    
    // die Zeit nur erhöhen, wenn die Stoppuhr aktiviert ist
    if(running == 1)
    {
      time++;
    }
    
    // Zeit auf dem Display ausgeben
    FourDigitLedDisplay.writeDecimal(time, 1);
    
    // Stoppuhr laufen lassen, wenn der Start-Taster
    // gedrückt wurde
    if(digitalRead(pinStartButton) == 0)
    {
      running = 1;
    }

    // Stoppuhr anhalten, wenn der Stopp-Taster
    // gedrückt wurde    
    if(digitalRead(pinStopButton) == 0)
    {
      running = 0;
    }
    
    // Stoppuhr anhalten und zurücksetzen, wenn
    // der Reset-Taster gedrückt wurde
    if(digitalRead(pinResetButton) == 0)
    {
      running = 0;
      time = 0;
    }
  }
}
