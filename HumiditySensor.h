#ifndef __HUM_SENSOR__
#define __HUM_SENSOR__

#include <dht.h>
#include "HwComponent.h"

//class used to perform operations with servomotor
class HumiditySensor : public HwComponent
{
  public:
    //constructor: selected_pin(pin number where component is wired)
    HumiditySensor(char selected_pin);

    int init();
    
    int get_temp();

    int get_humidity();

  private:
    dht DHT;
};

#endif
