// https://codebender.cc/example/OneWire/DS2408_Switch#DS2408_Switch.ino
// https://forum.arduino.cc/index.php?topic=287552.msg2014060#msg2014060
// https://github.com/queezythegreat/arduino-ds2408/blob/master/DS2408/DS2408.h

#ifndef ONEWIRE2408_H
#define ONEWIRE2408_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "OneWire.h"

class OWBusMaster : public OneWire {

  //typedef uint8_t  OWDevice_t[8];
    typedef struct { byte OWDevice[8]; } OWDevice_t;
    
//  typedef Device*   Devices;
//  typedef uint16_t Register;
//  typedef uint8_t  CRC8;
//  typedef uint16_t CRC16;

  public:
    OWBusMaster(uint8_t pin);

    
    
  private:
    uint8_t       find();
    char*         print_address(OWDevice_t* address);
    const char*   print_devices();
    
    const uint8_t DS2408_FAMILY = 0x29;
    std::vector<OWDevice_t>* OWDevices = NULL;
  
};

#endif
