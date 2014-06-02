#ifndef SAA1064_H
#define SAA1064_H

#include <Arduino.h>

/**********************************************************
Klasse SAA1064
**********************************************************/
class SAA1064
{
  private:
    uint8_t address;
    uint8_t controlRegister;

    void writeControlRegister(void);

  public:
    enum Subaddress
    {
      SUBADDRESS_CONTROLREGISTER = 0x00,
      SUBADDRESS_DIGIT_1,
      SUBADDRESS_DIGIT_2,
      SUBADDRESS_DIGIT_3,
      SUBADDRESS_DIGIT_4
    };

    enum Mode
    {
      STATIC_MODE   = (0 << 0),
      DYNAMIC_MODE  = (1 << 0)
    };

    enum ActiveDigits
    {
      DIGITS_1_AND_3 = 1,
      DIGITS_2_AND_4 = 2
    };

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

    SAA1064();

    void begin(uint8_t i2cAddress);
    void begin(uint8_t i2cAddress, Mode mode, OutputCurrent outputCurrent);
    
    void clear(void);
    void activeDigits(ActiveDigits blankedDigits, uint8_t active);
    void segmentTest(uint8_t active);

    void setMode(Mode mode);
    void setOutputCurrent(OutputCurrent outputCurrent);

    void writeDigits(Subaddress subaddress, uint8_t *data, uint8_t datalength);
};

extern SAA1064 LedDriver;

#endif /* SAA1064_H */