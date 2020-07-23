#define DELTA_DELAY 20000 //20 seconds delay

//PINS
#define DHT11_PIN 3
#define HOT_LED_PIN 4
#define OK_LED_PIN 5
#define COLD_LED_PIN 6

//VALUES
#define ERROR_IN_SENSOR -999
#define MAX_TEMP 30
#define MIN_TEMP 20

//OTHERS
#define N_LEDS 3

#include "HumiditySensor.h"
#include "DigitalLed.h"

char night_mode = 0;

HumiditySensor* hum_sensor = new HumiditySensor(DHT11_PIN);
DigitalLed* cold_led = new DigitalLed(COLD_LED_PIN);
DigitalLed* ok_led = new DigitalLed(OK_LED_PIN);
DigitalLed* hot_led = new DigitalLed(HOT_LED_PIN);
DigitalLed* leds [] = {cold_led, ok_led, hot_led};

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int chk = hum_sensor->init();
  int temp = hum_sensor -> get_temp();
  int hum = hum_sensor -> get_humidity();
  print_data_on_serial(temp, hum);
  power_led_on(temp, hum);
  delay(DELTA_DELAY);
 
}

//power on only the right led, based on temperature and humidity
void power_led_on(int temp, int hum)
{
  all_led_off();
  if(temp == ERROR_IN_SENSOR)
  {
    all_led_blink();
  }
  else if(temp > MAX_TEMP)
  {
    hot_led->on();
  }
  else if(temp < MIN_TEMP)
  {
    cold_led->on();
  }else{
    ok_led->on();
  }
}

//turn off all the leds at once
void all_led_off()
{
    for (int i = 0; i < N_LEDS; i++) {
      leds[i]->off();
   }
}

void print_data_on_serial(int temp, int humidity)
{
  Serial.print("\nTemperature: ");
  Serial.print(temp);
  Serial.print("C°");
  Serial.print("\nHumidity: ");
  Serial.print(humidity);
  Serial.print("%");
}

void all_led_blink()
{
    for (int i = 0; i < N_LEDS; i++) {
      leds[i]->on();
   }
}
