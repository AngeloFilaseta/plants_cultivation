#include "HwComponent.h"

//constructor: selected_pin(pin number where component is wired)
HwComponent::HwComponent(char selected_pin)
{
  this -> pin = selected_pin;
}

//returns the pin which this hardware component is wired to
char HwComponent::get_pin(){
	return this -> pin;
}