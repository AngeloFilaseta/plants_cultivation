#define DELTA_DELAY 60000 //60 seconds delay

//PINS
#define DHT11_PIN 3

//TEMP LEDS
#define HOT_LED_PIN 4
#define OK_TEMP_LED_PIN 5
#define COLD_LED_PIN 6

//HUMIDITY LEDS
#define LOW_LED_PIN 8
#define OK_HUM_LED_PIN 9
#define HIGH_LED_PIN 10

//VALUES
#define ERROR_IN_SENSOR -999
#define MAX_TEMP 30
#define MIN_TEMP 20
#define MAX_HUM  70
#define MIN_HUM 40

//OTHERS
#define N_LEDS 3

#include "HumiditySensor.h"
#include "DigitalLed.h"

char night_mode = 0;

HumiditySensor* hum_sensor = new HumiditySensor(DHT11_PIN);
DigitalLed* cold_led = new DigitalLed(COLD_LED_PIN);
DigitalLed* ok_temp_led = new DigitalLed(OK_TEMP_LED_PIN);
DigitalLed* hot_led = new DigitalLed(HOT_LED_PIN);

DigitalLed* low_led = new DigitalLed(LOW_LED_PIN);
DigitalLed* ok_hum_led = new DigitalLed(OK_HUM_LED_PIN);
DigitalLed* high_led = new DigitalLed(HIGH_LED_PIN);


DigitalLed* temp_leds [] = {cold_led, ok_temp_led, hot_led};
DigitalLed* hum_leds [] = {low_led, ok_hum_led, high_led};

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
  temp_led_on(temp);
  hum_led_on(hum);
}

void error_check(int temp, int hum)
{
  if(temp == ERROR_IN_SENSOR || temp == ERROR_IN_SENSOR)
  {
    all_led_blink();
  }
}

void temp_led_on(int temp)
{
  if(temp > MAX_TEMP)
  {
    hot_led->on();
  }
  else if(temp < MIN_TEMP)
  {
    cold_led->on();
  }else{
    ok_temp_led->on();
  }
}

void hum_led_on(int hum)
{
  if(hum > MAX_HUM)
  {
    high_led->on();
  }
  else if(hum < MIN_HUM)
  {
    low_led->on();
  }else{
    ok_hum_led->on();
  }
}

//turn off all the leds at once
void all_led_off()
{
    for (int i = 0; i < N_LEDS; i++) {
      temp_leds[i]->off();
      hum_leds[i]->off();
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
      temp_leds[i]->on();
      hum_leds[i]->on();
   }
}
