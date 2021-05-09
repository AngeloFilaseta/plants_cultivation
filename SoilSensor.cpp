#include <Arduino.h>
#include "SoilSensor.h"

//constructor: selected_pin(pin number where component is wired)
SoilSensor::SoilSensor(char selected_pin) : HwComponent(selected_pin){}

int SoilSensor::get_humidity()
{
    int analog_value = analogRead(HwComponent::get_pin());
    return analog_value;
}
