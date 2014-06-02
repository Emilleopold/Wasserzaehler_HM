#ifndef MCP9801_H
#define MCP9801_H

#include <Arduino.h>

/**********************************************************
Klasse MCP9801
**********************************************************/
class MCP9801
{
  private:
    uint8_t address;
    uint8_t configRegister;

    void writeConfigRegister(void);
    void readConfigRegister(void);

public:
  enum Register
  {
    TEMPERATURE_REGISTER              = ((0 << 1) | (0 << 0)),
    CONFIG_REGISTER                   = ((0 << 1) | (1 << 0)),
    TEMPERATURE_HYSTERESIS_REGISTER   = ((1 << 1) | (0 << 0)),
    TEMPERATURE_LIMIT_SET_REGISTER    = ((1 << 1) | (1 << 0))
  };

  enum OneShot
  {
    ONESHOT_DISABLE  = (0 << 7),
    ONESHOT_ENABLE   = (1 << 7)
  };
  
  enum ADCResolution
  {
    RESOLUTION_9BIT   = ((0 << 6) | (0 << 5)),
    RESOLUTION_10BIT  = ((0 << 6) | (1 << 5)),
    RESOLUTION_11BIT  = ((1 << 6) | (0 << 5)),
    RESOLUTION_12BIT  = ((1 << 6) | (1 << 5))
  };

  enum FaultQueue
  {
    FAULTQUEUE_1  = ((0 << 4) | (0 << 3)),
    FAULTQUEUE_2  = ((0 << 4) | (1 << 3)),
    FAULTQUEUE_4  = ((1 << 4) | (0 << 3)),
    FAULTQUEUE_8  = ((1 << 4) | (1 << 3))
  };

  enum AlertPolarity
  {
    POLARITY_ACTIVE_LOW   = (0 << 2),
    POLARITY_ACTIVE_HIGH  = (1 << 2)
  };

  enum Mode
  {
    COMPARATOR_MODE   = (0 << 1),
    INTERRUPT_MODE    = (1 << 1)
  };

  enum Shutdown
  {
    SHUTDOWN_DISABLE   = (0 << 0),
    SHUTDOWN_ENABLE    = (1 << 0)
  };

  MCP9801();

  void begin(uint8_t);
  void begin(uint8_t, OneShot, ADCResolution, FaultQueue, AlertPolarity, Mode, Shutdown);

  void setOneShot(OneShot);
  OneShot getOneShot(void);
  
  void setADCResolution(ADCResolution);
  ADCResolution getADCResolution(void);

  void setFaultQueue(FaultQueue);
  FaultQueue getFaultQueue(void);

  void setAlertPolarity(AlertPolarity);
  AlertPolarity getAlertPolarity(void);

  void setMode(Mode);
  Mode getMode(void);

  void setShutdown(Shutdown);
  Shutdown getShutdown(void);

  int32_t readTemperature(void);

  void setTemperatureHysteresis(int16_t);
  int16_t getTemperatureHysteresis(void);

  void setTemperatureLimit(int16_t);
  int16_t getTemperatureLimit(void);

};

extern MCP9801 TemperatureSensor;

#endif /* MCP9801_H */