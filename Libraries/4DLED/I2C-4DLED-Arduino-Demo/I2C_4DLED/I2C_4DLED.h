#ifndef I2C_4DLED_H
#define I2C_4DLED_H

#include <Arduino.h>
#include "utility/SAA1064.h"

/**********************************************************
Klasse I2C_4DLED
**********************************************************/
class I2C_4DLED
{
private:
  void calcDigits(uint8_t *digits, int decimal, uint8_t minimumDigitCount, uint8_t base);

public:

  enum OutputCurrent
  {
    OUTPUT_CURRENT_0_MA   = ((0 << 6) | (0 << 5) | (0 << 4)),
    OUTPUT_CURRENT_3_MA   = ((0 << 6) | (0 << 5) | (1 << 4)),
    OUTPUT_CURRENT_6_MA   = ((0 << 6) | (1 << 5) | (0 << 4)),
    OUTPUT_CURRENT_9_MA   = ((0 << 6) | (1 << 5) | (1 << 4)),
    OUTPUT_CURRENT_12_MA  = ((1 << 6) | (0 << 5) | (0 << 4)),
    OUTPUT_CURRENT_15_MA  = ((1 << 6) | (0 << 5) | (1 << 4)),
    OUTPUT_CURRENT_18_MA  = ((1 << 6) | (1 << 5) | (0 << 4)),
    OUTPUT_CURRENT_21_MA  = ((1 << 6) | (1 << 5) | (1 << 4))
  };

	I2C_4DLED();
	void begin(uint8_t i2cAddressSAA1064);  
  void clearDisplay(void);
  void testDisplaySegments(uint8_t active);
  void setDisplayOutputCurrent(OutputCurrent outputCurrent);
  void writeDecimal(int16_t decimal);
  void writeDecimal(int16_t decimal, uint8_t decimalDigits);
  void writeHexadecimal(uint16_t hexadecimal);
};

extern I2C_4DLED FourDigitLedDisplay;

#endif /* I2C_4DLED_H */
