#include "OWBusMaster.h"

OWBusMaster::OWBusMaster(uint8_t pin): OneWire(pin) {
  this->OWDevices = new std::vector<OWDevice_t>{};
}

char* OWBusMaster::print_address(OWDevice_t* address) {
  char buffer[50] = {0};
  memset(buffer, 0, sizeof(buffer));
  snprintf (buffer, sizeof(buffer), "[0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X]", 
      address->OWDevice[0], 
      address->OWDevice[1], 
      address->OWDevice[2], 
      address->OWDevice[3], 
      address->OWDevice[4], 
      address->OWDevice[5], 
      address->OWDevice[6], 
      address->OWDevice[7]);
  return buffer;
}

const char* OWBusMaster::print_devices() {
  String ret="";
  for (uint8_t i = 0; i < this->OWDevices->size(); i++) {
    ret.concat(this->print_address(&this->OWDevices->at(i)));
    if (i < this->OWDevices->size()+1) { ret.concat(", "); }
  }
  return ret.c_str();
}

uint8_t  OWBusMaster::find() {
    OWDevice_t d = {};
    this->reset_search();
    delay(250);
    
    while(this->search(d.OWDevice)) {
      if(d.OWDevice[0] == DS2408_FAMILY) {
        Serial.println("Count!!");
        this->OWDevices->push_back(d);
      }
    }
    
    this->reset_search();
    delay(250);

    return OWDevices->size();
}
