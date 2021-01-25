#include <Arduino.h>
#include "SoilSensor.h"

#define WET_VALUE 280
#define DRY_VALUE 540

//constructor: selected_pin(pin number where component is wired)
SoilSensor::SoilSensor(char selected_pin) : HwComponent(selected_pin){}

int SoilSensor::get_humidity()
{
    int soilMoistureValue = map(analogRead(HwComponent::get_pin()), WET_VALUE, DRY_VALUE, 100, 0);
    if(soilMoistureValue > 100){
      soilMoistureValue = 100;
    }
    else if(soilMoistureValue < 0)
    {
      soilMoistureValue = 0;
    }
    return soilMoistureValue;
}
