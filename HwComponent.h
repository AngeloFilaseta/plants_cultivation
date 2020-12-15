#include <DHT.h>
#include <DHT_U.h>

#ifndef __HW_COMPONENT__
#define __HW_COMPONENT__

//parent class of every hardware component
class HwComponent
{
  public:
    //constructor: selected_pin(pin number where component is wired)
    HwComponent(char selected_pin);

    //returns the pin which this hardware component is wired to
    char get_pin();
  private:
	  char pin;
};

#endif
