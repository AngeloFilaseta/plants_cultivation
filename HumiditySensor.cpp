#include <Arduino.h>
#include "HumiditySensor.h"

//constructor: selected_pin(pin number where component is wired)
HumiditySensor::HumiditySensor(char selected_pin) : HwComponent(selected_pin){}

int HumiditySensor::init()
{
  return DHT.read11(this->get_pin());
}

int HumiditySensor::get_temp()
{
  return DHT.temperature;
}

int HumiditySensor::get_humidity()
{
  return DHT.humidity;
}
