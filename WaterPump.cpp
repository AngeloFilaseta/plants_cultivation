#include <Arduino.h>
#include "WaterPump.h"

#define DELTA_WATER 3000

//constructor: selected_pin(pin number where component is wired)
WaterPump::WaterPump(char selected_pin) : HwComponent(selected_pin){}

void WaterPump::init()
{
  digitalWrite(HwComponent::get_pin(), LOW);
}

void WaterPump::send_water()
{
  digitalWrite(HwComponent::get_pin(), HIGH);
  Serial.print("\nSending water");
  delay(DELTA_WATER);
  digitalWrite(HwComponent::get_pin(), LOW);
}
