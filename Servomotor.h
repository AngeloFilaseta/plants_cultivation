#ifndef __SERVOMOTOR__
#define __SERVOMOTOR__

#include "HwComponent.h"
#include <Servo.h>

//class used to perform operations with servomotor
class Servomotor : public HwComponent
{
  public:
    //constructor: selected_pin(pin number where component is wired)
    Servomotor(char selected_pin);

    //attaches the servomotor
    void turn_on();

    //detaches the servomotor (used primarily to eliminate servomotor noise while servomotor stays still)
    void turn_off();

    //resets the servomotor positioning it at degree 0
    void set_to_min();

    //moves to max degree
    char set_to_max();

    //set to a specific degree
    char set_to(int degree);

  private:
    Servo myservo;
};

#endif
