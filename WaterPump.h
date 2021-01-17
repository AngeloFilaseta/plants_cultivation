#ifndef __WATER_PUMP__
#define __WATER_PUMP__

#include "HwComponent.h"

//class used to perform operations with servomotor
class WaterPump : public HwComponent
{
  public:
    //constructor: selected_pin(pin number where component is wired)
    WaterPump(char selected_pin);

    void init();
    
    void send_water();
};

#endif
