#ifndef __DIGITAL_LED__
#define __DIGITAL_LED__

#include "HwComponent.h"

#define DELTA_BLINK 50

//allow to perform simple operations on a digital led
class DigitalLed : public HwComponent
{
  public:
    //constructor: selected_pin(pin number where component is wired)
    DigitalLed(char selected_pin);

    //turns on the led
    void on();

    //turns off the led
    void off();

    //blink the led one time; after turning on the led waits DELTA_BLINK to turn it off
    void blinky();
};

#endif
