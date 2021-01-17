#ifndef __SOIL_SENSOR__
#define __SOIL_SENSOR__

#include "HwComponent.h"

class SoilSensor : public HwComponent
{
  public:
    //constructor: selected_pin(pin number where component is wired)
    SoilSensor(char selected_pin);

    int get_humidity();
};

#endif
