#include <Arduino.h>
#include "Servomotor.h"

#define START_POSITION 0

#define END_POSITION 100

//constructor: selected_pin(pin number where component is wired)
Servomotor::Servomotor(char selected_pin) : HwComponent(selected_pin){}

//attaches the servomotor
void Servomotor::turn_on()
{
  this -> myservo.attach(HwComponent::get_pin());
}

//detaches the servomotor (used primarily to eliminate servomotor noise while servomotor stays still)
void Servomotor::turn_off()
{
  this -> myservo.detach();
}


//resets the servomotor positioning it at degree 0
void Servomotor::set_to_min()
{ 
  this -> myservo.write(0);
}

//moves to max degree
char Servomotor::set_to_max()
{
  this -> myservo.write(END_POSITION);
}

//set to a specific degree    
char Servomotor::set_to(int degree)
{
  this -> myservo.write(degree);
}
