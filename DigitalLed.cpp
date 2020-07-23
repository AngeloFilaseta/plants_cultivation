#include <Arduino.h>
#include "DigitalLed.h"

//constructor: selected_pin(pin number where component is wired)
DigitalLed::DigitalLed(char selected_pin) : HwComponent(selected_pin)
{
  pinMode(selected_pin, OUTPUT);
}

//turns on the led
void DigitalLed::on()
{
  digitalWrite(HwComponent::get_pin(), HIGH);
}

//turns off the led
void DigitalLed::off()
{
  digitalWrite(HwComponent::get_pin(), LOW);
}

//blink the led one time; after turning on the led waits DELTA_BLINK to turn it off
void DigitalLed::blinky()
{
  this->on();
  delay(DELTA_BLINK);
  this->off();
}
